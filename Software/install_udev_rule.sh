# Installs the udev rules in the system
sudo cp 99-temp-stick.rules /etc/udev/rules.d/
# Reload the udev rules
sudo udevadm control --reload-rules && sudo udevadm trigger
