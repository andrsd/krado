// SPDX-FileCopyrightText: Copyright (C) 1997-2024 C. Geuzaine, J.-F. Remacle
// SPDX-License-Identifier: GPL-2.0-or-later
//
// This is a rewrite of `meshGEdge` from gmsh

#include "krado/scheme_transfinite.h"
#include "krado/geom_curve.h"
#include "krado/mesh_curve.h"
#include "krado/mesh_curve_vertex.h"
#include "krado/numerics.h"
#include "krado/utils.h"
#include <array>

namespace krado {

namespace {

SchemeTransfinite::Type
get_sub_type(const std::string & str_type)
{
    auto lc_type = utils::to_lower(str_type);
    if (lc_type == "progression")
        return SchemeTransfinite::PROGRESSION;
    else if (lc_type == "bump")
        return SchemeTransfinite::BUMP;
    else if (lc_type == "beta-law")
        return SchemeTransfinite::BETA_LAW;
    else if (lc_type == "size-map")
        return SchemeTransfinite::SIZE_MAP;
    else
        throw Exception("Unknown type of transfinite scheme: '{}'", str_type);
}

// Transfinite (prescribed number of edges) but the points are positioned
// according to the standard size constraints (size map, etc)
class FLc {
public:
    double
    operator()(const GeomCurve & curve, double t)
    {
        auto p = curve.point(t);
        auto [t_begin, t_end] = curve.param_range();
        double lc_here = 1.e22;

        if (t == t_begin)
            lc_here = 0.1;
        else if (t == t_end)
            lc_here = 0.5;

        // if(t == t_begin && ge->getBeginVertex())
        //     lc_here = BGM_MeshSize(ge->getBeginVertex(), t, 0, p.x(), p.y(), p.z());
        // else if(t == t_end && ge->getEndVertex())
        //     lc_here = BGM_MeshSize(ge->getEndVertex(), t, 0, p.x(), p.y(), p.z());
        //
        // lc_here = std::min(lc_here, BGM_MeshSize(ge, t, 0, p.x(), p.y(), p.z()));

        double lc = 1.;
        double lc_min = 0;
        double lc_max = 1.e22;
        lc = std::max(lc, lc_min);
        lc = std::min(lc, lc_max);

        lc_here = lc;

        // std::cerr << "lc = " << lc_here << std::endl;

        auto der = curve.d1(t);
        return der.norm() / lc_here;
    }
};

// Geometric progression ar^i; Sum of n terms = length = a (r^n-1)/(r-1)
class FTransfiniteProgression {
public:
    FTransfiniteProgression(double coef, int n_pts, int ori) : coef(coef), n_points(n_pts), ori(ori)
    {
    }

    double
    operator()(const GeomCurve & curve, double u)
    {
        double length = curve.length();

        double d = curve.d1(u).norm();

        if (this->coef == 1.) {
            return d * this->coef / curve.length();
        }
        else {
            auto [u_lower, u_upper] = curve.param_range();
            double t = (u - u_lower) / (u_upper - u_lower);

            double r = (sign(this->ori) >= 0) ? this->coef : 1. / this->coef;
            double a = length * (r - 1.) / (std::pow(r, this->n_points - 1.) - 1.);
            int i = (int) (std::log(t * length / a * (r - 1.) + 1.) / std::log(r));
            return d / (a * std::pow(r, (double) i));
        }
    }

private:
    /// Coefficient
    double coef;
    /// Number of points
    int n_points;
    /// Orientation of the curve
    int ori;
};

// "bump"
class FTransfiniteBump {
public:
    FTransfiniteBump(double coef, int n_pts) : coef(coef), n_points(n_pts) {}

    double
    operator()(const GeomCurve & curve, double u)
    {
        double length = curve.length();

        double d = curve.d1(u).norm();
        auto [u_lower, u_upper] = curve.param_range();
        double t = (u - u_lower) / (u_upper - u_lower);

        double a;
        if (this->coef > 1.0) {
            a = -4. * std::sqrt(this->coef - 1.) * std::atan2(1.0, std::sqrt(this->coef - 1.)) /
                ((double) this->n_points * length);
        }
        else {
            a = 2. * std::sqrt(1. - this->coef) *
                std::log(std::abs((1. + 1. / std::sqrt(1. - this->coef)) /
                                  (1. - 1. / std::sqrt(1. - this->coef)))) /
                ((double) this->n_points * length);
        }
        double b = -a * length * length / (4. * (this->coef - 1.));
        return d / (-a * std::pow(t * length - (length) * 0.5, 2) + b);
    }

public:
    /// Coefficient
    double coef;
    // Number of points
    int n_points;
};

/// Beta law
class FTransfiniteBetaLaw {
public:
    FTransfiniteBetaLaw(double coef, int ori) : coef(coef), ori(ori) {}

    double
    operator()(const GeomCurve & curve, double u)
    {
        auto [u_lower, u_hi] = curve.param_range();
        double t = (u - u_lower) / (u_hi - u_lower);

        if (this->coef < 1.) {
            auto d = curve.d1(u).norm();
            return d * this->coef / curve.length();
        }
        else {
            // "beta" law
            if (this->ori < 0)
                return dfbeta(1. - t, this->coef);
            else
                return dfbeta(t, this->coef);
        }
    }

private:
    /// Coefficient
    double coef;
    /// Orientation
    int ori;

