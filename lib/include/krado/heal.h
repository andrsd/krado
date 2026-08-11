// SPDX-FileCopyrightText: 2026 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include "krado/types.h"
#include "krado/flags.h"

namespace krado {

class GeomShape;

enum HealFlag : u8 {
    FIX_DEGENERATED,
    FIX_SMALL_EDGES,
    FIX_SMALL_FACES,
    SEW_FACES,
    MAKE_SOLIDS,
};

/// Heal the shape
///
/// @param shape Shape to heal
/// @param tolerance Tolerance used for healing
/// @param flags What should be "healed"
/// @return Healed shape
GeomShape heal(const GeomShape & shape,
               double tolerance,
               Flags<HealFlag> flags = FIX_DEGENERATED | FIX_SMALL_EDGES | FIX_SMALL_FACES |
                                       SEW_FACES | MAKE_SOLIDS);

} // namespace krado
