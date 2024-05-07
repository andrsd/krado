// SPDX-FileCopyrightText: 2024 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "krado/scheme_factory.h"
#include "krado/scheme/auto.h"
#include "krado/scheme/equal.h"
#include "krado/scheme/bias.h"
#include "krado/scheme/bump.h"
#include "krado/scheme/betalaw.h"
#include "krado/scheme/sizemap.h"
#include "krado/scheme/bamg.h"
#include "krado/scheme/triangle.h"
#include "krado/scheme/trisurf.h"
#include "krado/scheme/frontal_delaunay.h"

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
    add<SchemeBias>("bias");
    add<SchemeBump>("bump");
    add<SchemeBetaLaw>("beta-law");
    add<SchemeSizeMap>("sizemap");
    add<SchemeBAMG>("bamg");
    add<SchemeTriangle>("triangle");
    add<SchemeTriSurf>("trisurf");
    add<SchemeFrontalDelaunay>("frontal-delaunay");
}

} // namespace krado
