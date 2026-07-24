// SPDX-FileCopyrightText: 2026 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include "krado/element.h"
#include "krado/mesh.h"
#include "krado/point.h"
#include "krado/vector.h"
#include "krado/quadrature.h"

namespace krado {

/// Finite element values
template <ElementType ET>
struct FEValues {
    /// Compute shape function value
    ///
    /// @param i Node index
    /// @param p Point in reference space
    /// @return Shape function value
    static double shape_val(u8 i, const Point & p);

    /// Compute shape function derivative
    ///
    /// @param i Node index
    /// @param p Point in reference space
    /// @return Shape function derivative
    static Vector shape_der(u8 i, const Point & p);

    /// Compute Jacobian determinant
    ///
    /// @param elem Element
    /// @param mesh Mesh
    /// @param p Point in reference space
    /// @return Jacobian determinant
    static double compute_det_J(const Element & elem, const Mesh & mesh, const Point & p);
};

// LINE2
template <>
struct FEValues<ElementType::LINE2> {
    static double
    shape_val(u8 i, const Point & p)
    {
        const double xi = p.x;
        switch (i) {
        case 0:
            return 0.5 * (1. - xi);
        case 1:
            return 0.5 * (1. + xi);
        default:
            return 0.;
        }
    }

    static Vector
    shape_der(u8 i, const Point & p)
    {
        (void) p;
        switch (i) {
        case 0:
            return Vector(-0.5, 0., 0.);
        case 1:
            return Vector(0.5, 0., 0.);
        default:
            return Vector(0., 0., 0.);
        }
    }

    static double
    compute_det_J(const Element & elem, const Mesh & mesh, const Point & p)
    {
        Vector dxdxi(0, 0, 0);
        auto idxs = elem.indices();
        for (u8 i = 0; i < Line2::N_VERTICES; ++i) {
            const auto deriv = shape_der(i, p);
            const Vector pt(mesh.point(idxs[i]));
            dxdxi += deriv.x * pt;
        }
        return dxdxi.magnitude();
    }
};

// TRI3
template <>
struct FEValues<ElementType::TRI3> {
    static double
    shape_val(u8 i, const Point & p)
    {
        const double xi = p.x;
        const double eta = p.y;
        switch (i) {
        case 0:
            return 1. - xi - eta;
        case 1:
            return xi;
        case 2:
            return eta;
        default:
            return 0.;
        }
    }

    static Vector
    shape_der(u8 i, const Point & p)
    {
        (void) p;
        switch (i) {
        case 0:
            return Vector(-1., -1., 0.);
        case 1:
            return Vector(1., 0., 0.);
        case 2:
            return Vector(0., 1., 0.);
        default:
            return Vector(0., 0., 0.);
        }
    }

    static double
    compute_det_J(const Element & elem, const Mesh & mesh, const Point & p)
    {
        Vector dxdxi(0, 0, 0);
        Vector dxdeta(0, 0, 0);
        auto idxs = elem.indices();
        for (u8 i = 0; i < Tri3::N_VERTICES; ++i) {
            const auto der = shape_der(i, p);
            const Vector pt(mesh.point(idxs[i]));
            dxdxi += der.x * pt;
            dxdeta += der.y * pt;
        }
        return cross_product(dxdxi, dxdeta).magnitude();
    }
};

// QUAD4
template <>
struct FEValues<ElementType::QUAD4> {
    static double
    shape_val(u8 i, const Point & p)
    {
        const double xi = p.x;
        const double eta = p.y;
        switch (i) {
        case 0:
            return 0.25 * (1. - xi) * (1. - eta);
        case 1:
            return 0.25 * (1. + xi) * (1. - eta);
        case 2:
            return 0.25 * (1. + xi) * (1. + eta);
        case 3:
            return 0.25 * (1. - xi) * (1. + eta);
        default:
            return 0.;
        }
    }

