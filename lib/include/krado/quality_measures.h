// SPDX-FileCopyrightText: 2026 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include "krado/element.h"
#include "krado/mesh.h"
#include <concepts>

namespace krado {

namespace qm {

template <typename T>
concept IsSimplex = (T::TYPE == ElementType::TRI3) || (T::TYPE == ElementType::TETRA4);

template <typename T>
concept IsNotSimplex = (T::TYPE == ElementType::QUAD4) || (T::TYPE == ElementType::HEX8) ||
                       (T::TYPE == ElementType::PRISM6) || (T::TYPE == ElementType::PYRAMID5);

/// Compute aspect ratio of an element
///
/// @tparam ET Element type
/// @param elem Element
/// @param mesh Mesh
/// @return Aspect ratio
template <ElementType ET>
double aspect_ratio(const Element & elem, const Mesh & mesh);

/// Compute minimum angle of an element
///
/// @tparam ET Element type
/// @param elem Element
/// @param mesh Mesh
/// @return Minimum angle [deg]
template <ElementType ET>
double min_angle(const Element & elem, const Mesh & mesh);

/// Compute maximum angle of an element
///
/// @tparam ET Element type
/// @param elem Element
/// @param mesh Mesh
/// @return Maximum angle [deg]
template <ElementType ET>
double max_angle(const Element & elem, const Mesh & mesh);

/// Compute scaled Jacobian of an element
///
/// @tparam ET Element type
/// @param elem Element
/// @param mesh Mesh
/// @return Scaled Jacobian [-1..1]
template <ElementType ET>
double scaled_jacobian(const Element & elem, const Mesh & mesh);

/// Compute equiangular skewness (eta) of an element
///
/// @tparam ET Element type
/// @param elem Element
/// @param mesh Mesh
/// @return Equiangular skewness [0..1]
template <ElementType ET>
double eta(const Element & elem, const Mesh & mesh);

/// Compute radius ratio (gamma) of a simplex element
///
/// @tparam ET Element type
/// @param elem Element
/// @param mesh Mesh
/// @return Radius ratio
template <ElementType ET>
    requires IsSimplex<ElementSelector<ET>>
double gamma(const Element & elem, const Mesh & mesh);

/// Compute skewness of a non-simplex element
///
/// @tparam ET Element type
/// @param elem Element
/// @param mesh Mesh
/// @return Skewness
template <ElementType ET>
    requires IsNotSimplex<ElementSelector<ET>>
double skewness(const Element & elem, const Mesh & mesh);

enum class Metric {
    ASPECT_RATIO,
    MIN_ANGLE,
    MAX_ANGLE,
    SCALED_JACOBIAN,
    ETA,
    GAMMA,
    SKEWNESS,
};

struct QualityStats {
    Metric metric;
    /// Minimum quality
    double min;
    /// Maximum quality
    double max;
    /// Histogram
    std::vector<std::size_t> histogram;
};

} // namespace qm

/// Compute quality statistics for a mesh
///
/// @param mesh Mesh
/// @param metric Metric to compute statistics for
/// @param n_bins Number of bins for the histogram
/// @return Quality statistics
qm::QualityStats compute_quality(const Mesh & mesh, qm::Metric metric, int n_bins = 10);

/// Print quality statistics for a mesh
///
/// @param stats Quality statistics
/// @param metric Metric to compute statistics for
void print_quality(const qm::QualityStats & stats);

} // namespace krado
