// SPDX-FileCopyrightText: 2024 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include "krado/element.h"

namespace krado {

class MeshElement;
class BDS_Point;

namespace quality {

/// Compute eta quality measure for a given element
///
/// @param e Mesh element
/// @return Eta quality measure
///
/// @note The eta quality measure is defined as the ratio of the inradius (the radius of the largest
///       circle that can fit inside the element) to the circumradius (the radius of the smallest
///       circle that can encompass the element)
double eta(const MeshElement & e);

/// Compute gamma quality measure for a given element
///
/// @param e Mesh element
/// @return Gamma quality measure [0-1], where 1 is the best quality and 0 is the worst
double gamma(const MeshElement & e);

template <ElementType ET>
double gamma(const std::array<BDS_Point *, ElementSelector<ET>::N_VERTICES> & pts);

} // namespace quality
} // namespace krado
