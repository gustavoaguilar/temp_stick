#include "device.hpp"
#include <iostream>
#include <sstream>
#include <vector>
#include <algorithm>

bool Device::parse_input(const std::string input){
    std::string input_tmp(input.c_str());
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
            if(!result_vector[index].find("no_data") != std::string::npos){
                prob = std::stof(result_vector[index]);
            }
            break;        
        default:
            break;
        }
    }

    return true;
}


void Device::parse_internal(std::string input){
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
            temp = std::stof(tmp);
            break;
        case HUMI:
            humi = std::stof(tmp);
            break;
        case PRES:
            pres = std::stof(tmp);
            break;
        default:
            std::cout << "Error parsing internal sensor data\n";
            break;
        }
        i++;
    }
}

void Device::print(void){
    std::cout << name << std::endl;
    
    std::cout << "Temperature: " << temp << std::endl;
    std::cout << "Humidity: " << humi << std::endl;
    std::cout << "Pressure: " << pres << std::endl;

    std::cout << "Probe: " << prob << std::endl;
}

std::string Device::to_string(){
    char output[64];

    snprintf(output, 64, "%s,%.2f,%.2f,%.2f,%.2f\n", name.c_str(), temp, humi, pres, prob);
    return std::string(output);
}