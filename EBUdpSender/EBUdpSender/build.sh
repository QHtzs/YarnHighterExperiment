
PHOME=/usr/local/python374

#g++ -fPIC -shared udpwrapper.cpp  -o libmudp.so  -DUDPWRAPPER_LIBRARY --std=c++11 -lpthread


#g++ -fPIC -shared  udpmodule.cpp -L${PHOME}/lib -lpython3.7m -o EBUdpWrap.pyd --std=c++11 -I${PHOME}/include/python3.7m -L. -lmudp -lpthread


g++ -fPIC -pthread -c udpwrapper.cpp udpmodule.cpp  --std=c++11 -I${PHOME}/include/python3.7m

g++ -pthread -shared udpwrapper.o udpmodule.o -o EBUdpWrap.pyd -L${PHOME}/lib -lpython3.7m

rm -rf udpwrapper.o udpmodule.o
