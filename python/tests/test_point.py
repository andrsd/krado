import krado
import pytest
import math


def test_ctor():
    pt = krado.Point()
    assert pt.x == 0.0
    assert pt.y == 0.0
    assert pt.z == 0.0

    pt2 = krado.Point(1.0, 2.0, 3.0)
    assert pt2.x == 1.0
    assert pt2.y == 2.0
    assert pt2.z == 3.0


def test_add_point():
    a = krado.Point(1.0, 2.0, 3.0)
    b = krado.Point(2.0, -1.0, 4.0)
    c = a + b
    assert isinstance(c, krado.Point)
    assert c.x == 3.0
    assert c.y == 1.0
    assert c.z == 7.0


def test_sub_point():
    a = krado.Point(1.0, 2.0, 3.0)
    b = krado.Point(2.0, -1.0, 4.0)
    c = a - b
    assert isinstance(c, krado.Vector)
    assert c.x == -1.0
    assert c.y == 3.0
    assert c.z == -1.0


def test_iadd_point():
    a = krado.Point(1.0, 2.0, 3.0)
    b = krado.Point(2.0, -1.0, 4.0)
    a += b
    assert a.x == 3.0
    assert a.y == 1.0
    assert a.z == 7.0


def test_isub_point():
    a = krado.Point(1.0, 2.0, 3.0)
    b = krado.Point(2.0, -1.0, 4.0)
    a -= b
    assert a.x == -1.0
    assert a.y == 3.0
    assert a.z == -1.0


def test_mult_scalar():
    a = krado.Point(1.0, 2.0, 3.0)
    # *=
    a *= 3.0
    assert a.x == 3.0
    assert a.y == 6.0
    assert a.z == 9.0

    # * and rmul
    b = krado.Point(1.0, 2.0, 3.0)
    c = b * 2.0
    assert isinstance(c, krado.Point)
    assert c.x == 2.0
    assert c.y == 4.0
    assert c.z == 6.0

    d = 2.0 * b
    assert isinstance(d, krado.Point)
    assert d.x == 2.0
    assert d.y == 4.0
    assert d.z == 6.0


def test_div_scalar():
    a = krado.Point(1.0, 2.0, 3.0)
    a /= 2.0
    assert a.x == 0.5
    assert a.y == 1.0
    assert a.z == 1.5


def test_distance():
    a = krado.Point(1.0, -2.0, 3.0)
    b = krado.Point(2.0, -1.0, 6.0)
    d = a.distance(b)
    assert math.isclose(d, math.sqrt(11))


def test_add_sub_vector():
    a = krado.Point(1.0, 2.0, 3.0)
    b = krado.Vector(2.0, -1.0, 4.0)
    
    # Point + Vector
    c = a + b
    assert isinstance(c, krado.Point)
    assert c.x == 3.0
    assert c.y == 1.0
    assert c.z == 7.0

    # Point - Vector
    d = a - b
    assert isinstance(d, krado.Point)
    assert d.x == -1.0
    assert d.y == 3.0
    assert d.z == -1.0

    # iadd
    a2 = krado.Point(1.0, 2.0, 3.0)
    a2 += b
    assert a2.x == 3.0
    assert a2.y == 1.0
    assert a2.z == 7.0

    # isub
    a3 = krado.Point(1.0, 2.0, 3.0)
    a3 -= b
    assert a3.x == -1.0
    assert a3.y == 3.0
    assert a3.z == -1.0


def test_comparison():
    a = krado.Point(1.0, 2.0, 3.0)
    b = krado.Point(1.0, 2.0, 3.0)
    c = krado.Point(1.0, 2.0, 3.0000001)

    assert a == b
    assert a != c

    # lexicographical comparison
    p1 = krado.Point(1, 2, 3)
    p2 = krado.Point(2, 2, 3)
    p3 = krado.Point(1, 3, 3)
    p4 = krado.Point(1, 2, 4)

    assert p1 < p2
    assert p1 < p3
    assert p1 < p4
