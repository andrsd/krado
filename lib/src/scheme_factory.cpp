// SPDX-FileCopyrightText: 2024 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "krado/scheme_factory.h"
#include "krado/scheme/auto.h"
#include "krado/scheme/equal.h"
#include "krado/scheme/tricircle.h"
#include "krado/scheme/bamg.h"
#include "krado/scheme/triangle.h"
#include "krado/scheme/trisurf.h"

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
    add<SchemeBAMG>("bamg");
    add<SchemeEqual>("equal");
    add<SchemeTriangle>("triangle");
    add<SchemeTriSurf>("trisurf");
    add<SchemeTriCircle>("tricircle");
}

} // namespace krado
