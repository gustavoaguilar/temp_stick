import curses
import time
from curses import wrapper
import serial
import sys
import csv
from datetime import datetime

class Device:
    temp = 0
    temp_max = sys.float_info.min
    temp_min = sys.float_info.max

    humi = 0
    humi_max = sys.float_info.min
    humi_min = sys.float_info.max

    pres = 0
    pres_max = sys.float_info.min
    pres_min = sys.float_info.max

    probe_temp = 0
    probe_temp_max = sys.float_info.min
    probe_temp_min = sys.float_info.max

    def __init__(self):
        pass
    def __str__(self):
        pass
    
    def decode_sensor(self, data):
        self.temp = float(data[0])

        self.temp_min = self.temp if self.temp < self.temp_min else self.temp_min
        self.temp_max = self.temp if self.temp > self.temp_max else self.temp_max

        self.humi = float(data[1])
        self.humi_min = self.humi if self.humi < self.humi_min else self.humi_min
        self.humi_max = self.humi if self.humi > self.humi_max else self.humi_max
        
        self.pres = float(data[2])
        self.pres_min = self.pres if self.pres < self.pres_min else self.pres_min
        self.pres_max = self.pres if self.pres > self.pres_max else self.pres_max
    
    def decode_probe(self, data):
        self.probe_temp = float(data)
        self.probe_temp_min = self.probe_temp if self.probe_temp < self.probe_temp_min else self.probe_temp_min
        self.probe_temp_max = self.probe_temp if self.probe_temp > self.probe_temp_max else self.probe_temp_max

    def csv_header(self, csvWrite):
        header = ["date", "temperature", "humidity", "pressure", "probe_temperature"]
        csvWrite.writerow(header)

    def csv_row(self, csvWrite):
        data = [datetime.now().strftime("%d/%m/%Y %H:%M:%S"), self.temp, self.humi, self.pres, self.probe_temp]
        csvWrite.writerow(data)

        pass

sensor_string = "Sensor:\n\tTemperature: {} C\t(Max: {} | Min: {})\n\tHumidity: {} pc\t(Max: {} | Min: {})\n\tPressure: {} hPa\t(Max: {} | Min: {})\n"


def detect_device(stdscr):
    while True:
        try:
            port = serial.Serial(sys.argv[1], 9600, timeout=5);
        except:
            port = serial.Serial()
        
        if(port.is_open):
            return port
        else:
            stdscr.clear()
            stdscr.addstr("Device not found at {}".format(sys.argv[1]))
            stdscr.refresh()
            time.sleep(1)

def main (stdscr):
    device = Device()
    port = detect_device(stdscr)
    content = port.read()
    
    csvFile = open("data.csv", 'w', encoding='UTF8')
    csvWriter = csv.writer(csvFile)
    device.csv_header(csvWriter)

    start_time = time.time()
    while content:
        stdscr.clear()
        
        try:
            content = port.readline().decode()
        except:
            port.close()
            port = detect_device(stdscr)

        content = content.replace("<", "");
        content = content.replace(">", "");
        content = content.replace(" ", "");
        content = content.strip();
        x = content.split(sep='|')

        for i, data in enumerate(x):
            if i == 0:
                stdscr.addstr("Device name: {}\n".format(data))
            elif i == 1:
                if(data != "no_data"):
                    values = data.split(',');
                    device.decode_sensor(values)
                    stdscr.addstr(sensor_string.format(device.temp, device.temp_max, device.temp_min, device.humi, device.humi_max, device.humi_min, device.pres, device.pres_max, device.pres_min))
                else:
                    stdscr.addstr("No Sensor Detected")
            else:
                if(data != "no_data"):
                    device.decode_probe(data)
                    stdscr.addstr("Probe:\n")
                    stdscr.addstr("\tTemperature: {} C\t(Max: {} | Min: {})".format(device.probe_temp, device.probe_temp_max, device.probe_temp_min))
                else:
                    stdscr.addstr("\tNo Probe Detected")

        end_time = time.time()
        if (sys.argv[2]):
            if(end_time - start_time >= float(sys.argv[2])):
                device.csv_row(csvWriter)
                start_time = end_time

        time.sleep(1)
        stdscr.refresh()
    csvFile.close()

wrapper(main)