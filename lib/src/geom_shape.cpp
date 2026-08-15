// SPDX-FileCopyrightText: 2024 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "krado/geom_shape.h"
#include "krado/color_map.h"
#include "BRepLib.hxx"
#include "BRep_Tool.hxx"
#include "BRepTools.hxx"
#include "BRepBuilderAPI_MakeSolid.hxx"
#include "BRepBuilderAPI_Transform.hxx"
#include "TopoDS.hxx"
#include "ShapeBuild_ReShape.hxx"
#include "GProp_GProps.hxx"
#include "BRepGProp.hxx"

namespace krado {

namespace {

const std::vector<Color> shape_color = { ColorMap::medium_blue, ColorMap::medium_grey,
                                         ColorMap::dark_blue,   ColorMap::light_grey,
                                         ColorMap::light_blue,  ColorMap::orange,
                                         ColorMap::dark_grey,   ColorMap::yellow };

Color
get_next_color()
{
    static std::size_t color_index = 0;
    Color clr = shape_color[color_index];
    color_index = (color_index + 1) % shape_color.size();
    return clr;
}

} // namespace

GeomShape::GeomShape(const TopoDS_Shape & shape) : shape_(shape) {}

int
GeomShape::dim() const
{
    return -1;
}

int
GeomShape::id() const
{
    return this->id_;
}

std::string
GeomShape::name() const
{
    return this->name_;
}

void
GeomShape::set_name(const std::string & name)
{
    this->name_ = name;
}

bool
GeomShape::has_material() const
{
    return !this->material_name_.empty();
}

const std::string &
GeomShape::material_description() const
{
    return this->material_description_;
}

void
GeomShape::set_material(const std::string & name, const std::string & description, double density)
{
    this->material_name_ = name;
    this->material_description_ = description;
    this->density_ = density;
}

const std::string &
GeomShape::material() const
{
    return this->material_name_;
}

Color
GeomShape::color() const
{
    return this->clr_;
}

void
GeomShape::set_color(const Color & color)
{
    this->clr_ = color;
}

double
GeomShape::length() const
{
    GProp_GProps props;
    BRepGProp::LinearProperties(this->shape_, props);
    return props.Mass();
}

double
GeomShape::area() const
{
    GProp_GProps props;
    BRepGProp::SurfaceProperties(this->shape_, props);
    return props.Mass();
}

double
GeomShape::volume() const
{
    GProp_GProps props;
    BRepGProp::VolumeProperties(this->shape_, props);
    return props.Mass();
}

double
GeomShape::density() const
{
    return this->density_;
}

void
GeomShape::scale(double scale_factor)
{
    gp_Trsf t;
    t.SetScaleFactor(scale_factor);
    BRepBuilderAPI_Transform trsf(this->shape_, t);
    this->shape_ = trsf.Shape();
}

void
GeomShape::clean()
{
    BRepTools::Clean(this->shape_);
}

GeomShape::operator const TopoDS_Shape &() const
{
    return this->shape_;
}

void
GeomShape::assign_color()
{
    this->clr_ = get_next_color();
}

} // namespace krado
