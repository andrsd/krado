// SPDX-FileCopyrightText: 2025 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#ifdef _MSC_VER
#ifndef __restrict__
#define __restrict__ __restrict
#endif
#endif

namespace krado {
// namespace necessary to avoid conflicts with predicates used by Tetgen
namespace robust_predicates {

void exactinit(double maxx, double maxy, double maxz);

double insphere(const double * const __restrict__ pa,
                const double * const __restrict__ pb,
                const double * const __restrict__ pc,
                const double * const __restrict__ pd,
                const double * const __restrict__ pe);

double orient3d(const double * const __restrict__ pa,
                const double * const __restrict__ pb,
                const double * const __restrict__ pc,
                const double * const __restrict__ pd);

double incircle(const double * const __restrict__ pa,
                const double * const __restrict__ pb,
                const double * const __restrict__ pc,
                const double * const __restrict__ pd);

double orient2d(const double * const __restrict__ pa,
                const double * const __restrict__ pb,
                const double * const __restrict__ pc);

} // namespace robust_predicates
} // namespace krado
