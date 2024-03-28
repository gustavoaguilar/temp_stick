g++ main.cpp
socat PTY,link=./virtual-device,raw,echo=0 EXEC:./a.out