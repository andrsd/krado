#include "krado/poly_mesh.h"

namespace krado {

PolyMesh::Vertex::Vertex(double x, double y, double z, int d) :
    position(x, y, z),
    he(nullptr),
    data(d)
{
}

//

PolyMesh::HalfEdge::HalfEdge(Vertex * vv) :
    v(vv),
    f(nullptr),
    prev(nullptr),
    next(nullptr),
    opposite(nullptr),
    data(-1)
{
}

Vector
PolyMesh::HalfEdge::d() const
{
    Vector t = this->next->v->position - this->v->position;
    t.normalize();
    return t;
}

//

PolyMesh::Face::Face(HalfEdge * e) : he(e), data(-1) {}

//

PolyMesh::~PolyMesh()
{
    reset();
}

void
PolyMesh::reset()
{
    for (auto it : this->vertices)
        delete it;
    for (auto it : this->hedges)
        delete it;
    for (auto it : this->faces)
        delete it;
}

size_t
PolyMesh::degree(const Vertex * v) const
{
    HalfEdge * he = v->he;
    size_t count = 0;
    do {
        he = he->opposite;
        if (he == nullptr)
            return -1;
        he = he->next;
        count++;
    } while (he != v->he);
    return count;
}

size_t
PolyMesh::num_sides(const HalfEdge * he) const
{
    size_t count = 0;
    const HalfEdge * start = he;
    do {
        count++;
        he = he->next;
    } while (he != start);
    return count;
}

Vector
PolyMesh::normal(const Vertex * v) const
{
    Vector n(0, 0, 0);
    HalfEdge * he = v->he;
    do {
        Vector n1 = he->d();
        he = he->opposite;
        if (he == nullptr)
            return Vector(-1, -1, -1);
        he = he->next;
        n += cross_product(n1, he->d());
    } while (he != v->he);
    n.normalize();
    return n;
}

PolyMesh::HalfEdge *
PolyMesh::get_edge(Vertex * v0, Vertex * v1)
{
    HalfEdge * he = v0->he;
    do {
        if (he->next->v == v1)
            return he;
        he = he->opposite;
        if (he == nullptr)
            return nullptr;
        he = he->next;
    } while (he != v0->he);
    return nullptr;
}

void
PolyMesh::create_face(Face * f,
                      Vertex * v0,
                      Vertex * v1,
                      Vertex * v2,
                      HalfEdge * he0,
                      HalfEdge * he1,
                      HalfEdge * he2)
{
    he0->v = v0;
    he1->v = v1;
    he2->v = v2;
    v0->he = he0;
    v1->he = he1;
    v2->he = he2;

    he0->next = he1;
    he1->prev = he0;
    he1->next = he2;
    he2->prev = he1;
    he2->next = he0;
    he0->prev = he2;
    he0->f = he1->f = he2->f = f;
    f->he = he0;
}

int
PolyMesh::swap_edge(HalfEdge * he0)
{
    HalfEdge * heo0 = he0->opposite;
    if (heo0 == nullptr)
        return -1;

    HalfEdge * he1 = he0->next;
    HalfEdge * he2 = he1->next;
    HalfEdge * heo1 = heo0->next;
    HalfEdge * heo2 = heo1->next;

    Vertex * v0 = heo1->v;
    Vertex * v1 = heo2->v;
    Vertex * v2 = heo0->v;
    Vertex * v3 = he2->v;

    create_face(he0->f, v0, v1, v3, heo1, heo0, he2);
    create_face(heo2->f, v1, v2, v3, heo2, he1, he0);
    return 0;
}

int
PolyMesh::merge_faces(HalfEdge * he)
{
    PolyMesh::HalfEdge * heo = he->opposite;

    if (heo == nullptr)
        return -1;

    PolyMesh::Face * to_delete = heo->f;

    do {
        heo->f = he->f;
        heo = heo->next;
    } while (heo != he->opposite);

    he->next->prev = heo->prev;
    heo->prev->next = he->next;
    he->prev->next = heo->next;
    heo->next->prev = he->prev;

    he->f->he = he->next;
    he->v->he = heo->next;
    heo->v->he = he->next;

    // remove afterwards...
    he->v = nullptr;
    heo->v = nullptr;
    to_delete->he = nullptr;
    return 0;
}

void
PolyMesh::cleanv()
{
    std::vector<Vertex *> uv;
    for (auto v : this->vertices) {
        if (v->he)
            uv.push_back(v);
        else
            delete v;
    }
    this->vertices = uv;
}

void
PolyMesh::cleanh()
{
    std::vector<HalfEdge *> uh;
    for (auto h : this->hedges) {
        if (h->f)
            uh.push_back(h);
        else
            delete h;
    }
    this->hedges = uh;
}

void
PolyMesh::cleanf()
{
    std::vector<Face *> uf;
    for (auto f : this->faces) {
        if (f->he)
            uf.push_back(f);
        else
            delete f;
    }
    this->faces = uf;
}

void
PolyMesh::clean()
{
    cleanv();
    cleanh();
    cleanf();
}

int
PolyMesh::split_edge(HalfEdge * he0m, const Vector & position, int data)
{
    HalfEdge * he1m = he0m->opposite;
    if (he1m == nullptr)
        return -1;

    auto * mid = new Vertex(position.x, position.y, position.z, data);
    this->vertices.push_back(mid);

    HalfEdge * he12 = he0m->next;
    HalfEdge * he20 = he0m->next->next;
    HalfEdge * he03 = he0m->opposite->next;
    HalfEdge * he31 = he0m->opposite->next->next;

    Vertex * v0 = he03->v;
    Vertex * v1 = he12->v;
    Vertex * v2 = he20->v;
    Vertex * v3 = he31->v;

    auto * hem0 = new HalfEdge(mid);
    auto * hem1 = new HalfEdge(mid);
    auto * hem2 = new HalfEdge(mid);
    auto * hem3 = new HalfEdge(mid);

    auto * he2m = new HalfEdge(v2);
    auto * he3m = new HalfEdge(v3);

    he0m->opposite = hem0;
    hem0->opposite = he0m;
    he1m->opposite = hem1;
    hem1->opposite = he1m;
    he2m->opposite = hem2;
    hem2->opposite = he2m;
    he3m->opposite = hem3;
    hem3->opposite = he3m;

    this->hedges.push_back(hem0);
    this->hedges.push_back(hem1);
    this->hedges.push_back(hem2);
    this->hedges.push_back(hem3);
    this->hedges.push_back(he2m);
    this->hedges.push_back(he3m);

    Face * f0m2 = he0m->f;
    Face * f1m3 = he1m->f;
    auto * f2m1 = new Face(he2m);
    auto * f3m0 = new Face(he3m);
    this->faces.push_back(f2m1);
    this->faces.push_back(f3m0);

    create_face(f0m2, v0, mid, v2, he0m, hem2, he20);
    create_face(f1m3, v1, mid, v3, he1m, hem3, he31);
    create_face(f2m1, v2, mid, v1, he2m, hem1, he12);
    create_face(f3m0, v3, mid, v0, he3m, hem0, he03);
    return 0;
}

void
PolyMesh::initialize_rectangle(double xmin, double xmax, double ymin, double ymax)
{
    reset();
    auto * v_mm = new Vertex(xmin, ymin, 0);
    this->vertices.push_back(v_mm);
    auto * v_mM = new Vertex(xmin, ymax, 0);
    this->vertices.push_back(v_mM);
    auto * v_MM = new Vertex(xmax, ymax, 0);
    this->vertices.push_back(v_MM);
    auto * v_Mm = new Vertex(xmax, ymin, 0);
    this->vertices.push_back(v_Mm);
    auto * mm_MM = new HalfEdge(v_mm);
    auto * MM_Mm = new HalfEdge(v_MM);
    auto * Mm_mm = new HalfEdge(v_Mm);
    this->hedges.push_back(mm_MM);
    this->hedges.push_back(MM_Mm);
    this->hedges.push_back(Mm_mm);
    auto * f0 = new Face(mm_MM);
    this->faces.push_back(f0);
    create_face(f0, v_mm, v_MM, v_Mm, mm_MM, MM_Mm, Mm_mm);

    auto * MM_mm = new HalfEdge(v_MM);
    auto * mm_mM = new HalfEdge(v_mm);
    auto * mM_MM = new HalfEdge(v_mM);
    this->hedges.push_back(MM_mm);
    this->hedges.push_back(mm_mM);
    this->hedges.push_back(mM_MM);
    auto * f1 = new Face(MM_mm);
    this->faces.push_back(f1);
    create_face(f1, v_MM, v_mm, v_mM, MM_mm, mm_mM, mM_MM);

    MM_mm->opposite = mm_MM;
    mm_MM->opposite = MM_mm;
}

int
PolyMesh::split_triangle(int index,
                         double x,
                         double y,
                         double z,
                         Face * f,
                         int (*do_swap)(PolyMesh::HalfEdge *, void *),
                         void * data,
                         std::vector<HalfEdge *> * t)
{
    // one more vertex
    auto * v = new Vertex(x, y, z);
    v->data = -1;

    this->vertices.push_back(v);

    HalfEdge * he0 = f->he;
    HalfEdge * he1 = he0->next;
    HalfEdge * he2 = he1->next;

    Vertex * v0 = he0->v;
    Vertex * v1 = he1->v;
    Vertex * v2 = he2->v;
    auto * hev0 = new HalfEdge(v);
    auto * hev1 = new HalfEdge(v);
    auto * hev2 = new HalfEdge(v);

    auto * he0v = new HalfEdge(v0);
    auto * he1v = new HalfEdge(v1);
    auto * he2v = new HalfEdge(v2);

    this->hedges.push_back(hev0);
    this->hedges.push_back(hev1);
    this->hedges.push_back(hev2);
    this->hedges.push_back(he0v);
    this->hedges.push_back(he1v);
    this->hedges.push_back(he2v);

    hev0->opposite = he0v;
    he0v->opposite = hev0;
    hev1->opposite = he1v;
    he1v->opposite = hev1;
    hev2->opposite = he2v;
    he2v->opposite = hev2;

    Face * f0 = f;
    f->he = hev0;
    auto * f1 = new Face(hev1);
    auto * f2 = new Face(hev2);
    f1->data = f2->data = f0->data;

    this->faces.push_back(f1);
    this->faces.push_back(f2);

    create_face(f0, v0, v1, v, he0, he1v, hev0);
    create_face(f1, v1, v2, v, he1, he2v, hev1);
    create_face(f2, v2, v0, v, he2, he0v, hev2);

    if (do_swap) {
        std::stack<HalfEdge *> stack;
        stack.push(he0);
        stack.push(he1);
        stack.push(he2);
        std::vector<HalfEdge *> touched;
        while (!stack.empty()) {
            HalfEdge * he = stack.top();
            touched.push_back(he);
            stack.pop();
            if (do_swap(he, data) == 1) {
                swap_edge(he);

                HalfEdge * H[2] = { he, he->opposite };

                for (int k = 0; k < 2; k++) {
                    if (H[k] == nullptr)
                        continue;
                    HalfEdge * heb = H[k]->next;
                    HalfEdge * hebo = heb->opposite;

                    if (std::find(touched.begin(), touched.end(), heb) == touched.end() &&
                        std::find(touched.begin(), touched.end(), hebo) == touched.end()) {
                        stack.push(heb);
                    }

                    HalfEdge * hec = heb->next;
                    HalfEdge * heco = hec->opposite;

                    if (std::find(touched.begin(), touched.end(), hec) == touched.end() &&
                        std::find(touched.begin(), touched.end(), heco) == touched.end()) {
                        stack.push(hec);
                    }
                }
            }
        }
        if (t)
            *t = touched;
    }
    return 0;
}

} // namespace krado
