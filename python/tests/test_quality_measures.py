import math

import krado
import pytest


def test_compute_quality_tri3():
    pts = [
        krado.Point(0, 0, 0),
        krado.Point(1, 0, 0),
        krado.Point(0.5, math.sqrt(3) / 2, 0),
    ]
    elems = [krado.Element(krado.ElementType.TRI3, [0, 1, 2])]
    mesh = krado.Mesh(pts, elems)

    stats = krado.compute_quality(mesh, krado.qm.Metric.ASPECT_RATIO)
    assert math.isclose(stats.min, 1.0, rel_tol=1e-12)
    assert math.isclose(stats.max, 1.0, rel_tol=1e-12)
    assert len(stats.histogram) == 10
    assert stats.histogram[0] == 1


def test_print_quality():
    pts = [
        krado.Point(0, 0, 0),
        krado.Point(1, 0, 0),
        krado.Point(0.5, math.sqrt(3) / 2, 0),
    ]
    elems = [krado.Element(krado.ElementType.TRI3, [0, 1, 2])]
    mesh = krado.Mesh(pts, elems)

    stats = krado.compute_quality(mesh, krado.qm.Metric.ASPECT_RATIO)
    # This should just not crash
    krado.print_quality(stats)


def test_compute_quality_unsupported():
    pts = [
        krado.Point(0, 0, 0),
        krado.Point(1, 0, 0),
        krado.Point(0.5, math.sqrt(3) / 2, 0),
    ]
    elems = [krado.Element(krado.ElementType.TRI3, [0, 1, 2])]
    mesh = krado.Mesh(pts, elems)

    with pytest.raises(RuntimeError):
        krado.compute_quality(mesh, krado.qm.Metric.SKEWNESS)


def test_compute_quality_complex():
    pts = [
        krado.Point(0, 0, 0),
        krado.Point(1, 0, 0),
        krado.Point(0.5, math.sqrt(3) / 2, 0),
        krado.Point(2, 0, 0),
        krado.Point(1.1, 0.1, 0),
    ]
    elems = [
        krado.Element(krado.ElementType.TRI3, [0, 1, 2]),
        krado.Element(krado.ElementType.TRI3, [1, 3, 4]),
    ]
    mesh = krado.Mesh(pts, elems)

    stats = krado.compute_quality(mesh, krado.qm.Metric.ASPECT_RATIO)
    assert math.isclose(stats.min, 1.0, rel_tol=1e-12)
    assert math.isclose(stats.max, 7.071067811865471, rel_tol=1e-12)
    assert stats.histogram[0] == 1
    assert stats.histogram[9] == 1