    double
    dfbeta(double t, double beta)
    {
        double ratio = (1 + beta) / (beta - 1);
        double zlog = std::log(ratio);
        return 2 * beta / ((1 + beta - t) * (-1 + beta + t) * zlog);
    }
};

class Integral {
public:
    struct IntPoint {
        int num;
        // The local coordinate of the point
        double t;
        // lc is x'(t)/h(x(t))
        double lc;
        // The value of the primitive
        double p;
        // The norm of the tangent vector
        double xp;
    };

    double
    value() const
    {
        return this->val;
    }

    const IntPoint &
    point(int idx) const
    {
        return this->pts[idx];
    }

    template <typename FUNCTION>
    void
    integrate(const GeomCurve & curve, double t1, double t2, FUNCTION f)
    {
        this->depth = 0;

        IntPoint from;
        from.t = t1;
        from.lc = f(curve, from.t);
        from.p = 0.0;
        this->pts.push_back(from);

        IntPoint to;
        to.t = t2;
        to.lc = f(curve, to.t);

        integrate_recursively(curve, from, to, f);
        this->val = this->pts.back().p;
    }

private:
    double
    trapezoidal(const IntPoint & p1, const IntPoint & p2)
    {
        return 0.5 * (p1.lc + p2.lc) * (p2.t - p1.t);
    }

    template <typename FUNCTION>
    void
    integrate_recursively(const GeomCurve & curve, IntPoint & from, IntPoint & to, FUNCTION f)
    {
        this->depth++;

        IntPoint mid;
        mid.t = 0.5 * (from.t + to.t);
        mid.lc = f(curve, mid.t);

        double const val1 = trapezoidal(from, to);
        double const val2 = trapezoidal(from, mid);
        double const val3 = trapezoidal(mid, to);
        double const err = std::abs(val1 - val2 - val3);

        if (((err < this->precision) && (this->depth > 6)) || (this->depth > 25)) {
            IntPoint p1 = this->pts.back();
            mid.p = p1.p + val2;
            this->pts.push_back(mid);

            p1 = this->pts.back();
            to.p = p1.p + val3;
            this->pts.push_back(to);
        }
        else {
            integrate_recursively(curve, from, mid, f);
            integrate_recursively(curve, mid, to, f);
        }

        this->depth--;
    }

    /// Value of the integral
    double val;
    /// integration points
    std::vector<IntPoint> pts;
    /// resursive depth during integration
    int depth;
    ///
    double precision = 1e-8;
};

std::vector<MeshCurveVertex *>
build_curve_vertices(const GeomCurve & curve, const Integral & integral, int n)
{
    std::vector<MeshCurveVertex *> vertices;

    // do not consider the first and the last vertex (those are not classified on this mesh edge)
    if (n > 1) {
        const double b = integral.value() / static_cast<double>(n - 1);
        int i = 1;
        int n_points = 1;
        vertices.resize(n - 2);

        while (n_points < n - 1) {
            auto p1 = integral.point(i - 1);
            auto p2 = integral.point(i);
            const double d = (double) n_points * b;
            if ((std::abs(p1.p) < std::abs(d)) && (std::abs(d) <= std::abs(p2.p))) {
                double const dt = p2.t - p1.t;
                double const dlc = p2.lc - p1.lc;
                double const dp = p2.p - p1.p;
                double const t = p1.t + dt / dp * (d - p1.p);
                const double d1_mag = curve.d1(t).norm();
                double lc = d1_mag / (p1.lc + dlc / dp * (d1_mag - p1.p));
                vertices[n_points - 1] = new MeshCurveVertex(curve, t);
                // TODO: store Lc in mesh curve vertex
                n_points++;
            }
            else {
                i++;
            }
        }
        vertices.resize(n_points - 1);
    }

    return vertices;
}

} // namespace

SchemeTransfinite::SchemeTransfinite() : Scheme("transfinite")
{
    set("points", 2);
    set("coef", 1);
    set("type", std::string("progression"));
}

void
SchemeTransfinite::mesh_curve(MeshCurve & curve)
{
    auto & geom_curve = curve.geom_curve();

    auto [u_lower, u_upper] = geom_curve.param_range();
    auto subtype = get_sub_type(get<std::string>("type"));
    auto coeff = get<double>("coef");
    auto n_intervals = get<int>("intervals");
    auto n_pts = n_intervals + 1;

    Integral integral;
    if (subtype == PROGRESSION)
        integral.integrate(geom_curve, u_lower, u_upper, FTransfiniteProgression(coeff, n_pts, 1));
    else if (subtype == BUMP)
        integral.integrate(geom_curve, u_lower, u_upper, FTransfiniteBump(coeff, n_pts));
    else if (subtype == BETA_LAW)
        integral.integrate(geom_curve, u_lower, u_upper, FTransfiniteBetaLaw(coeff, 1));
    else if (subtype == SIZE_MAP)
        integral.integrate(geom_curve, u_lower, u_upper, FLc());

    std::vector<MeshCurveVertex *> curve_vtxs = build_curve_vertices(geom_curve, integral, n_pts);
    auto & bnd_verts = curve.bounding_vertices();
    curve.add_vertex(bnd_verts[0]);
    for (auto & cv: curve_vtxs)
        curve.add_vertex(cv);
    curve.add_vertex(bnd_verts[1]);

    for (int i = 0; i < n_intervals; i++)
        curve.add_segment({ i, i + 1 });
}

} // namespace krado
