#!/bin/bash

# Find all ttyACM devices with Espressif manufacturer
echo "Scanning for Espressif devices..."
found_devices=0
for tty in /dev/ttyACM*; do
  if [ -e "$tty" ]; then
    # Check the manufacturer using udevadm
    manufacturer=$(udevadm info --query=all --name="$tty" | grep "ID_VENDOR=" | awk -F'=' '{print $2}')
    if [[ "$manufacturer" =~ "Espressif" ]]; then
      echo "Device: $tty -> Manufacturer: $manufacturer"
      found_devices=1
    fi
  fi
done

if [[ $found_devices -eq 0 ]]; then
  echo "No Espressif devices found."
  exit 1
fi

# Prompt user for port selection
while true; do
  read -p "Enter port to connect (/dev/PORTNAME): " port
  if [[ -e "$port" ]]; then
    break
  else
    echo "Port $port does not exist. Please enter a valid port."
  fi
done

# Prompt user for baud rate with default value
read -p "Enter baud rate [115200]: " baud_rate
baud_rate=${baud_rate:-115200}

echo "Using port: $port with baud rate: $baud_rate"

# Get the root directory of the project
project_root=$(pwd)/..

# Create deploy directory structure if it doesn't exist
deploy_dir="${project_root}/deploy/test_results"
mkdir -p "$deploy_dir"

cd ../RPS_Controller || exit 1

# Build Controller
rm -rf build
source /home/rud/esp/esp-idf/export.sh
idf.py build

# Flash the board
source /home/rud/esp/esp-idf/export.sh
esptool.py -p "$port" -b "$baud_rate" \
  --before default_reset \
  --after hard_reset \
  --chip esp32s3 \
  write_flash \
  --flash_mode dio \
  --flash_size detect \
  --flash_freq 40m \
  0x0 build/bootloader/bootloader.bin \
  0x8000 build/partition_table/partition-table.bin \
  0x10000 build/RPS_Controller.bin

# Run tests
cd ../RPS_GUI || exit 1
cmake -S . -B build-tests -DBUILD_TESTS=ON -DTEST_PORT="$port"
cd build-tests/ || exit 1
cmake --build .
xvfb-run -a ctest --verbose

# Save test results with timestamp
timestamp=$(date +"%Y%m%d_%H%M%S")
test_result_file="test_results_${timestamp}.log"

# Copy test results to deploy directory
if [ -f "Testing/Temporary/LastTest.log" ]; then
    cp "Testing/Temporary/LastTest.log" "${deploy_dir}/${test_result_file}"
    echo "Test results saved to: ${deploy_dir}/${test_result_file}"
else
    echo "Warning: Test log file not found at Testing/Temporary/LastTest.log"
fi
