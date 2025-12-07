// SPDX-FileCopyrightText: 2025 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "krado/classifier.h"
#include "krado/occ.h"
#include "gp_Pnt.hxx"
#include <TopAbs_State.hxx>

namespace krado {

SolidClassifier::SolidClassifier(const GeomShape & shape) : classifier_(shape) {}

bool
SolidClassifier::inside(const Point & pt)
{
    this->classifier_.Perform(occ::to_pnt(pt), Precision::Confusion());
    auto state = this->classifier_.State();
    return (state == TopAbs_IN || state == TopAbs_ON);
}

bool
SolidClassifier::outside(const Point & pt)
{
    this->classifier_.Perform(occ::to_pnt(pt), Precision::Confusion());
    auto state = this->classifier_.State();
    return (state == TopAbs_OUT);
}

} // namespace krado
