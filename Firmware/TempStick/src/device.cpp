#include "device.hpp"

void Device::ButtonPress(){
    switch (screen_state_){
    case ScreenState_t::SCREEN_INTERNAL:
        screen_state_ = ScreenState_t::SCREEN_EXTERNAL;
        break;
    case ScreenState_t::SCREEN_EXTERNAL:
        screen_state_ = ScreenState_t::SCREEN_OFF;
        break;
    case ScreenState_t::SCREEN_OFF:
        screen_state_ = ScreenState_t::SCREEN_INTERNAL;
        break;
    // case ScreenState_t::SCREEN_WIFI:
    //     screen_state_ = ScreenState_t::SCREEN_INTERNAL;
    //     break;
    default:
        screen_state_ = ScreenState_t::SCREEN_INTERNAL;
        break;
    }
}

void Device::UpdateInternalValues(std::tuple<float, float, float> new_values){
    internal_.temperature = std::get<0>(new_values);
    internal_.humidity = std::get<1>(new_values);
    internal_.pressure = std::get<2>(new_values);
}

std::tuple<float, float, float> Device::GetInternalValues(){
    return {internal_.temperature, internal_.humidity, internal_.pressure};
}

void Device::UpdateExternalValues(float new_value){
    external_.temperature = new_value;
}

float Device::GetExternalValues(){
    return external_.temperature;
}

void Device::InternalAvailable(bool status){
    internal_.isAvailable = status;
}

void Device::ExternalAvailable(bool status){
    external_.isAvailable = status;
}