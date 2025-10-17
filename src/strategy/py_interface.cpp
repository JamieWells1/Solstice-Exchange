#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <strategy.h>

namespace py = pybind11;

PYBIND11_MODULE(py_interface, m)
{
    m.doc() = "Solstice LOB Python bindings";

    py::class_<solstice::strategy::Strategy>(m, "Strategy").def(py::init<>());
}
