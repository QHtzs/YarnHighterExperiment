# 编译
## 环境
* 配置好qtbuild.py中 qmake , jom/make路径
* linux
* * Qt （对应版本的qt库及qmake)
* * 有sharedlib(--lib-shared)的python版本
* windows
* * Qt （对应版本的qt库及qmake)
* * QtCreator(windows下 jom) 
* * python对应版本vc build tool
## 动态库生成
* pythonx qtbuild.py 其中pythonx为相应版本的可执行程序软链，如python, python3.5等 
* 动态库会生在当前目录
## Extension 安装
* pythonx setup.py build install
* * windows下 会自动复制需要依赖的动态库到包目录
* * linux下动态库不做复制也不做安装,按需求自行处理

# 使用
* 库中函数需要在QApplication下才能正常使用，即依赖PyQt5
* PyQt5中会导入Qt动态库，程序不对Qt的动态库做引入，使用前先import PyQt5
* * linux下请确保程序能正常找到libQXmpp.so.1
