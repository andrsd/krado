#include "krado/scheme_equal.h"
#include "krado/exception.h"
#include "krado/mesh_curve_vertex.h"

namespace krado {

SchemeEqual::SchemeEqual(const Parameters & params) :
    Scheme(params),
    n_intervals(params.get<int>("intervals"))
{
}

void
SchemeEqual::mesh_curve(MeshCurve & mcurve)
{
    auto & gcurve = mcurve.geom_curve();
    auto [lo, hi] = gcurve.param_range();
    auto A = build_matrix();
    auto b = build_rhs(lo, hi);
    auto u = solve(A, b);

    for (int i = 1; i < this->n_intervals; i++) {
        MeshCurveVertex curve_vert(gcurve, u(i));
        mcurve.add_curve_vertex(curve_vert);
    }
    if (this->n_intervals > 1) {
        mcurve.add_curve_segment(MeshCurve::FIRST_VERTEX, 0);
        for (int i = 1; i < this->n_intervals - 1; i++)
            mcurve.add_curve_segment(i - 1, i);
        mcurve.add_curve_segment(this->n_intervals - 2, MeshCurve::LAST_VERTEX);
    }
    else {
        mcurve.add_curve_segment(MeshCurve::FIRST_VERTEX, MeshCurve::LAST_VERTEX);
    }
}

Eigen::SparseMatrix<double>
SchemeEqual::build_matrix()
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
SchemeEqual::build_rhs(double lo, double hi)
{
    int n = this->n_intervals + 2;
    Eigen::VectorXd b(n);
    b.setZero();
    b(0) = lo;
    b(n - 1) = hi;
    return b;
}

Eigen::VectorXd
SchemeEqual::solve(const Eigen::SparseMatrix<double> & A, const Eigen::VectorXd & b)
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

} // namespace krado
