from setuptools import setup, Extension
from setuptools.command.build_ext import build_ext
import pybind11

ext_modules = [
    Extension(
        "strategy.py_interface",
        [
            "src/strategy/py_interface.cpp",
            "src/strategy/market_data.cpp",
            "src/strategy/dispatcher.cpp",
            "src/strategy/implementations/sharp_movements.cpp",
            "src/config/config.cpp",
            "src/config/logging.cpp"
        ],
        include_dirs=[
            pybind11.get_include(),
            "src",
            "src/strategy",
            "src/strategy/implementations",
            "src/common",
            "src/config",
            "src/utils",
            "src/enums"
        ],
        language="c++",
        extra_compile_args=["-std=c++23", "-mmacosx-version-min=13.3"],
    ),
]

setup(
    name="solstice",
    packages=["strategy"],
    package_dir={"": "src"},
    ext_modules=ext_modules,
)
