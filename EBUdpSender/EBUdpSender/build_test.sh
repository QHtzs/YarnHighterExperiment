
g++ -fPIC -shared udpwrapper.cpp  -o libmudp.so  -DUDPWRAPPER_LIBRARY --std=c++11 -lpthread

g++ testSender.cpp -o test_sender -L. -lmudp -lpthread --std=c++11
g++ testServer.cpp -o test_server -L. -lmudp -lpthread --std=c++11
