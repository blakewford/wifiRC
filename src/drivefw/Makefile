PLATFORM=EDISON
LIBS=-lmraa -lcurl
DEVICE_NUMBER=6

wifiRC: main.o parser.o
	g++ -pthread $? -o wifiRC $(LIBS)

main.o: main.cpp
	g++ -D$(PLATFORM) -DDEV=$(DEVICE_NUMBER) -std=c++11 -c $< -o $@

parser.o: parser.cpp
	g++ -c $< -o $@

clean:
	-@rm *.o wifiRC
