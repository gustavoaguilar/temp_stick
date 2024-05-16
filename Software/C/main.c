#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <termios.h>
#include <string.h>
#include <ctype.h>

//maybe?? error in the CRTSCTS flag
#include <bits/termios-baud.h>
#include <asm-generic/termbits-common.h>

#define ARRAY_LEN(a) (sizeof(a) / sizeof(a[0]))

struct device_data{
    char name[64];
    float temperature;
    float humidity;
    float pressure;
    float probe;
};

// Stealed from Gemini
void trim(char str[]) {
  int i, j, len = strlen(str);

  // Find the first non-whitespace character
  i = 0;
  while (i < len && isspace((unsigned char)str[i])) {
    i++;
  }

  // Find the last non-whitespace character
  j = len - 1;
  while (j >= 0 && isspace((unsigned char)str[j])) {
    j--;
  }

  // Move remaining characters to the beginning of the string
  if (i < j) {
    for (int k = i; k <= j; k++) {
      str[k - i] = str[k];
    }
  }

  // Null-terminate the string at the new end
  str[j + 1] = '\0';
}

FILE* serial_init(const char* path, struct termios* tty){
    FILE* serial_port = fopen(path, "r");

    if(serial_port < 0){
        printf("Failed opening: %s\n", path);
        return serial_port;
    }

    int serial_fd = fileno(serial_port);

    if(tcgetattr(serial_fd, tty) != 0){
        printf("Failed getting atribute from serial port\n");
    }

    // TODO: Create a function to build this struct
    tty->c_cflag &= ~PARENB;
    tty->c_cflag &= ~CSTOPB;
    tty->c_cflag |= CS8;
    tty->c_cflag &= ~CRTSCTS;
    tty->c_cflag |= CREAD | CLOCAL;

    tty->c_lflag |= ICANON; // Canonical mode, buffer to end line
    tty->c_lflag &= ~ECHO;
    tty->c_lflag &= ~ECHOE;
    tty->c_lflag &= ~ECHONL;
    tty->c_lflag &= ~ISIG;

    tty->c_iflag &= ~(IXON | IXOFF | IXANY);
    tty->c_iflag &= ~(IGNBRK | BRKINT | PARMRK | ISTRIP | INLCR | IGNCR | ICRNL);

    tty->c_oflag &= ~OPOST;
    //tty->c_oflag &= ~ONLCR;
    tty->c_oflag |= ONLCR;

    tty->c_cc[VTIME] = 10;
    tty->c_cc[VMIN] = 0;

    cfsetispeed(tty, B9600);
    cfsetospeed(tty, B9600);
    if(tcsetattr(serial_fd, TCSANOW, tty) != 0){
        printf("Error configuring device\n");
    }
    return serial_port;
}

int device_data_parse(struct device_data* dd, const char* data){
    int tok_counter = 0;
    char mut_data[124];
    char token_list[3][124];

    strncpy(mut_data, data, ARRAY_LEN(mut_data));
    char* tok = strtok(mut_data, "|");

    // Extract tokens
    while(tok != NULL){
        trim(tok);
        strncpy(token_list[tok_counter], tok, 124);
        tok = strtok(NULL, "|");
        tok_counter++;
        if(tok_counter > 3){
            break; // too many stuff
        }
    }

    // Check if all fields exist (Name | Data | Data)
    if(tok_counter != 3){
        printf("Failed parsing data: [%s]\n", data);
        return -1;
    }

    strncpy(dd->name, token_list[0], 64);


    // FIXME: Looks like those scanf operations "round" the float
    if(strncmp(token_list[1], "no_data", 8) != 0){
        int ret = sscanf(token_list[1], "%f, %f, %f", &(dd->temperature), &(dd->humidity), &(dd->pressure));
        if(ret < 3){
            printf("Failed reading internal values\n");
        }
    }

    if(strncmp(token_list[2], "no_data", 8) != 0){
        int ret = sscanf(token_list[2], "%f", &(dd->probe));
        if(ret < 1){
            printf("Failed reading probe value\n");
        }
    }

    printf("data: %s | %.2f, %.2f, %.2f | %.2f\n", dd->name, dd->temperature, dd->humidity, dd->pressure, dd->probe);
    return 0;
}

int main(){
    struct termios termios;
    struct device_data dd;
    char buf[512];

    FILE* serial_fd = serial_init("../Cpp/simulator/virtual-device", &termios);

    int ret;
    while(1){
        fflush(stdout); //FIXME: IDK why yet!

        ret = fscanf(serial_fd, "%[^\n]\n", buf);
        if(ret > 0){
            device_data_parse(&dd, buf);
        }

        sleep(1);
    }

    fclose(serial_fd);
    return 0;
}