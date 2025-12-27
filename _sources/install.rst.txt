Installation
============

Prerequisites:

- A C++17-compatible compiler (e.g., GCC >= 11, Clang >= 14)
- `CMake <https://cmake.org/>`_ >= 3.15
- `fmt <https://github.com/fmtlib/fmt>`_ = 11.x
- `spdlog <https://github.com/gabime/spdlog>`_
- `exodusIIcpp <https://github.com/andrsd/exodusIIcpp>`_ = 3.x
- `OpenCASCADE <https://www.opencascade.com/>`_ >= 7.6.0
- `Eigen3 <https://eigen.tuxfamily.org/>`_ >= 3.4
- `Boost <https://www.boost.org/>`_ >= 1.70
- Python >= 3.10
- `pybind11 <https://github.com/pybind/pybind11>`_ >= 2.11
- `Doxygen <https://www.doxygen.nl/>`_ (optional, for documentation)


.. tab-set::

   .. tab-item:: conda

      1. Add the following channels:

         .. code:: shell

            conda config --add channels andrsd

      2. Install

         .. code:: shell

            conda install -c andrsd krado

   .. tab-item:: from sources

      1. Clone the repo:

         .. code:: shell

            git clone https://github.com/andrsd/krado.git

      2. Build the code

         .. code:: shell

            cd krado
            mkdir build
            cmake -S . -B build
            cmake --build build
