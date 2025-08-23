# C++ Implementation Setup Guide

## Overview
This guide covers setting up and running the C++ version of the IoT Predictive Maintenance System. The C++ implementation offers better performance and lower memory usage compared to Python.

## Prerequisites

### System Requirements
- Raspberry Pi 3B+ or newer
- Raspberry Pi OS (32-bit or 64-bit)
- At least 1GB RAM available
- MicroSD card with 16GB+ storage

### Hardware Components
- Vibration sensor (SW-420)
- Temperature/Humidity sensor (DHT22)
- Jumper wires and breadboard
- Pull-up resistors (4.7kΩ for DHT22)

## Installation Steps

### 1. Update System
```bash
sudo apt update && sudo apt upgrade -y
```

### 2. Install Build Dependencies
```bash
# Install build tools and libraries
sudo make install-deps

# Or manually:
sudo apt install -y build-essential libsqlite3-dev wiringpi git
```

### 3. Clone Repository
```bash
git clone https://github.com/navin-vijay/IoT-predictive-maintenance.git
cd IoT-predictive-maintenance
```

### 4. Build the Application
```bash
# Build the C++ application
make

# Or for debug version
make debug
```

### 5. Set Permissions
```bash
# Make executable and set up environment
make setup
```

## Hardware Wiring

### DHT22 Temperature Sensor
```
DHT22 Pin 1 (VCC) → Raspberry Pi 3.3V (Pin 1)
DHT22 Pin 2 (Data) → Raspberry Pi GPIO 18 (Pin 12) + 4.7kΩ resistor to VCC
DHT22 Pin 3 (NC) → Not connected
DHT22 Pin 4 (GND) → Raspberry Pi Ground (Pin 6)
```

### SW-420 Vibration Sensor
```
SW-420 VCC → Raspberry Pi 5V (Pin 2)
SW-420 GND → Raspberry Pi Ground (Pin 9)
SW-420 DO → Raspberry Pi GPIO 17 (Pin 11)
```

## Configuration

### Pin Configuration
Edit the pin definitions in `iot_maintenance.cpp`:
```cpp
const int VIBRATION_SENSOR_PIN = 0;  // WiringPi pin 0 = GPIO 17
const int TEMP_SENSOR_PIN = 1;       // WiringPi pin 1 = GPIO 18
```

### Timing Configuration
```cpp
const int READING_INTERVAL = 60;     // seconds between readings
const int DATA_COLLECTION_BATCH = 10; // readings before CSV export
```

## Running the Application

### Manual Execution
```bash
# Run the application
make run

# Or directly
./iot_maintenance
```

### As a System Service

#### Create Service File
```bash
sudo nano /etc/systemd/system/iot-maintenance-cpp.service
```

#### Service Configuration
```ini
[Unit]
Description=IoT Predictive Maintenance (C++)
After=network.target

[Service]
Type=simple
User=pi
Group=gpio
WorkingDirectory=/home/pi/IoT-predictive-maintenance
ExecStart=/home/pi/IoT-predictive-maintenance/iot_maintenance
Restart=always
RestartSec=10

[Install]
WantedBy=multi-user.target
```

#### Enable and Start Service
```bash
sudo systemctl daemon-reload
sudo systemctl enable iot-maintenance-cpp.service
sudo systemctl start iot-maintenance-cpp.service
```

#### Monitor Service
```bash
# Check status
sudo systemctl status iot-maintenance-cpp.service

# View logs
journalctl -u iot-maintenance-cpp.service -f
```

## Performance Optimization

### Compiler Optimizations
The Makefile includes optimization flags:
```makefile
CXXFLAGS = -std=c++11 -Wall -Wextra -O2
```

For maximum performance:
```bash
# Build with highest optimization
g++ -std=c++11 -O3 -march=native -o iot_maintenance iot_maintenance.cpp -lwiringPi -lsqlite3 -lpthread
```

### Memory Management
The application automatically manages memory by:
- Limiting stored data points to 100 recent readings
- Using efficient STL containers
- Cleaning up database connections properly

