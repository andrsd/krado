// SPDX-FileCopyrightText: 2024 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include "krado/scheme.h"
#include "krado/scheme1d.h"
#include "eigen3/Eigen/Sparse"

namespace krado {

class SchemeEqual : public Scheme, public Scheme1D {
public:
    SchemeEqual(Mesh & mesh, const Parameters & params);

    void mesh_curve(MeshCurve & mcurve) override;

private:
    Eigen::SparseMatrix<double> build_matrix();
    Eigen::VectorXd build_rhs(double lo, double hi);
    Eigen::VectorXd solve(const Eigen::SparseMatrix<double> & A, const Eigen::VectorXd & b);

    int n_intervals;
};

} // namespace krado
