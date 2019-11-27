# -*- coding:utf-8 -*-

from setuptools import setup, Extension
import os
import sys
import platform
import re


data_files = {"xmpp_client.py":"EbXmpp"}
PyVersion = sys.version[0:3]
PyLibDir = filter(lambda x: "-packages" in x, sys.path)
PyLibDir = [re.sub("(^.+?lib)(.*)$", r"\1", x) for x in PyLibDir if x]
PyLibDir = list(set(PyLibDir))
PyLibDir.append(".")
PyLibs = []
CompileArgs = []

if "window" in platform.platform().lower():
    PyInclude = [x.replace("lib", "include") for x in PyLibDir]
    PyLibs.append("python%s" % PyVersion.replace(".", ""))
    CompileArgs.append("/std:c++latest")
    data_files["QXmpp.dll"]="EbXmpp"
else:
    PyInclude = [x.replace("lib", "include/python%sm" % PyVersion) for x in PyLibDir]
    PyLibs.append("pthread")
    CompileArgs.append("--std=c++11")
    PyLibs.append("python%sm" % PyVersion)


PyLibs.append("QXmpp")  # from qmake
PyLibDir.append("./xmpp_client")

setup(
    name="EbXmpp", version="0.0.2",
    license="LGPL",
    platforms=["unix", "windows"],
    install_requires=["PyQt5>=0.1"],
    data_files=data_files,
    ext_modules=[
        Extension("EbXmpp",
                  sources=["EbXmpp.cpp"],
                  library_dirs=PyLibDir,
                  libraries=PyLibs,
                  include_dirs=PyInclude,
                  extra_compile_args=CompileArgs,
                  )
    ],
)