## Advanced Features

### Real Sensor Integration
To use real sensors instead of simulation, uncomment and modify these sections in `iot_maintenance.cpp`:

```cpp
// For DHT22 sensor
#include "sensor_utils.h"

std::pair<double, double> readSensors() {
    DHT22Data dht = SensorUtils::readDHT22(TEMP_SENSOR_PIN);
    double vibration = SensorUtils::readVibration(VIBRATION_SENSOR_PIN);
    
    if (dht.valid) {
        return std::make_pair(vibration, dht.temperature);
    }
    // Fallback to simulation if sensor fails
    return std::make_pair(vibrationDist(gen), tempDist(gen));
}
```

### Custom Anomaly Detection
Replace the simple z-score method with more advanced algorithms:

```cpp
// Example: Isolation Forest equivalent
double advancedAnomalyScore(double vibration, double temperature) {
    // Implement more sophisticated anomaly detection
    // Could integrate with external ML libraries
}
```

## Troubleshooting

### Build Errors

#### WiringPi Not Found
```bash
# Install WiringPi manually
cd /tmp
wget https://project-downloads.drogon.net/wiringpi-latest.deb
sudo dpkg -i wiringpi-latest.deb
```

#### SQLite3 Development Headers Missing
```bash
sudo apt install libsqlite3-dev
```

### Runtime Issues

#### GPIO Permission Denied
```bash
# Add user to gpio group
sudo usermod -a -G gpio $USER
# Logout and login again
```

#### Database Lock Issues
```bash
# Stop service if running
sudo systemctl stop iot-maintenance-cpp.service
# Remove database file
rm machine_health.db
# Restart application
```

### Sensor Reading Issues

#### DHT22 Invalid Readings
- Check wiring connections
- Verify pull-up resistor (4.7kΩ)
- Ensure adequate power supply
- Add delays between readings

#### Vibration Sensor Not Responding
- Test with multimeter
- Check GPIO pin configuration
- Verify sensor power supply

## Performance Monitoring

### System Resources
```bash
# Monitor CPU and memory usage
top -p $(pgrep iot_maintenance)

# Check disk usage
df -h
du -sh machine_health.db
```

### Application Metrics
```bash
# Count database records
sqlite3 machine_health.db "SELECT COUNT(*) FROM machine_data;"

# Check latest readings
sqlite3 machine_health.db "SELECT * FROM machine_data ORDER BY timestamp DESC LIMIT 5;"
```

## Maintenance

### Database Cleanup
```bash
# Remove old records (keep last 1000)
sqlite3 machine_health.db "DELETE FROM machine_data WHERE id NOT IN (SELECT id FROM machine_data ORDER BY timestamp DESC LIMIT 1000);"

# Vacuum database to reclaim space
sqlite3 machine_health.db "VACUUM;"
```

### Log Rotation
```bash
# Set up log rotation for service logs
sudo nano /etc/logrotate.d/iot-maintenance
```

```
/var/log/iot-maintenance.log {
    daily
    rotate 7
    compress
    delaycompress
    missingok
    notifempty
}
```

## Comparison: C++ vs Python

| Feature | C++ Version | Python Version |
|---------|-------------|----------------|
| Performance | ~5-10x faster | Slower |
| Memory Usage | ~50% less | Higher |
| Startup Time | Instant | 2-3 seconds |
| Dependencies | Fewer | More libraries |
| Development | More complex | Easier |
| Debugging | GDB, Valgrind | Rich ecosystem |

## Support and Contributing

For issues specific to the C++ implementation:
1. Check compiler and library versions
2. Verify hardware connections
3. Review system logs
4. Create GitHub issue with error details

## Future Enhancements

- [ ] Integration with TensorFlow Lite C++
- [ ] Real-time signal processing with FFTW
- [ ] WebSocket server for real-time monitoring
- [ ] Multi-threading for concurrent sensor reading
- [ ] Custom PCB design for sensor interface
