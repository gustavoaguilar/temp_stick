import curses
import time
from curses import wrapper
import serial
import sys

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

    sensor_temp_max = sys.float_info.min
    sensor_temp_min = sys.float_info.max

    sensor_humi_max = sys.float_info.min
    sensor_humi_min = sys.float_info.max

    sensor_pres_max = sys.float_info.min
    sensor_pres_min = sys.float_info.max

    probe_temp_max = sys.float_info.min
    probe_temp_min = sys.float_info.max

    port = detect_device(stdscr)
    content = port.read()
    
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
                    temp = float(values[0])

                    sensor_temp_min = temp if temp < sensor_temp_min else sensor_temp_min
                    sensor_temp_max = temp if temp > sensor_temp_max else sensor_temp_max

                    humi = float(values[1])
                    sensor_humi_min = humi if humi < sensor_humi_min else sensor_humi_min
                    sensor_humi_max = humi if humi > sensor_humi_max else sensor_humi_max
                    
                    pres = float(values[2])
                    sensor_pres_min = pres if pres < sensor_pres_min else sensor_pres_min
                    sensor_pres_max = pres if pres > sensor_pres_max else sensor_pres_max
                    
                    stdscr.addstr(sensor_string.format(temp, sensor_temp_max, sensor_temp_min, humi, sensor_humi_max, sensor_humi_min, pres, sensor_pres_max, sensor_pres_min))
                else:
                    stdscr.addstr("No Sensor Detected")
            else:
                if(data != "no_data"):
                    probe_temp = float(data)
                    probe_temp_min = probe_temp if probe_temp < probe_temp_min else probe_temp_min
                    probe_temp_max = probe_temp if probe_temp > probe_temp_max else probe_temp_max
                    stdscr.addstr("Probe:\n")
                    stdscr.addstr("\tTemperature: {} C\t(Max: {} | Min: {})".format(probe_temp, probe_temp_max, probe_temp_min))
                else:
                    stdscr.addstr("\tNo Probe Detected")

        stdscr.refresh()

wrapper(main)
    
