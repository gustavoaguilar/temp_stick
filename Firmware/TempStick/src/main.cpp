#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_I2CDevice.h>
#include <Adafruit_BME280.h>

#include <OneWire.h>
#include <DallasTemperature.h>

#include "device.hpp"

#define DEVICE_STRING "<TempStick 0> | "
#define BUTTON_PIN 0

Adafruit_BME280 bme280;
Adafruit_SSD1306 display = Adafruit_SSD1306(128, 32, &Wire);

OneWire oneWire(10);
DallasTemperature external_sensor(&oneWire);
DeviceAddress temp_addr;

Device dev;

void detectBME280(void){
    if(!bme280.begin(0x76, &Wire)){
        dev.InternalAvailable(false);
    }else{
        dev.InternalAvailable(true);
    }
}

void getDataFromSensor(void* parameter){
    detectBME280();
    external_sensor.begin();
    
    while(true){
        if(dev.IsInternalAvailable()){
            dev.UpdateInternalValues({bme280.readTemperature(), bme280.readHumidity(),bme280.readPressure() / 100.0});
        } else {
            dev.UpdateInternalValues({0, 0, 0});
            detectBME280();
        }

        external_sensor.requestTemperatures(); // Send the command to get temperatures
        if(!external_sensor.getAddress(temp_addr, 0)){
            dev.UpdateExternalValues(0);
            dev.ExternalAvailable(false);
        } else {
            dev.UpdateExternalValues(external_sensor.getTempC(temp_addr));
            dev.ExternalAvailable(true);
        }

        vTaskDelay(1000);
    }
}

void sendDataSerial(void* parameter){
    while(true){
        Serial.print(DEVICE_STRING);
        //Internal Sensor
        if(dev.IsInternalAvailable()){
            auto internal_data = dev.GetInternalValues();
            Serial.printf("%.02f, %.02f, %.02f | ",
                        std::get<0>(internal_data),
                        std::get<1>(internal_data),
                        std::get<2>(internal_data));
        } else {
            Serial.printf("no_data | ");
        }
        
        //External Sensor
        if(dev.IsExternalAvailable()){
            auto external_data = dev.GetExternalValues();
            Serial.printf("%.02f\n", external_data);
        } else {
            Serial.printf("no_data\n");
        }
        
        vTaskDelay(1000);
    }
}

void buttonPressSimulator(void* parameter){
    while(true){
        dev.ButtonPress();
        vTaskDelay(1000);
    }
}

void screenDisplay(void* parameter){
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.clearDisplay();
    display.display();
    
    while(true){
        display.clearDisplay();
        
        switch (dev.ScreenState()){
        case ScreenState_t::SCREEN_INTERNAL:
            if(dev.IsInternalAvailable()){
                display.setCursor(0,0);

                auto internal_data = dev.GetInternalValues();
                display.printf("Temperature: %.02fC\nHumidity: %.02fpc\nPressure: %.02fhPa\nSensor: Internal",
                    std::get<0>(internal_data),
                    std::get<1>(internal_data),
                    std::get<2>(internal_data));
            } else {
                display.setCursor(0,0);
                display.printf("Internal sensor not available");
            }
            break;
        case ScreenState_t::SCREEN_EXTERNAL:
            display.setCursor(0,0);
            if(dev.IsExternalAvailable()){
                display.printf("Temperature: %.02fC\n\n\nSensor: Probe",
                    dev.GetExternalValues());
            }else{
                display.printf("Sensor not connected\n\n\nSensor: Probe");
            }
            break;
        case ScreenState_t::SCREEN_BLUETOOTH:
            display.setCursor(0,0);
            display.print("Bluetooth");
            break;
        case ScreenState_t::SCREEN_OFF:
            // Do nothing and disables the burn in avoider
            break;
        default:
            break;
        }
        display.display();

        vTaskDelay(1000);
    }
}

void burningAvoider(void* parameter){
    int isInverted = 0;
  
    while(1){
        if(dev.ScreenState() == ScreenState_t::SCREEN_OFF && !isInverted){
            vTaskDelay(1000);
            continue;
        }
        isInverted = !isInverted;
        display.invertDisplay(isInverted);
        if(isInverted)
            vTaskDelay(1000);
        else
            vTaskDelay(10000);
    }
}

void setup(){
    pinMode(BUTTON_PIN, INPUT_PULLDOWN);
    pinMode(22, INPUT_PULLUP);
    pinMode(21, INPUT_PULLUP);
    
    Serial.begin(9600);
    
    while(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)){
        Serial.println("Failed Initializing display");
        vTaskDelay(1000);
    }

    xTaskCreate(
        burningAvoider,   /* Task function. */
        "BurningAvoider", /* String with name of task. */
        10000,            /* Stack size in bytes. */
        NULL,             /* Parameter passed as input of the task */
        3,                /* Priority of the task. */
        NULL);            /* Task handle. */

    xTaskCreate(sendDataSerial, "SerialData", 10000, NULL, 1, NULL);
    xTaskCreate(screenDisplay, "Display", 10000, NULL, 2, NULL);
    xTaskCreate(getDataFromSensor, "GetData", 10000, NULL, 1, NULL);
    // xTaskCreate(buttonPressSimulator, "SimulatedButton", 10000, NULL, 1, NULL);   
}

void loop(){
    if(digitalRead(BUTTON_PIN)){
        dev.ButtonPress();
        delay(1000);
    }
}