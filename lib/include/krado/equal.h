#pragma once

#include "krado/scheme1d.h"
#include "eigen3/Eigen/Sparse"

namespace krado {

class Equal : public Scheme1D {
public:
    Equal(int n_intervals);

    void mesh_curve(MeshCurve & mcurve) override;

private:
    Eigen::SparseMatrix<double> build_matrix();
    Eigen::VectorXd build_rhs(double lo, double hi);
    Eigen::VectorXd solve(const Eigen::SparseMatrix<double> & A, const Eigen::VectorXd & b);

    int n_intervals;
};

} // namespace krado
