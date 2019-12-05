# -*- coding:utf-8 -*-

import os
import sys
import warnings
import platform
import re


# windows下需要相关环境，比如
# set PATH=%PATH%;"C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\VC\Auxiliary\Build"
# "C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\VC\Auxiliary\Build\vcvarsall.bat" amd64
# 可能找不到rc，需要把相应windows kit bin加入PATh
# windows下 python3.5+一般是支持msvc2015编译工具
if platform.platform().lower().__contains__("win"):
    q_make_home = r"D:\Qt\Qt5.12.2\5.12.2\msvc2017_64\bin"
    q_creator_home = r"D:\Qt\Qt5.12.2\Tools\QtCreator\bin"
    if q_make_home == "":
        warnings.warn("qmake home is not assigned, using current dir")
    q_make = os.path.join(q_make_home, "qmake")
    os.system('%s Pushing.pro -spec win32-msvc "CONFIG+=qtquickcompiler"' % q_make)
    if q_creator_home == "":
        warnings.warn("jom home is not assigned, using current dir")
    make = os.path.join(q_creator_home, "jom.exe")
    os.system("%s qmake_all && %s" % (make, make))
    os.system(r"xcopy .\release\QXmpp.dll  .\  /s/q")
    os.system(r"xcopy .\release\QXmpp.lib  .\  /s/q")
    os.system(r"xcopy .\release\libQXmpp.a .\  /s/q")   # gm
    os.system(r"rd /s/q .\release")
    os.system(r"rd /s/q .\debug")

elif "linux" in platform.platform().lower():
    os.system('qmake Pushing.pro -spec linux-g++ CONFIG+=qtquickcompiler')
    os.system("make qmake_all && make -j2")
    os.system("rm -rf moc_*")
    os.system("rm -rf *.o")

else:
    raise EnvironmentError("platform is not supported")


files = os.listdir("./")
files = filter(lambda x: "Makefile" in x or "stash" in x, files)
for file in files:
    os.unlink("./%s" % file)
