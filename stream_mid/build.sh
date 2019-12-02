g++ -pthread -c configloader.cpp ebdecoder.cpp eboptiondict.cpp udppostsample.cpp udpwrapper.cpp main.cpp --std=c++11 -I/usr/local/include
g++ -pthread  configloader.o ebdecoder.o eboptiondict.o udppostsample.o udpwrapper.o main.o  -L/usr/local/lib  -lavcodec -lavdevice -lavutil -lavformat -lswscale -o streamtool
rm -rf *.o
