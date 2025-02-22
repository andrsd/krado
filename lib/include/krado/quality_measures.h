// SPDX-FileCopyrightText: 2024 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

namespace krado {

class MeshElement;

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

} // namespace quality
} // namespace krado
