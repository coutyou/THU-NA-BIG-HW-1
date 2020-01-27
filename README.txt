三种运行方法:

1.终端直接运行程序：
cd Release
chmod +x Interpolation
./ Interpolation

2.使用QTCreator运行：
打开pro文件，编译，运行

3.使用qmake编译+终端运行：
依赖详见Makefile

p.s. 
QT4会缺少文件无法直接运行，需要QT5
如果选择使用QTCreator运行，关于OpenCV的路径依赖，在Interpolation.pro文件中INCLUDEPATH中