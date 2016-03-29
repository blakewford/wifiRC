PLATFORM=EDISON
LIBS=-lmraa -lcurl
DEVICE_NUMBER=6

wifiRC: main.o
	g++ -pthread main.o -o wifiRC $(LIBS)

main.o:
	g++ -D$(PLATFORM) -DDEV=$(DEVICE_NUMBER) -std=c++11 -c main.cpp -o main.o

clean:
	-@rm *.o wifiRC
