g++ -pthread -c im_buff.cpp ffmpegavdecoder.cpp httprequest.cpp main.cpp --std=c++11 -I/usr/local/include
g++ -pthread im_buff.o ffmpegavdecoder.o httprequest.o main.o -L/usr/local/lib  -lavcodec -lavdevice -lavutil -lavformat -lswscale -o dest
rm -rf *.o

