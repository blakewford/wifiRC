wifiRC: main.o
	g++ main.o -o wifiRC -lmraa -lcurl

main.o:
	g++ -std=c++11 -c main.cpp -o main.o

clean:
	-@rm *.o wifiRC
