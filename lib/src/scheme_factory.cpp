// SPDX-FileCopyrightText: 2024 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "krado/scheme_factory.h"
#include "krado/scheme_auto.h"
#include "krado/scheme_equal.h"
#include "krado/scheme_transfinite.h"
#include "krado/scheme_bamg.h"
#include "krado/scheme_triangle.h"

namespace krado {

SchemeFactory &
SchemeFactory::instance()
{
    static SchemeFactory factory;
    return factory;
}

SchemeFactory::SchemeFactory()
{
    add<SchemeAuto>("auto");
    add<SchemeEqual>("equal");
    add<SchemeTransfinite>("transfinite");
    add<SchemeBAMG>("bamg");
    add<SchemeTriangle>("triangle");
}

} // namespace krado
