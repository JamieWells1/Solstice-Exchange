#include <config.h>
#include <dispatcher.h>
#include <py_interface.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <strategy.h>

#include <string>

namespace py = pybind11;

std::expected<PyInterface, std::string> PyInterface::establish()
{
    auto config = solstice::Config::init();
    if (!config)
    {
        return std::unexpected(config.error());
    }

    auto pyInterface = PyInterface();

    return pyInterface;
}

template <typename T>
std::expected<solstice::strategy::Report, std::string> PyInterface::orchestrate(RawMarketData& rawData)
{
    using namespace solstice::strategy;

    MarketData marketData = MarketData::mapRawInput(rawData);
    const Strategy strategy = solstice::Config::strategy;

    std::unique_ptr<T> logic = Dispatcher::constructStrategy<T>(strategy, marketData);
    Report result = logic.execute();

    return result;  // Return empty expected<void> on success
}

PYBIND11_MODULE(py_interface, m)
{
    m.doc() = "Solstice LOB Python bindings";

    py::class_<PyInterface>(m, "PyInterface")
        .def(py::init<>())
        .def_static("establish",
                    []()
                    {
                        auto result = PyInterface::establish();
                        if (!result)
                        {
                            throw std::runtime_error(result.error());
                        }

                        return result.value();
                    })

        .def("strategy", &PyInterface::strategy);
}
