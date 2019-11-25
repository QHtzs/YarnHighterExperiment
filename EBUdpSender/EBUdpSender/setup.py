# -*- coding:utf-8 -*-

try:
    from setuptools import setup, Extension
except Exception as e:
    print("warn:import setuptools error %s" % e)
    from distutils.core import setup, Extension

import sys
import re
import platform


PyVersion = sys.version[0:3]
PyLibDir = filter(lambda x: "-packages" in x, sys.path)
PyLibDir = [re.sub("(^.+?lib)(.*)$", r"\1", x) for x in PyLibDir if x]
PyLibDir = list(set(PyLibDir))
PyInclude = [x.replace("lib", "include/python%sm" % PyVersion) for x in PyLibDir]
PyLibDir.append(".")
PyLibs = []
CompileArgs = []
if PyVersion < "3.6":
    CompileArgs.append("-DNoSlot")
if "window" in platform.platform().lower():
    PyLibs.append("ws2_32")
    PyLibs.append("python%s" % PyVersion.replace(".", ""))
    CompileArgs.append("/std:c++latest")
else:
    PyLibs.append("pthread")
    CompileArgs.append("--std=c++11")
    PyLibs.append("python%sm" % PyVersion)


setup(
    name="EBOurs", version="0.0.2",
    maintainer="ExcellentBear",
    license="MIT",
    maintainer_email="https://github.com/ExcellentBear",
    url="https://github.com/ExcellentBear/UdpSimpleWrap",
    platforms=["unix", "windows"],
    python_requires='>=3',
    ext_modules=[
        Extension("EBUdpWrap",
                  sources=["udpwrapper.cpp", "udpmodule.cpp"],
                  library_dirs=PyLibDir,
                  libraries=PyLibs,
                  include_dirs=PyInclude,
                  extra_compile_args=CompileArgs
                  )
    ],
    packages=["simple_wrap"],
)
