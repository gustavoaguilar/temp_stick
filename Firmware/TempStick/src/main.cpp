#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_I2CDevice.h>
#include <Adafruit_BME280.h>

#include <OneWire.h>
#include <DallasTemperature.h>

#define DEVICE_STRING "<TempStick 0> | "
#define BUTTON_PIN 0

Adafruit_BME280 bme280;
Adafruit_SSD1306 display = Adafruit_SSD1306(128, 32, &Wire);

OneWire oneWire(10);
DallasTemperature external_sensor(&oneWire);
DeviceAddress temp_addr;

struct{
    bool isAvailable;
    float temperature;
    float humidity;
    float pressure;
}internal_data;

struct{
    bool isAvailable;
    float temperature;
}external_data;

typedef enum {
    SCREEN_INTERNAL = 0,
    SCREEN_EXTERNAL,
    SCREEN_BLUETOOTH,
}ScreenState;

ScreenState screenState = SCREEN_INTERNAL;

void buttonPress(void){
    switch (screenState){
    case SCREEN_INTERNAL:
        screenState = SCREEN_EXTERNAL;
        break;
    case SCREEN_EXTERNAL:
        screenState = SCREEN_INTERNAL;
        break;
    // case SCREEN_BLUETOOTH:
    //     screenState = SCREEN_INTERNAL;
    //     break;
    default:
        screenState = SCREEN_INTERNAL;
        break;
    }
}

void detectBME280(void){
    if(!bme280.begin(0x76, &Wire)){
        internal_data.isAvailable = false;
    }else{
        internal_data.isAvailable = true;
    }
}

void getDataFromSensor(void* parameter){
    detectBME280();
    external_sensor.begin();
    
    while(true){
        if(internal_data.isAvailable){
            internal_data.temperature = bme280.readTemperature();
            internal_data.humidity = bme280.readHumidity();
            internal_data.pressure = bme280.readPressure() / 100.0;
        } else {
            internal_data.temperature = 0;
            internal_data.humidity = 0;
            internal_data.pressure = 0;
            detectBME280();
        }

        external_sensor.requestTemperatures(); // Send the command to get temperatures
        if(!external_sensor.getAddress(temp_addr, 0)){
            external_data.isAvailable = false;
            external_data.temperature = 0;
        } else {
            external_data.temperature = external_sensor.getTempC(temp_addr);
            external_data.isAvailable = true;
        }

        vTaskDelay(1000);
    }
}

void sendDataSerial(void* parameter){
    Serial.begin(9600);
    while(true){
        Serial.print(DEVICE_STRING);

        //Internal Sensor
        if(internal_data.isAvailable)
            Serial.printf("%.02f, %.02f, %.02f | ",
                        internal_data.temperature,
                        internal_data.humidity,
                        internal_data.pressure);
        else
            Serial.printf("no_data | ");
        
        //External Sensor
        if(external_data.isAvailable)
            Serial.printf("%.02f\n", external_data.temperature);
        else
            Serial.printf("no_data\n");
        
        vTaskDelay(1000);
    }
}

void buttonPressSimulator(void* parameter){
    while(true){
        buttonPress();
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
        
        switch (screenState){
        case SCREEN_INTERNAL:
            if(internal_data.isAvailable){
                display.setCursor(0,0);
                display.printf("Temperature: %.02fC\nHumidity: %.02fpc\nPressure: %.02fhPa\nSensor: Internal",
                    internal_data.temperature,
                    internal_data.humidity,
                    internal_data.pressure);
            } else {
                display.setCursor(0,0);
                display.printf("Internal sensor not available");
            }
            break;
        case SCREEN_EXTERNAL:
            display.setCursor(0,0);
            if(external_data.isAvailable){
                display.printf("Temperature: %.02fC\n\n\nSensor: Probe",
                    external_data.temperature);
            }else{
                display.printf("Sensor not connected\n\n\nSensor: Probe");
            }
            break;
        case SCREEN_BLUETOOTH:
            display.setCursor(0,0);
            display.print("Bluetooth");
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

    while(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)){
        // Serial.println("Failed Initializing display");
        vTaskDelay(1000);
    }

    xTaskCreate(
        burningAvoider,   /* Task function. */
        "BurningAvoider", /* String with name of task. */
        10000,            /* Stack size in bytes. */
        NULL,             /* Parameter passed as input of the task */
        1,                /* Priority of the task. */
        NULL);            /* Task handle. */

    xTaskCreate(sendDataSerial, "SerialData", 10000, NULL, 1, NULL);
    xTaskCreate(screenDisplay, "Display", 10000, NULL, 1, NULL);
    xTaskCreate(getDataFromSensor, "GetData", 10000, NULL, 1, NULL);
    // xTaskCreate(buttonPressSimulator, "SimulatedButton", 10000, NULL, 1, NULL);   
}

void loop(){
    if(digitalRead(BUTTON_PIN)){
        buttonPress();
        delay(1000);
    }
}