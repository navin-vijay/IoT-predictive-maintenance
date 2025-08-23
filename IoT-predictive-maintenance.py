import time
import random
import sqlite3
import pandas as pd
from datetime import datetime
import RPi.GPIO as GPIO
import numpy as np
from sklearn.ensemble import IsolationForest
import logging

# Setup logging
logging.basicConfig(level=logging.INFO, format='%(asctime)s - %(levelname)s - %(message)s')

# GPIO setup for Raspberry Pi
GPIO.setmode(GPIO.BCM)
VIBRATION_SENSOR_PIN = 17
TEMP_SENSOR_PIN = 18
GPIO.setup(VIBRATION_SENSOR_PIN, GPIO.IN)
GPIO.setup(TEMP_SENSOR_PIN, GPIO.IN)

# SQLite database setup
def init_db():
    conn = sqlite3.connect('machine_health.db')
    c = conn.cursor()
    c.execute('''CREATE TABLE IF NOT EXISTS machine_data (
                 timestamp TEXT,
                 vibration REAL,
                 temperature REAL,
                 anomaly_score REAL
                 )''')
    conn.commit()
    conn.close()

# Simulate sensor readings (replace with actual sensor library like Adafruit_DHT for real sensors)
def read_sensors():
    vibration = random.uniform(0.5, 5.0)  # Simulate vibration in mm/s
    temperature = random.uniform(20.0, 80.0)  # Simulate temperature in °C
    return vibration, temperature

# Predictive maintenance model using Isolation Forest
def detect_anomaly(data):
    model = IsolationForest(contamination=0.1, random_state=42)
    anomaly_scores = model.fit_predict(data)
    return anomaly_scores

# Store data in SQLite
def store_data(vibration, temperature, anomaly_score):
    conn = sqlite3.connect('machine_health.db')
    c = conn.cursor()
    timestamp = datetime.now().strftime('%Y-%m-%d %H:%M:%S')
    c.execute("INSERT INTO machine_data (timestamp, vibration, temperature, anomaly_score) VALUES (?, ?, ?, ?)",
              (timestamp, vibration, temperature, anomaly_score))
    conn.commit()
    conn.close()
    logging.info(f"Stored data: {timestamp}, Vibration: {vibration}, Temperature: {temperature}, Anomaly: {anomaly_score}")

# Export data to CSV for Power BI
def export_to_csv():
    conn = sqlite3.connect('machine_health.db')
    query = "SELECT * FROM machine_data"
    df = pd.read_sql_query(query, conn)
    conn.close()
    df.to_csv('machine_data.csv', index=False)
    logging.info("Data exported to machine_data.csv")

# Main function
def main():
    init_db()
    data_points = []
    
    try:
        while True:
            # Read sensor data
            vibration, temperature = read_sensors()
            data_points.append([vibration, temperature])
            
            # Perform anomaly detection when enough data is collected
            if len(data_points) >= 10:
                anomaly_scores = detect_anomaly(np.array(data_points[-10:]))
                anomaly_score = anomaly_scores[-1]
                
                # Store data
                store_data(vibration, temperature, anomaly_score)
                
                # Export to CSV every 10 readings
                if len(data_points) % 10 == 0:
                    export_to_csv()
                
                # Log maintenance alerts
                if anomaly_score == -1:
                    logging.warning("Potential maintenance issue detected!")
                
            time.sleep(60)  # Collect data every minute
            
    except KeyboardInterrupt:
        logging.info("Program terminated by user")
        GPIO.cleanup()
        export_to_csv()

if __name__ == "__main__":
    main()