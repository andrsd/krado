// SPDX-FileCopyrightText: 2024 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include "krado/scheme.h"
#include "krado/scheme_transfinite.h"

namespace krado {

class SchemeEqual : public SchemeTransfinite {
public:
    SchemeEqual();

private:
    [[nodiscard]] Integral1D compute_integral(const MeshCurve & curve) override;
};

} // namespace krado
