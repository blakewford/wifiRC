DEVICE_NUMBER=6

wifiRC: main.o parser.o platform.o
	g++ -pthread $? -o wifiRC -lcurl

main.cpp: drivefw.ino
	ln -fs drivefw.ino main.cpp

main.o: main.cpp
	g++ -DDESKTOP -DDEV=$(DEVICE_NUMBER) -std=c++11 -c $< -o $@

parser.o: parser.cpp
	g++ -c $< -o $@

platform.o: platform.cpp
	g++ -DDESKTOP -std=c++11 -c $< -o $@

clean:
	-@rm *.o wifiRC main.cpp
