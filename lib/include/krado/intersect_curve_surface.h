#pragma once

#include "krado/point.h"
#include "krado/geom_curve.h"
#include "krado/geom_surface.h"

namespace krado {

class SurfaceFunctor {
public:
    virtual ~SurfaceFunctor() {}
    virtual Point operator()(UVParam param) const = 0;
};

class CurveFunctor {
public:
    virtual ~CurveFunctor() {}
    virtual Point operator()(double t) const = 0;
};

class SurfaceFunctorGFace : public SurfaceFunctor {
    const GeomSurface & gsurface_;

public:
    SurfaceFunctorGFace(const GeomSurface & gf) : gsurface_(gf) {}

    Point
    operator()(UVParam param) const override
    {
        return this->gsurface_.point(param);
    }
};

class CurveFunctorGEdge : public CurveFunctor {
    const GeomCurve & gcurve_;

public:
    CurveFunctorGEdge(const GeomCurve & gcurve) : gcurve_(gcurve) {}

    Point
    operator()(double t) const override
    {
        return this->gcurve_.point(t);
    }
};

class CurveFunctorCircle : public CurveFunctor {
    Vector n1_, n2_;
    Point middle_;
    double d_;

public:
    CurveFunctorCircle(const Vector & n1,
                       const Vector & n2,
                       const Point & middle,
                       const double & d) :
        n1_(n1),
        n2_(n2),
        middle_(middle),
        d_(d)
    {
    }

    Point
    operator()(double t) const override
    {
        auto dir = (this->n1_ * std::cos(t) + this->n2_ * std::sin(t)) * this->d_;
        return this->middle_ + dir;
    }
};

// intersects the curve and the surface using Newton.
// the initial guess should be a good guess
// returns 1 --> OK
// returns 0 --> NOT CONVERGED
int intersect_curve_surface(CurveFunctor & c, SurfaceFunctor & s, double uvt[3], double epsilon);

} // namespace krado
