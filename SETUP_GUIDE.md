# IoT Predictive Maintenance - Setup Guide

## Hardware Setup

### Components Required
- Raspberry Pi 4 (or newer)
- MicroSD card (32GB recommended)
- Vibration sensor (SW-420 or similar)
- Temperature sensor (DHT22 or DS18B20)
- Jumper wires
- Breadboard
- Power supply for Raspberry Pi

### Wiring Diagram

#### Vibration Sensor (SW-420)
- VCC → 5V (Pin 2)
- GND → Ground (Pin 6)
- DO (Digital Out) → GPIO 17 (Pin 11)

#### Temperature Sensor (DHT22)
- VCC → 3.3V (Pin 1)
- GND → Ground (Pin 9)
- DATA → GPIO 18 (Pin 12)

## Software Installation

### 1. Raspberry Pi OS Setup
```bash
# Update system
sudo apt update && sudo apt upgrade -y

# Install Python and pip
sudo apt install python3 python3-pip -y

# Install Git
sudo apt install git -y
```

### 2. Clone Repository
```bash
git clone https://github.com/navin-vijay/IoT-predictive-maintenance.git
cd IoT-predictive-maintenance
```

### 3. Install Python Dependencies
```bash
# Install required packages
pip3 install -r requirements.txt

# For DHT22 sensor (if using real sensor instead of simulation)
pip3 install Adafruit-DHT
```

### 4. Enable GPIO
```bash
# Enable GPIO interface
sudo raspi-config
# Navigate to: Interfacing Options → GPIO → Enable
```

## Configuration

### Sensor Configuration
Edit `config.py` to match your hardware setup:
- Adjust GPIO pin numbers if needed
- Modify sensor reading intervals
- Set appropriate temperature/vibration ranges

### Database Setup
The SQLite database will be created automatically when you first run the script.

## Running the System

### 1. Test Run
```bash
# Run the main script
python3 IoT-predictive-maintenance.py
```

### 2. Run as Service (Optional)
Create a systemd service to run automatically:

```bash
# Create service file
sudo nano /etc/systemd/system/iot-maintenance.service
```

Add the following content:
```ini
[Unit]
Description=IoT Predictive Maintenance
After=network.target

[Service]
Type=simple
User=pi
WorkingDirectory=/home/pi/IoT-predictive-maintenance
ExecStart=/usr/bin/python3 /home/pi/IoT-predictive-maintenance/IoT-predictive-maintenance.py
Restart=always

[Install]
WantedBy=multi-user.target
```

Enable and start the service:
```bash
sudo systemctl enable iot-maintenance.service
sudo systemctl start iot-maintenance.service
```

## Power BI Setup

### 1. Install Power BI Desktop
Download from Microsoft's official website.

### 2. Import Data
1. Open Power BI Desktop
2. Click "Get Data" → "Text/CSV"
3. Select `machine_data.csv` from your project directory
4. Load the data

### 3. Create Visualizations
- Line chart for vibration trends over time
- Line chart for temperature trends over time
- Card visual for latest readings
- Table for anomaly alerts

## Troubleshooting

### Common Issues

#### GPIO Permission Denied
```bash
sudo usermod -a -G gpio pi
```

#### Module Import Errors
```bash
# Reinstall packages
pip3 uninstall -r requirements.txt
pip3 install -r requirements.txt
```

#### Database Lock Issues
```bash
# Stop the service if running
sudo systemctl stop iot-maintenance.service
# Remove database file
rm machine_health.db
# Restart the script
```

### Logs and Monitoring
- Check system logs: `journalctl -u iot-maintenance.service -f`
- Monitor CSV output: `tail -f machine_data.csv`

## Support
For issues or questions, please create an issue on GitHub.
