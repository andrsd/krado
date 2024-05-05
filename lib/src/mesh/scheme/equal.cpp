#include "krado/mesh/scheme/equal.h"
#include "krado/exception.h"

namespace krado::mesh {

Equal::Equal(int n_intervals) :
    n_intervals(n_intervals)
{
}

void
Equal::mesh_curve(const mesh::Curve & mcurve)
{
    auto gcurve = mcurve.geom_curve();
    auto [lo, hi] = gcurve.param_range();
    auto A = build_matrix();
    auto b = build_rhs(lo, hi);
    auto u = solve(A, b);
}

Eigen::SparseMatrix<double>
Equal::build_matrix()
{
    int n = this->n_intervals + 2;
    Eigen::SparseMatrix<double> A(n, n);
    // number of non-zeros on a row
    constexpr int NNZ = 3;
    A.reserve(Eigen::VectorXi::Constant(n, NNZ));
    for (int i = 0; i < n - 1; i++)
        A.insert(i, i) = 1.;
    for (int i = 0; i < n - 2; i++)
        A.insert(i + 1, i) = -1.;
    A.insert(n - 1, n - 2) = 1.;
    for (int i = 1; i < n - 1; i++)
        A.insert(i, n - 1) = -1.;
    return A;
}

Eigen::VectorXd
Equal::build_rhs(double lo, double hi)
{
    int n = this->n_intervals + 2;
    Eigen::VectorXd b(n);
    b.setZero();
    b(0) = lo;
    b(n - 1) = hi;
    return b;
}

Eigen::VectorXd
Equal::solve(const Eigen::SparseMatrix<double> & A, const Eigen::VectorXd & b)
{
    Eigen::SparseLU<Eigen::SparseMatrix<double>> lu;
    lu.compute(A);
    if (lu.info() == Eigen::Success) {
        auto x = lu.solve(b);
        if (lu.info() == Eigen::Success)
            return x;
        else
            throw Exception("Solving failed");
    }
    else
        throw Exception("LU decomposition failed");
}

} // namespace krado::mesh
