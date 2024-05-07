#include "krado/intersect_curve_surface.h"
#include "krado/numerics.h"
#include <Eigen/Dense>

namespace krado {

static bool kaboom(Eigen::VectorXd & uvt, Eigen::VectorXd & res, void * data);

struct IntersectCurveSurfaceData {
    const CurveFunctor & c;
    const SurfaceFunctor & s;
    const double epsilon;
    IntersectCurveSurfaceData(const CurveFunctor & c,
                              const SurfaceFunctor & s,
                              const double & eps) :
        c(c),
        s(s),
        epsilon(eps)
    {
    }

    bool
    apply(double newPoint[3])
    {
        try {
            Eigen::VectorXd uvt(3);
            uvt(0) = newPoint[0];
            uvt(1) = newPoint[1];
            uvt(2) = newPoint[2];
            Eigen::VectorXd res(3);
            kaboom(uvt, res, this);
            if (res.norm() < epsilon)
                return true;
            if (newton_fd(kaboom, uvt, this)) {
                newPoint[0] = uvt(0);
                newPoint[1] = uvt(1);
                newPoint[2] = uvt(2);
                return true;
            }
        }
        catch (...) {
            // printf("intersect curve surface failed !\n");
        }
        // printf("newton failed\n");
        return false;
    }
};

static bool
kaboom(Eigen::VectorXd & uvt, Eigen::VectorXd & res, void * ctx)
{
    auto * data = (IntersectCurveSurfaceData *) ctx;
    auto s = data->s({ uvt(0), uvt(1) });
    auto c = data->c(uvt(2));
    res(0) = s.x - c.x;
    res(1) = s.y - c.y;
    res(2) = s.z - c.z;
    return true;
}

int
intersect_curve_surface(CurveFunctor & c, SurfaceFunctor & s, double uvt[3], double epsilon)
{
    IntersectCurveSurfaceData data(c, s, epsilon);
    return data.apply(uvt);
}

} // namespace krado
