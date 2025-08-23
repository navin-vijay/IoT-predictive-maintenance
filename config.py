# Configuration file for IoT Predictive Maintenance System

# GPIO Pin Configuration
VIBRATION_SENSOR_PIN = 17
TEMP_SENSOR_PIN = 18

# Database Configuration
DATABASE_NAME = 'machine_health.db'
CSV_EXPORT_NAME = 'machine_data.csv'

# Sensor Reading Configuration
READING_INTERVAL = 60  # seconds between readings
DATA_COLLECTION_BATCH = 10  # number of readings before anomaly detection

# Anomaly Detection Configuration
ANOMALY_CONTAMINATION = 0.1  # expected proportion of outliers
RANDOM_STATE = 42

# Sensor Simulation Ranges (for testing without actual sensors)
VIBRATION_MIN = 0.5  # mm/s
VIBRATION_MAX = 5.0  # mm/s
TEMP_MIN = 20.0      # °C
TEMP_MAX = 80.0      # °C

# Logging Configuration
LOG_LEVEL = 'INFO'
LOG_FORMAT = '%(asctime)s - %(levelname)s - %(message)s'
