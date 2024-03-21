#pragma once

#include<string>
#include<vector>

struct Device{
    std::string name;
    float temp;
    float humi;
    float pres;
    float prob;

    std::vector<std::string> parse_input(const std::string input);
    void parse_internal(std::string input);
    void print(void);
    std::string to_string();
};