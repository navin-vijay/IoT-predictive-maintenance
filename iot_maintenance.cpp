#include <iostream>
#include <fstream>
#include <vector>
#include <random>
#include <chrono>
#include <thread>
#include <sqlite3.h>
#include <wiringPi.h>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <algorithm>
#include <cmath>

// Configuration constants
const int VIBRATION_SENSOR_PIN = 0;  // WiringPi pin 0 = GPIO 17
const int TEMP_SENSOR_PIN = 1;       // WiringPi pin 1 = GPIO 18
const int READING_INTERVAL = 60;     // seconds
const int DATA_COLLECTION_BATCH = 10;
const double ANOMALY_THRESHOLD = -0.5;

class IoTPredictiveMaintenance {
private:
    sqlite3* db;
    std::vector<std::pair<double, double>> dataPoints;
    std::mt19937 gen;
    std::uniform_real_distribution<double> vibrationDist;
    std::uniform_real_distribution<double> tempDist;

public:
    IoTPredictiveMaintenance() : gen(std::chrono::steady_clock::now().time_since_epoch().count()),
                                vibrationDist(0.5, 5.0), tempDist(20.0, 80.0) {
        initializeGPIO();
        initializeDatabase();
    }

    ~IoTPredictiveMaintenance() {
        if (db) {
            sqlite3_close(db);
        }
    }

    void initializeGPIO() {
        if (wiringPiSetup() == -1) {
            std::cerr << "Failed to initialize WiringPi" << std::endl;
            exit(1);
        }
        pinMode(VIBRATION_SENSOR_PIN, INPUT);
        pinMode(TEMP_SENSOR_PIN, INPUT);
        std::cout << "GPIO initialized successfully" << std::endl;
    }

    void initializeDatabase() {
        int rc = sqlite3_open("machine_health.db", &db);
        if (rc) {
            std::cerr << "Can't open database: " << sqlite3_errmsg(db) << std::endl;
            exit(1);
        }

        const char* sql = "CREATE TABLE IF NOT EXISTS machine_data ("
                         "id INTEGER PRIMARY KEY AUTOINCREMENT,"
                         "timestamp TEXT NOT NULL,"
                         "vibration REAL NOT NULL,"
                         "temperature REAL NOT NULL,"
                         "anomaly_score REAL NOT NULL);";

        char* errMsg = 0;
        rc = sqlite3_exec(db, sql, 0, 0, &errMsg);
        if (rc != SQLITE_OK) {
            std::cerr << "SQL error: " << errMsg << std::endl;
            sqlite3_free(errMsg);
            exit(1);
        }
        std::cout << "Database initialized successfully" << std::endl;
    }

    std::pair<double, double> readSensors() {
        // For real sensors, replace this with actual sensor reading code
        // Example for DHT22: you'd use a DHT library here
        double vibration = vibrationDist(gen);
        double temperature = tempDist(gen);
        
        // Uncomment below for real sensor readings
        // int vibrationReading = digitalRead(VIBRATION_SENSOR_PIN);
        // vibration = mapVibrationReading(vibrationReading);
        
        return std::make_pair(vibration, temperature);
    }

    std::string getCurrentTimestamp() {
        auto now = std::chrono::system_clock::now();
        auto time_t = std::chrono::system_clock::to_time_t(now);
        std::stringstream ss;
        ss << std::put_time(std::localtime(&time_t), "%Y-%m-%d %H:%M:%S");
        return ss.str();
    }

    double calculateMean(const std::vector<double>& data) {
        double sum = 0.0;
        for (double value : data) {
            sum += value;
        }
        return sum / data.size();
    }

    double calculateStdDev(const std::vector<double>& data, double mean) {
        double sum = 0.0;
        for (double value : data) {
            sum += (value - mean) * (value - mean);
        }
        return std::sqrt(sum / data.size());
    }

    double simpleAnomalyScore(double vibration, double temperature) {
        // Simple anomaly detection based on z-score
        // In a real implementation, you'd use more sophisticated algorithms
        
        if (dataPoints.size() < 5) {
            return 0.0; // Not enough data for anomaly detection
        }

        // Extract recent vibration and temperature values
        std::vector<double> recentVibrations, recentTemperatures;
        int startIdx = std::max(0, (int)dataPoints.size() - 10);
        
        for (int i = startIdx; i < dataPoints.size(); i++) {
            recentVibrations.push_back(dataPoints[i].first);
            recentTemperatures.push_back(dataPoints[i].second);
        }

        double vibMean = calculateMean(recentVibrations);
        double tempMean = calculateMean(recentTemperatures);
        double vibStd = calculateStdDev(recentVibrations, vibMean);
        double tempStd = calculateStdDev(recentTemperatures, tempMean);

        // Calculate z-scores
        double vibZScore = (vibStd > 0) ? (vibration - vibMean) / vibStd : 0;
        double tempZScore = (tempStd > 0) ? (temperature - tempMean) / tempStd : 0;

        // Combined anomaly score
        double anomalyScore = (vibZScore * vibZScore + tempZScore * tempZScore) / 2.0;
        
        // Return negative score if anomaly detected (following sklearn convention)
        return (anomalyScore > 4.0) ? -1.0 : anomalyScore;
    }

