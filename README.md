# IoT-Based Predictive Maintenance System

## Overview
This project implements an IoT-based predictive maintenance system for manufacturing machinery. It monitors real-time machine health (vibration and temperature) using a Raspberry Pi, processes data with Python, stores it in SQLite, and visualizes maintenance alerts in Power BI. The system uses an Isolation Forest algorithm for anomaly detection to predict maintenance needs, reducing downtime.

## Features
- **Real-Time Monitoring**: Collects vibration and temperature data from sensors.
- **Anomaly Detection**: Uses Isolation Forest to identify potential maintenance issues.
- **Data Storage**: Stores sensor data in a SQLite database for historical analysis.
- **Visualization**: Exports data to CSV for Power BI dashboards showing trends and alerts.
- **Scalability**: Modular code structure for easy integration with additional sensors.

## Hardware Requirements
- Raspberry Pi (e.g., Raspberry Pi 4)
- Vibration sensor (e.g., SW-420)
- Temperature sensor (e.g., DHT22)
- Jumper wires and breadboard

## Software Requirements
- Python 3.8+
- Libraries: `numpy`, `pandas`, `scikit-learn`, `RPi.GPIO`, `sqlite3`
- Power BI Desktop (for visualization)
- SQLite database

## Setup Instructions
1. **Hardware Setup**:
   - Connect the vibration sensor to GPIO pin 17 and the temperature sensor to GPIO pin 18 on the Raspberry Pi.
   - Ensure sensors are powered (3.3V or 5V as per specifications).
2. **Software Installation**:
   - Install Python 3.8+ on the Raspberry Pi.
   - Install required libraries: `pip install numpy pandas scikit-learn RPi.GPIO`.
   - Install SQLite if not already present.
3. **Code Execution**:
   - Clone this repository: `git clone <repository_url>`.
   - Run the main script: `python predictive_maintenance.py`.
   - The script collects data every minute, stores it in `machine_health.db`, and exports to `machine_data.csv`.
4. **Power BI Setup**:
   - Import `machine_data.csv` into Power BI Desktop.
   - Create visualizations (e.g., line charts for vibration/temperature, alerts for anomalies).
   - Example dashboard: [Insert screenshot or description of dashboard].

## File Structure
- `predictive_maintenance.py`: Main Python script for data collection, anomaly detection, and storage.
- `machine_data.csv`: Exported data for Power BI visualization.
- `machine_health.db`: SQLite database storing sensor data.
- `README.md`: Project documentation.

## Usage
- Run `predictive_maintenance.py` on the Raspberry Pi to start monitoring.
- Check logs for real-time updates and warnings.
- Import `machine_data.csv` into Power BI to visualize trends and maintenance alerts.

## Example Output
- **Example Dashboard**:
- <img width="963" height="542" alt="Machine Data Dashboard" src="https://github.com/user-attachments/assets/ea5ba1ea-3ee6-4046-a5f2-330ad0531491" />

> ⚠️ **Disclaimer**  
> This repository uses **sample / modeled datasets** and **sample code** to demonstrate the concept of IoT-based predictive maintenance.  
> It does **not** contain the original proprietary data or source code developed during my work at V.T.V Vejay Traders.  
> The project is intended for **learning, demonstration, and portfolio purposes only**.

