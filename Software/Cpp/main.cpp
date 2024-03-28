#include<iostream>
#include<string>
#include<sstream>
#include<algorithm>
#include<vector>
#include"device.hpp"

#include <fcntl.h>
#include <errno.h>
#include <termios.h>
#include <unistd.h>
#include "logger.hpp"

#define TEST 1

char read_buf[512];
int serial_port;
struct termios tty;

int logger_counter = 0;
Logger logger("out/temp_stick");

int init_serial(std::string path, int& serial_port, struct termios& tty){
    serial_port = open(path.c_str(), O_RDONLY);
    if (serial_port < 0){
        std::cout << "Failed to open device\n";
        return -1;
    }

    if(tcgetattr(serial_port, &tty) != 0){
        std::cout << "Failed getting config from device\n";
    }
    std::cout << "Initialized device\n";

    tty.c_cflag &= ~PARENB;
    tty.c_cflag &= ~CSTOPB;
    tty.c_cflag |= CS8;
    tty.c_cflag &= ~CRTSCTS;
    tty.c_cflag |= CREAD | CLOCAL;

    tty.c_lflag |= ICANON; // Canonical mode, buffer to end line
    tty.c_lflag &= ~ECHO;
    tty.c_lflag &= ~ECHOE;
    tty.c_lflag &= ~ECHONL;
    tty.c_lflag &= ~ISIG;

    tty.c_iflag &= ~(IXON | IXOFF | IXANY);
    tty.c_iflag &= ~(IGNBRK | BRKINT | PARMRK | ISTRIP | INLCR | IGNCR | ICRNL);

    tty.c_oflag &= ~OPOST;
    //tty.c_oflag &= ~ONLCR;
    tty.c_oflag |= ONLCR;

    tty.c_cc[VTIME] = 10;
    tty.c_cc[VMIN] = 0;

    cfsetispeed(&tty, B9600);
    cfsetospeed(&tty, B9600);
    if(tcsetattr(serial_port, TCSANOW, &tty) != 0){
        std::cout << "Error configuring device\n";
    }
    std::cout << "Configured device\n";
    return 0;
}

int main(int argc, char const *argv[]){
    Device dev;
    
    #if(!TEST)
        if(init_serial("/dev/temp-stick", serial_port, tty)){
    #else
        if(init_serial("simulator/virtual-device", serial_port, tty)){
    #endif
            exit(1);
        }

    while(true){
        //Debug clear the buffer before new read
        for(int i = 0; i < 512; ++i){
            read_buf[i] = 0;
        }
        (void) read(serial_port, &read_buf, sizeof(read_buf));
        std::string data = std::string(read_buf);

        std::cout << "Data received: " << data << std::endl;

        if(!dev.parse_input(data)){
            std::cout << "Failed parsing data\n";
        }

        if(logger_counter < 60){
            logger_counter++;
        }else{
            logger.Log(dev.to_string());
            logger_counter = 0;
        }
    }

    return 0;
}
