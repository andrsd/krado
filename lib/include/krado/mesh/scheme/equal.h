#pragma once

#include "krado/mesh/scheme/scheme1d.h"
#include "eigen3/Eigen/Sparse"

namespace krado::mesh {

class Equal : public Scheme1D {
public:
    Equal(int n_intervals);

    void mesh_curve(const mesh::Curve & mcurve) override;

private:
    Eigen::SparseMatrix<double> build_matrix();
    Eigen::VectorXd build_rhs(double lo, double hi);
    Eigen::VectorXd solve(const Eigen::SparseMatrix<double> & A, const Eigen::VectorXd & b);

    int n_intervals;
};

} // namespace krado::mesh
