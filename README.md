To compile

g++ queueapi.cpp -o queueapi.o -std=c++11 -c -fPIC -Wall

g++ -shared -o libq.so queueapi.o

gcc thpool.c -lq -lpthread -w


and to run ./a.out
