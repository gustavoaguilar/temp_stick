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
    snprintf(blabla, 64, "TempStickTest | %.2f, %.2f, %.2f | %.2f", temp, humi, pres, prob);
    //snprintf(blabla, 64, "TempStickTest | no_data | %.2f", prob);
    //snprintf(blabla, 64, "TempStickTest | %.2f, %.2f, %.2f | no_data", temp, humi, pres);
    //snprintf(blabla, 64, "TempStickTest | %.2f, %.2f, %.2f | %.2f | 13 |tempstick|513", temp, humi, pres, prob);
    //snprintf(blabla, 64, "TempStickTest | 123, 3");
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
