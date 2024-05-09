// SPDX-FileCopyrightText: 2024 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include <pybind11/pybind11.h>
#include "krado/config.h"

namespace py = pybind11;

PYBIND11_MODULE(krado, m)
{
    m.doc() = "pybind11 plugin for krado";
    py::setattr(m, "version", py::str(KRADO_VERSION));
}
