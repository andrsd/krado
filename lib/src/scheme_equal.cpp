#include "krado/scheme_equal.h"
#include "krado/mesh.h"
#include "krado/exception.h"
#include "krado/mesh_curve_vertex.h"

namespace krado {

SchemeEqual::SchemeEqual(Mesh & mesh, const Parameters & params) :
    Scheme(mesh, params),
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

    // TODO: create a better way how to construct curve mesh

    auto & bnd_verts = mcurve.bounding_vertices();
    mcurve.add_vertex(bnd_verts[0]);
    for (int i = 1; i < this->n_intervals; i++) {
        auto curve_vert = new MeshCurveVertex(gcurve, u(i));
        mcurve.add_curve_vertex(curve_vert);
    }
    mcurve.add_vertex(bnd_verts[1]);

    for (int i = 0; i < this->n_intervals; i++)
        mcurve.add_curve_segment(i, i + 1);
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