    static Vector
    shape_der(u8 i, const Point & p)
    {
        const double xi = p.x;
        const double eta = p.y;
        switch (i) {
        case 0:
            return Vector(-0.25 * (1. - eta), -0.25 * (1. - xi), 0.);
        case 1:
            return Vector(0.25 * (1. - eta), -0.25 * (1. + xi), 0.);
        case 2:
            return Vector(0.25 * (1. + eta), 0.25 * (1. + xi), 0.);
        case 3:
            return Vector(-0.25 * (1. + eta), 0.25 * (1. - xi), 0.);
        default:
            return Vector(0., 0., 0.);
        }
    }

    static double
    compute_det_J(const Element & elem, const Mesh & mesh, const Point & p)
    {
        Vector dxdxi(0, 0, 0);
        Vector dxdeta(0, 0, 0);
        auto idxs = elem.indices();
        for (u8 i = 0; i < Quad4::N_VERTICES; ++i) {
            const auto der = shape_der(i, p);
            const Vector pt(mesh.point(idxs[i]));
            dxdxi += der.x * pt;
            dxdeta += der.y * pt;
        }
        return cross_product(dxdxi, dxdeta).magnitude();
    }
};

// TETRA4
template <>
struct FEValues<ElementType::TETRA4> {
    static double
    shape_val(u8 i, const Point & p)
    {
        const double xi = p.x;
        const double eta = p.y;
        const double zeta = p.z;
        switch (i) {
        case 0:
            return 1. - xi - eta - zeta;
        case 1:
            return xi;
        case 2:
            return eta;
        case 3:
            return zeta;
        default:
            return 0.;
        }
    }

    static Vector
    shape_der(u8 i, const Point & p)
    {
        (void) p;
        switch (i) {
        case 0:
            return Vector(-1., -1., -1.);
        case 1:
            return Vector(1., 0., 0.);
        case 2:
            return Vector(0., 1., 0.);
        case 3:
            return Vector(0., 0., 1.);
        default:
            return Vector(0., 0., 0.);
        }
    }

    static double
    compute_det_J(const Element & elem, const Mesh & mesh, const Point & p)
    {
        Vector dxdxi(0, 0, 0);
        Vector dxdeta(0, 0, 0);
        Vector dxdzeta(0, 0, 0);
        auto idxs = elem.indices();
        for (u8 i = 0; i < Tetra4::N_VERTICES; ++i) {
            const auto der = shape_der(i, p);
            const Vector pt(mesh.point(idxs[i]));
            dxdxi += der.x * pt;
            dxdeta += der.y * pt;
            dxdzeta += der.z * pt;
        }
        return std::abs(dot_product(dxdxi, cross_product(dxdeta, dxdzeta)));
    }
};

// HEX8
template <>
struct FEValues<ElementType::HEX8> {
    static double
    shape_val(u8 i, const Point & p)
    {
        const double xi = p.x;
        const double eta = p.y;
        const double zeta = p.z;
        switch (i) {
        case 0:
            return 0.125 * (1. - xi) * (1. - eta) * (1. - zeta);
        case 1:
            return 0.125 * (1. + xi) * (1. - eta) * (1. - zeta);
        case 2:
            return 0.125 * (1. + xi) * (1. + eta) * (1. - zeta);
        case 3:
            return 0.125 * (1. - xi) * (1. + eta) * (1. - zeta);
        case 4:
            return 0.125 * (1. - xi) * (1. - eta) * (1. + zeta);
        case 5:
            return 0.125 * (1. + xi) * (1. - eta) * (1. + zeta);
        case 6:
            return 0.125 * (1. + xi) * (1. + eta) * (1. + zeta);
        case 7:
            return 0.125 * (1. - xi) * (1. + eta) * (1. + zeta);
        default:
            return 0.;
        }
    }

