PLATFORM=EDISON
LIBS=-lmraa -lcurl

wifiRC: main.o
	g++ main.o -o wifiRC $(LIBS)

main.o:
	g++ -D$(PLATFORM) -std=c++11 -c main.cpp -o main.o

clean:
	-@rm *.o wifiRC
