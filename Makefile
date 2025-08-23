# Makefile for IoT Predictive Maintenance System

# Compiler settings
CXX = g++
CXXFLAGS = -std=c++11 -Wall -Wextra -O2
LIBS = -lwiringPi -lsqlite3 -lpthread

# Target executable
TARGET = iot_maintenance
SOURCE = iot_maintenance.cpp

# Default target
all: $(TARGET)

# Build the main executable
$(TARGET): $(SOURCE)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SOURCE) $(LIBS)

# Install dependencies (run with sudo make install-deps)
install-deps:
	@echo "Installing dependencies..."
	apt update
	apt install -y build-essential libsqlite3-dev wiringpi
	@echo "Dependencies installed successfully!"

# Clean build files
clean:
	rm -f $(TARGET)
	rm -f *.o

# Create directories and set permissions
setup:
	@echo "Setting up environment..."
	mkdir -p logs
	chmod +x $(TARGET)

# Run the program
run: $(TARGET)
	./$(TARGET)

# Debug build
debug: CXXFLAGS += -g -DDEBUG
debug: $(TARGET)

# Install the program system-wide
install: $(TARGET)
	cp $(TARGET) /usr/local/bin/
	@echo "Program installed to /usr/local/bin/"

# Uninstall the program
uninstall:
	rm -f /usr/local/bin/$(TARGET)
	@echo "Program removed from /usr/local/bin/"

# Help target
help:
	@echo "Available targets:"
	@echo "  all         - Build the program (default)"
	@echo "  install-deps- Install required dependencies (needs sudo)"
	@echo "  clean       - Remove build files"
	@echo "  run         - Build and run the program"
	@echo "  debug       - Build with debug symbols"
	@echo "  install     - Install program system-wide"
	@echo "  uninstall   - Remove program from system"
	@echo "  help        - Show this help message"

.PHONY: all clean install-deps setup run debug install uninstall help