    static Vector
    shape_der(u8 i, const Point & p)
    {
        const double xi = p.x;
        const double eta = p.y;
        const double zeta = p.z;
        switch (i) {
        case 0:
            return Vector(-0.125 * (1. - eta) * (1. - zeta),
                          -0.125 * (1. - xi) * (1. - zeta),
                          -0.125 * (1. - xi) * (1. - eta));
        case 1:
            return Vector(0.125 * (1. - eta) * (1. - zeta),
                          -0.125 * (1. + xi) * (1. - zeta),
                          -0.125 * (1. + xi) * (1. - eta));
        case 2:
            return Vector(0.125 * (1. + eta) * (1. - zeta),
                          0.125 * (1. + xi) * (1. - zeta),
                          -0.125 * (1. + xi) * (1. + eta));
        case 3:
            return Vector(-0.125 * (1. + eta) * (1. - zeta),
                          0.125 * (1. - xi) * (1. - zeta),
                          -0.125 * (1. - xi) * (1. + eta));
        case 4:
            return Vector(-0.125 * (1. - eta) * (1. + zeta),
                          -0.125 * (1. - xi) * (1. + zeta),
                          0.125 * (1. - xi) * (1. - eta));
        case 5:
            return Vector(0.125 * (1. - eta) * (1. + zeta),
                          -0.125 * (1. + xi) * (1. + zeta),
                          0.125 * (1. + xi) * (1. - eta));
        case 6:
            return Vector(0.125 * (1. + eta) * (1. + zeta),
                          0.125 * (1. + xi) * (1. + zeta),
                          0.125 * (1. + xi) * (1. + eta));
        case 7:
            return Vector(-0.125 * (1. + eta) * (1. + zeta),
                          0.125 * (1. - xi) * (1. + zeta),
                          0.125 * (1. - xi) * (1. + eta));
        default:
            return Vector(0., 0., 0.);
        }
    }

    static double
    compute_det_J(const Element & elem, const Mesh & mesh, const Point & p)
    {
        Vector dxdxi(0, 0, 0);
        Vector dxdeta(0, 0, 0);
        Vector dxdzeta(0, 0, 0);
        auto idxs = elem.indices();
        for (u8 i = 0; i < Hex8::N_VERTICES; ++i) {
            const auto der = shape_der(i, p);
            const Vector pt(mesh.point(idxs[i]));
            dxdxi += der.x * pt;
            dxdeta += der.y * pt;
            dxdzeta += der.z * pt;
        }
        return std::abs(dot_product(dxdxi, cross_product(dxdeta, dxdzeta)));
    }
};

// PRISM6
template <>
struct FEValues<ElementType::PRISM6> {
    static double
    shape_val(u8 i, const Point & p)
    {
        const double xi = p.x;
        const double eta = p.y;
        const double zeta = p.z;
        switch (i) {
        case 0:
            return (1. - xi - eta) * 0.5 * (1. - zeta);
        case 1:
            return xi * 0.5 * (1. - zeta);
        case 2:
            return eta * 0.5 * (1. - zeta);
        case 3:
            return (1. - xi - eta) * 0.5 * (1. + zeta);
        case 4:
            return xi * 0.5 * (1. + zeta);
        case 5:
            return eta * 0.5 * (1. + zeta);
        default:
            return 0.;
        }
    }

    static Vector
    shape_der(u8 i, const Point & p)
    {
        const double xi = p.x;
        const double eta = p.y;
        const double zeta = p.z;
        switch (i) {
        case 0:
            return Vector(-0.5 * (1. - zeta), -0.5 * (1. - zeta), -0.5 * (1. - xi - eta));
        case 1:
            return Vector(0.5 * (1. - zeta), 0.0, -0.5 * xi);
        case 2:
            return Vector(0.0, 0.5 * (1. - zeta), -0.5 * eta);
        case 3:
            return Vector(-0.5 * (1. + zeta), -0.5 * (1. + zeta), 0.5 * (1. - xi - eta));
        case 4:
            return Vector(0.5 * (1. + zeta), 0.0, 0.5 * xi);
        case 5:
            return Vector(0.0, 0.5 * (1. + zeta), 0.5 * eta);
        default:
            return Vector(0., 0., 0.);
        }
    }

