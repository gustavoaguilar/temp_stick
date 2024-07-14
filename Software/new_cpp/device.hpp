#pragma once

#include<string>
#include<limits>

struct TempStickData{
    float temperature;
    float humidity;
    float pressure;
    float probe_temperature;
};

class TempStick{
public:
    TempStick();
    TempStickData getData();
    TempStickData getMaxData();
    TempStickData getMinData();
    std::string getName();

    void resetData();

    bool parseSerialLine(std::string input_str);
    std::string to_string();
private:
    std::string name;
    TempStickData data;
    TempStickData max_data;
    TempStickData min_data;

    void parse_internal(std::string input);
    void update_min_max();
};