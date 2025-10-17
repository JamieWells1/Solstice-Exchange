from setuptools import setup, Extension
from setuptools.command.build_ext import build_ext
import pybind11

ext_modules = [
    Extension(
        "strategy.py_interface",
        ["src/strategy/py_interface.cpp", "src/strategy/strategy.cpp"],
        include_dirs=[
            pybind11.get_include(),
            "src",
            "src/strategy",
            "src/common",
            "src/utils",
            "src/enums"
        ],
        language="c++",
        extra_compile_args=["-std=c++23"],
    ),
]

setup(
    name="solstice",
    packages=["strategy"],
    package_dir={"": "src"},
    ext_modules=ext_modules,
)