    void storeData(double vibration, double temperature, double anomalyScore) {
        const char* sql = "INSERT INTO machine_data (timestamp, vibration, temperature, anomaly_score) VALUES (?, ?, ?, ?);";
        sqlite3_stmt* stmt;

        int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
        if (rc != SQLITE_OK) {
            std::cerr << "Failed to prepare statement: " << sqlite3_errmsg(db) << std::endl;
            return;
        }

        std::string timestamp = getCurrentTimestamp();
        sqlite3_bind_text(stmt, 1, timestamp.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_double(stmt, 2, vibration);
        sqlite3_bind_double(stmt, 3, temperature);
        sqlite3_bind_double(stmt, 4, anomalyScore);

        rc = sqlite3_step(stmt);
        if (rc != SQLITE_DONE) {
            std::cerr << "Failed to execute statement: " << sqlite3_errmsg(db) << std::endl;
        } else {
            std::cout << "Data stored: " << timestamp 
                     << ", Vibration: " << vibration 
                     << ", Temperature: " << temperature 
                     << ", Anomaly: " << anomalyScore << std::endl;
        }

        sqlite3_finalize(stmt);
    }

    void exportToCSV() {
        const char* sql = "SELECT timestamp, vibration, temperature, anomaly_score FROM machine_data ORDER BY timestamp;";
        sqlite3_stmt* stmt;

        int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
        if (rc != SQLITE_OK) {
            std::cerr << "Failed to prepare statement: " << sqlite3_errmsg(db) << std::endl;
            return;
        }

        std::ofstream csvFile("machine_data.csv");
        if (!csvFile.is_open()) {
            std::cerr << "Failed to open CSV file for writing" << std::endl;
            return;
        }

        // Write header
        csvFile << "timestamp,vibration,temperature,anomaly_score" << std::endl;

        // Write data
        while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
            const char* timestamp = (const char*)sqlite3_column_text(stmt, 0);
            double vibration = sqlite3_column_double(stmt, 1);
            double temperature = sqlite3_column_double(stmt, 2);
            double anomalyScore = sqlite3_column_double(stmt, 3);

            csvFile << timestamp << "," << vibration << "," << temperature << "," << anomalyScore << std::endl;
        }

        csvFile.close();
        sqlite3_finalize(stmt);
        std::cout << "Data exported to machine_data.csv" << std::endl;
    }

    void run() {
        std::cout << "Starting IoT Predictive Maintenance System..." << std::endl;
        int readingCount = 0;

        try {
            while (true) {
                // Read sensor data
                auto sensorData = readSensors();
                double vibration = sensorData.first;
                double temperature = sensorData.second;

                // Add to data points
                dataPoints.push_back(sensorData);

                // Perform anomaly detection
                double anomalyScore = simpleAnomalyScore(vibration, temperature);

                // Store data
                storeData(vibration, temperature, anomalyScore);

                // Check for maintenance alerts
                if (anomalyScore < ANOMALY_THRESHOLD) {
                    std::cout << "⚠️  MAINTENANCE ALERT: Potential issue detected!" << std::endl;
                    std::cout << "   Vibration: " << vibration << " mm/s" << std::endl;
                    std::cout << "   Temperature: " << temperature << " °C" << std::endl;
                }

                readingCount++;

                // Export to CSV every 10 readings
                if (readingCount % DATA_COLLECTION_BATCH == 0) {
                    exportToCSV();
                }

                // Keep only recent data points to prevent memory issues
                if (dataPoints.size() > 100) {
                    dataPoints.erase(dataPoints.begin(), dataPoints.begin() + 50);
                }

                // Wait for next reading
                std::this_thread::sleep_for(std::chrono::seconds(READING_INTERVAL));
            }
        }
        catch (const std::exception& e) {
            std::cerr << "Error: " << e.what() << std::endl;
        }
    }
};

int main() {
    std::cout << "IoT Predictive Maintenance System (C++)" << std::endl;
    std::cout << "Press Ctrl+C to stop" << std::endl;

    IoTPredictiveMaintenance system;
    system.run();

    return 0;
}