    static double
    compute_det_J(const Element & elem, const Mesh & mesh, const Point & p)
    {
        Vector dxdxi(0, 0, 0);
        Vector dxdeta(0, 0, 0);
        Vector dxdzeta(0, 0, 0);
        auto idxs = elem.indices();
        for (u8 i = 0; i < Prism6::N_VERTICES; ++i) {
            const auto der = shape_der(i, p);
            const Vector pt(mesh.point(idxs[i]));
            dxdxi += der.x * pt;
            dxdeta += der.y * pt;
            dxdzeta += der.z * pt;
        }
        return std::abs(dot_product(dxdxi, cross_product(dxdeta, dxdzeta)));
    }
};

// PYRAMID5
template <>
struct FEValues<ElementType::PYRAMID5> {
    static double
    shape_val(u8 i, const Point & p)
    {
        const double xi = p.x;
        const double eta = p.y;
        const double zeta = p.z;
        switch (i) {
        case 0:
            return 0.25 * (1. - xi) * (1. - eta) * (1. - zeta);
        case 1:
            return 0.25 * (1. + xi) * (1. - eta) * (1. - zeta);
        case 2:
            return 0.25 * (1. + xi) * (1. + eta) * (1. - zeta);
        case 3:
            return 0.25 * (1. - xi) * (1. + eta) * (1. - zeta);
        case 4:
            return zeta;
        default:
            return 0.;
        }
    }

    static Vector
    shape_der(u8 i, const Point & p)
    {
        const double xi = p.x;
        const double eta = p.y;
        const double zeta = p.z;
        switch (i) {
        case 0:
            return Vector(-0.25 * (1. - eta) * (1. - zeta),
                          -0.25 * (1. - xi) * (1. - zeta),
                          -0.25 * (1. - xi) * (1. - eta));
        case 1:
            return Vector(0.25 * (1. - eta) * (1. - zeta),
                          -0.25 * (1. + xi) * (1. - zeta),
                          -0.25 * (1. + xi) * (1. - eta));
        case 2:
            return Vector(0.25 * (1. + eta) * (1. - zeta),
                          0.25 * (1. + xi) * (1. - zeta),
                          -0.25 * (1. + xi) * (1. + eta));
        case 3:
            return Vector(-0.25 * (1. + eta) * (1. - zeta),
                          0.25 * (1. - xi) * (1. - zeta),
                          -0.25 * (1. - xi) * (1. + eta));
        case 4:
            return Vector(0, 0, 1);
        default:
            return Vector(0, 0, 0);
        }
    }

    static double
    compute_det_J(const Element & elem, const Mesh & mesh, const Point & p)
    {
        Vector dxdxi(0, 0, 0);
        Vector dxdeta(0, 0, 0);
        Vector dxdzeta(0, 0, 0);
        auto idxs = elem.indices();
        for (u8 i = 0; i < Pyramid5::N_VERTICES; ++i) {
            const auto der = shape_der(i, p);
            const Vector pt(mesh.point(idxs[i]));
            dxdxi += der.x * pt;
            dxdeta += der.y * pt;
            dxdzeta += der.z * pt;
        }
        return std::abs(dot_product(dxdxi, cross_product(dxdeta, dxdzeta)));
    }
};

/// Integrate volume of an element
///
/// @tparam ET Element type
/// @param elem Element
/// @param mesh Mesh
/// @return Volume
template <ElementType ET>
double
integrate_volume(const Element & elem, const Mesh & mesh)
{
    auto qpts = Quadrature::get(ET);
    double volume = 0;
    for (auto & qp : qpts) {
        volume += qp.weight * FEValues<ET>::compute_det_J(elem, mesh, qp.point);
    }
    return volume;
}

} // namespace krado
