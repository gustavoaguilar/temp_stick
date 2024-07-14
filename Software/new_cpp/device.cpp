#include "device.hpp"
#include <iostream>
#include <sstream>
#include <vector>
#include <algorithm>
#include <format>

TempStick::TempStick(){
    resetData();
}

TempStickData TempStick::getData(){
    return data;
}

TempStickData TempStick::getMaxData(){
    return max_data;
}

TempStickData TempStick::getMinData(){
    return min_data;
}

std::string TempStick::getName(){
    return name;
}

void TempStick::resetData(){
    data = TempStickData{0};
    max_data = TempStickData{std::numeric_limits<float>::min(),
                                std::numeric_limits<float>::min(),
                                std::numeric_limits<float>::min(),
                                std::numeric_limits<float>::min()};
    min_data = TempStickData{std::numeric_limits<float>::max(),
                                std::numeric_limits<float>::max(),
                                std::numeric_limits<float>::max(),
                                std::numeric_limits<float>::max()};
    name = std::string("");
}

std::string TempStick::to_string(){
    std::string internal = std::format("{} ({} ~ {}) C | {} ({} ~ {}) pc | {} ({} ~ {}) hPa",
                                    data.temperature, min_data.temperature, max_data.temperature,
                                    data.humidity, min_data.humidity, max_data.humidity,
                                    data.pressure, min_data.pressure, max_data.pressure);
    std::string probe = std::format("{} ({} ~ {}) C", data.probe_temperature,
                                        min_data.probe_temperature, max_data.probe_temperature);

    return std::format("{} | {} | {}", name, internal, probe);
}

bool TempStick::parseSerialLine(const std::string intpu_str){
    std::string input_tmp(intpu_str.c_str());
    std::vector<std::string> result_vector = std::vector<std::string>();

    input_tmp.erase(remove_if(input_tmp.begin(), input_tmp.end(), isspace), input_tmp.end());
    // std::cout << input_tmp << std::endl;
    
    std::string tmp;
    std::stringstream ss(input_tmp);
    while(getline(ss, tmp, '|')){
        result_vector.push_back(std::string(tmp.c_str()));
    }

    if(result_vector.size() > 3){
        // too many stuff, abort and send no tokens in the vector
        // std::cout << "There are " << result_vector.size() << " fields in the vector\n";
        result_vector.clear();
        return false;
    }

    for (auto it = result_vector.begin(); it != result_vector.end(); ++it) {
        int index = std::distance(result_vector.begin(), it);
        switch (index){
        case 0:
            name = result_vector[index];
            break;
        case 1:
            parse_internal(result_vector[index]);
            break;
        case 2:
            if(result_vector[index] != "no_data"){
                data.probe_temperature = std::stof(result_vector[index]);
            }
            break;        
        default:
            break;
        }
    }

    update_min_max();

    return true;
}

void TempStick::parse_internal(std::string input){
    if(input.find("no_data") != std::string::npos){
        return;
    }

    enum ValueOrder{
        TEMP = 0,
        HUMI,
        PRES,
    };

    std::string tmp;
    std::stringstream ss(input);
    int i = TEMP;
    while(getline(ss, tmp, ',')){
        switch (i){
        case TEMP:
            data.temperature = std::stof(tmp);
            break;
        case HUMI:
            data.humidity = std::stof(tmp);
            break;
        case PRES:
            data.pressure = std::stof(tmp);
            break;
        default:
            std::cout << "Error parsing internal sensor data\n";
            break;
        }
        i++;
    }
}

void TempStick::update_min_max(){
    if(data.temperature < min_data.temperature){
        min_data.temperature = data.temperature;
    }
    if(data.humidity < min_data.humidity){
        min_data.humidity = data.humidity;
    }
    if(data.pressure < min_data.pressure){
        min_data.pressure = data.pressure;
    }
    if(data.probe_temperature < min_data.probe_temperature){
        min_data.probe_temperature = data.probe_temperature;
    }

    if(data.temperature > max_data.temperature){
        max_data.temperature = data.temperature;
    }
    if(data.humidity > max_data.humidity){
        max_data.humidity = data.humidity;
    }
    if(data.pressure > max_data.pressure){
        max_data.pressure = data.pressure;
    }
    if(data.probe_temperature > max_data.probe_temperature){
        max_data.probe_temperature = data.probe_temperature;
    }
}
