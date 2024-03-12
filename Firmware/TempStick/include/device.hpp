#pragma once
#include<tuple>

enum class ScreenState_t{
    SCREEN_INTERNAL = 0,
    SCREEN_EXTERNAL,
    SCREEN_BLUETOOTH,
    SCREEN_WIFI,
    SCREEN_OFF,
};

class Device{
public:
    bool IsInternalAvailable() const {return internal_.isAvailable;}
    bool IsExternalAvailable() const {return external_.isAvailable;}

    void InternalAvailable(bool status);
    void ExternalAvailable(bool status);

    void ButtonPress();
    void UpdateInternalValues(std::tuple<float, float, float> new_values);
    std::tuple<float, float, float> GetInternalValues();
    
    void UpdateExternalValues(float new_value);
    float GetExternalValues();

    ScreenState_t ScreenState(){ return screen_state_;}
private:
    struct{
        bool isAvailable;
        float temperature;
        float humidity;
        float pressure;
    }internal_; // bme280
    
    struct{
        bool isAvailable;
        float temperature;
    }external_; // onewire sensor... TODO: get the name of the sensor

    ScreenState_t screen_state_ = ScreenState_t::SCREEN_INTERNAL;
};