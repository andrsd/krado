#include "krado/numerics.h"
#include "krado/exception.h"
#include "krado/vector.h"

namespace krado {

UVParam
circum_center_xy(const UVParam & p1, const UVParam & p2, const UVParam & p3)
{
    const double x1 = p1.u;
    const double x2 = p2.u;
    const double x3 = p3.u;
    const double y1 = p1.v;
    const double y2 = p2.v;
    const double y3 = p3.v;

    auto d = 2. * (y1 * (x2 - x3) + y2 * (x3 - x1) + y3 * (x1 - x2));
    if (d == 0.0)
        throw Exception("Colinear points in circum circle computation");

    double a1 = x1 * x1 + y1 * y1;
    double a2 = x2 * x2 + y2 * y2;
    double a3 = x3 * x3 + y3 * y3;
    return { ((a1 * (y3 - y2) + a2 * (y1 - y3) + a3 * (y2 - y1)) / d),
             ((a1 * (x2 - x3) + a2 * (x3 - x1) + a3 * (x1 - x2)) / d) };
}

Point
circum_center_xyz(Point p1, Point p2, Point p3, std::tuple<double, double> * uv)
{
    Vector v1 = p2 - p1;
    Vector v2 = p3 - p1;
    Vector vx = p3 - p1;
    Vector vy = p3 - p1;
    auto vz = cross_product(vx, vy);
    vy = cross_product(vz, vx);
    vx.normalize();
    vy.normalize();
    vz.normalize();

    UVParam p1P(0.0, 0.0);
    UVParam p2P(dot_product(v1, vx), dot_product(v1, vy));
    UVParam p3P(dot_product(v2, vx), dot_product(v2, vy));

    UVParam resP;
    try {
        resP = circum_center_xy(p1P, p2P, p3P);
    }
    catch (Exception & e) {
        resP = { -99999., -99999. };
    }

    if (uv) {
#if 0
        Eigen::Matrix2d mat;
        mat(0, 0) = p2P.u - p1P.u;
        mat(0, 1) = p3P.u - p1P.u;
        mat(1, 0) = p2P.v - p1P.v;
        mat(1, 1) = p3P.v - p1P.v;
        Eigen::Vector2d rhs;
        rhs(0) = resP.u - p1P.u;
        rhs(1) = resP.v - p1P.v;
        auto sln = sys2x2(mat, rhs);
        *uv = { sln[0], sln[1] };
#endif
        *uv = { 0., 0. };
    }

    return p1 + resP.u * vx + resP.v * vy;
}

double
triangle_area(const Point & p0, const Point & p1, const Point & p2)
{
    Vector a = p2 - p1;
    Vector b = p0 - p1;
    auto c = cross_product(a, b);
    return 0.5 * c.magnitude();
}

Vector
normal3points(const Point & a, const Point & b, const Point & c)
{
    Vector t1 = b - a;
    Vector t2 = c - a;
    auto n = cross_product(t1, t2);
    n.normalize();
    return n;
}

#if 0
Eigen::Vector2d
sys2x2(const Eigen::Matrix2d & mat, const Eigen::Vector2d & rhs)
{
    auto qr = mat.colPivHouseholderQr();
    if (qr.info() == Eigen::Success)
        return qr.solve(rhs);
    else
        throw Exception("Failed to solve 2x2 system");
}

double
det2x2(const Eigen::Matrix2d & mat)
{
    return mat(0, 0) * mat(1, 1) - mat(1, 0) * mat(0, 1);
}
#endif

int
intersection_segments(const UVParam & p1,
                      const UVParam & p2,
                      const UVParam & q1,
                      const UVParam & q2,
                      UVParam & x)
{
    double xp_max = std::max(p1.u, p2.u);
    double yp_max = std::max(p1.v, p2.v);
    double xq_max = std::max(q1.u, q2.u);
    double yq_max = std::max(q1.v, q2.v);

    double xp_min = std::min(p1.u, p2.u);
    double yp_min = std::min(p1.v, p2.v);
    double xq_min = std::min(q1.u, q2.u);
    double yq_min = std::min(q1.v, q2.v);
    if (yq_min > yp_max || xq_min > xp_max || yq_max < yp_min || xq_max < xp_min) {
        return 0;
    }
    else {
#if 0
        Eigen::Matrix2d A;
        A(0, 0) = p2.u - p1.u;
        A(0, 1) = q1.u - q2.u;
        A(1, 0) = p2.v - p1.v;
        A(1, 1) = q1.v - q2.v;
        Eigen::Vector2d b;
        b[0] = q1.u - p1.u;
        b[1] = q1.v - p1.v;
        auto x = sys2x2(A, b);
        return (x[0] >= 0.0 && x[0] <= 1. && x[1] >= 0.0 && x[1] <= 1.);
#endif
        return 0;
    }
}

int
intersection_segments(const Point & p1,
                      const Point & p2,
                      const Point & q1,
                      const Point & q2,
                      UVParam & x)
{
#if 0
    auto v1 = p1 - p2;
    auto v2 = q1 - q2;
    auto n1 = v1.magnitude();
    auto n2 = v2.magnitude();
    auto EPS = 1.e-10 * std::max(n1, n2);
    Eigen::Matrix2d A;
    A(0, 0) = p2.x - p1.x;
    A(0, 1) = q1.x - q2.x;
    A(1, 0) = p2.y - p1.y;
    A(1, 1) = q1.y - q2.y;
    Eigen::Vector2d a;
    a[0] = q1.x - p1.x;
    a[1] = q1.y - p1.y;
    Eigen::Matrix2d B;
    B(0, 0) = p2.z - p1.z;
    B(0, 1) = q1.z - q2.z;
    B(1, 0) = p2.y - p1.y;
    B(1, 1) = q1.y - q2.y;
    Eigen::Vector2d b;
    b[0] = q1.z - p1.z;
    b[1] = q1.y - p1.y;
    Eigen::Matrix2d C;
    C(0, 0) = p2.z - p1.z;
    C(0, 1) = q1.z - q2.z;
    C(1, 0) = p2.x - p1.x;
    C(1, 1) = q1.x - q2.x;
    Eigen::Vector2d c;
    c[0] = q1.z - p1.z;
    c[1] = q1.x - p1.x;
    double detA = fabs(det2x2(A));
    double detB = fabs(det2x2(B));
    double detC = fabs(det2x2(C));
    //  printf("%12.5E %12.5E %12.5E\n",detA,detB,detC);
    Eigen::Vector2d sln;
    if (detA > detB && detA > detC)
        sln = sys2x2(A, a);
    else if (detB > detA && detB > detC)
        sln = sys2x2(B, b);
    else
        sln = sys2x2(C, c);
    x.u = sln[0];
    x.v = sln[1];
    if (x.u >= 0.0 && x.u <= 1. && x.v >= 0.0 && x.v <= 1.) {
        Point x1(p1.x * (1. - x.u) + p2.x * x.u,
                 p1.y * (1. - x.u) + p2.y * x.u,
                 p1.z * (1. - x.u) + p2.z * x.u);
        Point x2(q1.x * (1. - x.v) + q2.x * x.v,
                 q1.y * (1. - x.v) + q2.y * x.v,
                 q1.z * (1. - x.v) + q2.z * x.v);

        auto d = x1 - x2;
        auto nd = d.magnitude();
        if (nd > EPS) {
            x.u = x.v = 1.e22;
            return false;
        }
        return true;
    }
#endif
    return false;
}

#if 0
bool
newton_fd(bool (*func)(Eigen::VectorXd &, Eigen::VectorXd &, void *),
          Eigen::VectorXd & x,
          void * data,
          double relax,
          double tolx)
{
    const int MAX_ITERS = 100;
    const double EPS = 1.e-4;
    const int N = x.size();

    Eigen::MatrixXd J(N, N);
    Eigen::VectorXd f(N), feps(N), dx(N);

    for (int iter = 0; iter < MAX_ITERS; ++iter) {
        if (x.norm() > 1.e6)
            return false;
        if (!func(x, f, data))
            return false;

        bool is_zero = false;
        for (int k = 0; k < N; ++k) {
            if (f(k) == 0.)
                is_zero = true;
            else
                is_zero = false;
            if (is_zero == false)
                break;
        }
        if (is_zero)
            break;

        for (int j = 0; j < N; ++j) {
            double h = EPS * std::fabs(x(j));
            if (h == 0.)
                h = EPS;
            x(j) += h;
            if (!func(x, feps, data))
                return false;

            for (int i = 0; i < N; ++i)
                J(i, j) = (feps(i) - f(i)) / h;
            x(j) -= h;
        }

        if (N == 1)
            dx(0) = f(0) / J(0, 0);
        else {
            Eigen::PartialPivLU<Eigen::MatrixXd> lu_decomp(J);
            dx = lu_decomp.solve(f);
        }

        for (int i = 0; i < N; ++i)
            x(i) -= relax * dx(i);

        if (dx.norm() < tolx)
            return true;
    }
    return false;
}
#endif

} // namespace krado
