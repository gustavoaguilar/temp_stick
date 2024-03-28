#include <iostream>
#include <thread>
#include <chrono>

using namespace std;

float temp = 0;
float humi = 0;
float pres = 0;
float prob = 0;

int main() {
  string message = "TempStick Test | 12.3, 45.6, 78.9 | 32.1" + to_string(temp);

  while (true) {
    char blabla[64];
    snprintf(blabla, 64, "TempStick Test | %.2f, %.2f, %.2f | %.2f", temp, humi, pres, prob);
    message = string(blabla);
    
    cout << message << endl;
    temp += 0.01;
    humi += 0.01;
    pres += 0.01;
    prob += 0.01;
    this_thread::sleep_for(chrono::seconds(1));
  }

  return 0;
}