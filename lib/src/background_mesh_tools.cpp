#include "krado/background_mesh_tools.h"
#include "krado/mesh_surface.h"

namespace krado {

bool
extend_1d_mesh_in_2d_surfaces(const MeshSurface & msurface)
{
    auto val = msurface.mesh_size_from_boundary();
    return (val > 0 || val == -2);
}

} // namespace krado
