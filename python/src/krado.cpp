// SPDX-FileCopyrightText: 2024 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include <pybind11/pybind11.h>
#include "krado/config.h"
#include "krado/step_file.h"

namespace py = pybind11;
using namespace krado;

PYBIND11_MODULE(krado, m)
{
    m.doc() = "pybind11 plugin for krado";
    py::setattr(m, "version", py::str(KRADO_VERSION));

    // clang-format off
    py::class_<STEPFile>(m, "STEPFile")
        .def(py::init<std::string>())
        .def("load", &STEPFile::load);
    // clang-format on
}
