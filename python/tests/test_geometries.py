import pytest
import krado
import math

def test_circle():
    origin = krado.Axis2(krado.Point(0, 0, 0), krado.Vector(0, 0, 1))
    radius = 5.0
    circle = krado.Circle(origin, radius)
    assert math.isclose(circle.radius(), radius)
    assert math.isclose(circle.area(), math.pi * radius**2)

def test_arc_of_circle():
    p1 = krado.Point(1, 0, 0)
    p2 = krado.Point(0, 1, 0)
    p3 = krado.Point(-1, 0, 0)
    arc = krado.ArcOfCircle(p1, p2, p3)
    assert math.isclose(arc.length(), math.pi)

def test_spline():
    points = [krado.Point(0, 0, 0), krado.Point(1, 1, 0), krado.Point(2, 0, 0)]
    spline = krado.Spline(points)
    assert spline.length() > 2.0

def test_helix():
    ax2 = krado.Axis2(krado.Point(0, 0, 0), krado.Vector(0, 0, 1))
    helix = krado.Helix(ax2, radius=1.0, height=1.0, turns=1.0)
    assert helix.length() > 1.0

def test_inscribed_polygon():
    ax2 = krado.Axis2(krado.Point(0, 0, 0), krado.Vector(0, 0, 1))
    poly = krado.InscribedPolygon(ax2, radius=1.0, n_sides=4)
    # length of one side of square inscribed in circle of r=1 is sqrt(2)
    # perimeter = 4 * sqrt(2) approx 5.65685
    assert math.isclose(poly.length(), 4 * math.sqrt(2))

def test_circumscribed_polygon():
    ax2 = krado.Axis2(krado.Point(0, 0, 0), krado.Vector(0, 0, 1))
    poly = krado.CircumscribedPolygon(ax2, radius=1.0, n_sides=4)
    # side of square circumscribed around circle of r=1 is 2
    # perimeter = 8
    assert math.isclose(poly.length(), 8.0)

def test_sphere():
    center = krado.Point(0, 0, 0)
    radius = 1.0
    sphere = krado.Sphere(center, radius)
    assert math.isclose(sphere.volume(), 4/3 * math.pi * radius**3)

def test_cylinder():
    ax2 = krado.Axis2(krado.Point(0, 0, 0), krado.Vector(0, 0, 1))
    cylinder = krado.Cylinder(ax2, radius=1.0, height=2.0)
    assert math.isclose(cylinder.volume(), math.pi * 1.0**2 * 2.0)

def test_cone():
    ax2 = krado.Axis2(krado.Point(0, 0, 0), krado.Vector(0, 0, 1))
    cone = krado.Cone(ax2, radius1=1.0, radius2=0.0, height=3.0)
    assert math.isclose(cone.volume(), 1/3 * math.pi * 1.0**2 * 3.0)
