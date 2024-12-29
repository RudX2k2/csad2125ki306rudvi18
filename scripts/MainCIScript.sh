#!/bin/bash

# Function to display usage
usage() {
  echo "Usage: $0 [-p <port>] [-b <baud_rate>]"
  echo "  -p <port>       Specify the COM port (e.g., /dev/ttyACM0)"
  echo "  -b <baud_rate>  Specify the baud rate (default: 115200)"
  exit 1
}

# Default baud rate
baud_rate=115200

# Parse command-line arguments
while getopts "p:b:h" opt; do
  case "$opt" in
    p) port="$OPTARG" ;;
    b) baud_rate="$OPTARG" ;;
    h) usage ;;
    *) usage ;;
  esac
done

# Find all ttyACM devices with Espressif manufacturer if no port specified
if [ -z "$port" ]; then
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
fi

# Prompt for baud rate if not set via parameter
if [ -z "$baud_rate" ]; then
  read -p "Enter baud rate [115200]: " baud_rate_input
  baud_rate=${baud_rate_input:-115200}
fi

# Display selected settings
echo "Using port: $port with baud rate: $baud_rate"

# Get the root directory of the project
project_root=$(pwd)/..

# Create deploy directory structure if it doesn't exist
deploy_dir="${project_root}/deploy/test_results"
mkdir -p "$deploy_dir"

cd ../RPS_Controller || exit 1

# Build Controller
#rm -rf build
#source /home/rud/esp/esp-idf/export.sh
#idf.py build

# Flash the board
#source /home/rud/esp/esp-idf/export.sh
#esptool.py -p "$port" -b "$baud_rate" \
#  --before default_reset \
#  --after hard_reset \
#  --chip esp32s3 \
#  write_flash \
#  --flash_mode dio \
#  --flash_size detect \
#  --flash_freq 40m \
#  0x0 build/bootloader/bootloader.bin \
#  0x8000 build/partition_table/partition-table.bin \
#  0x10000 build/RPS_Controller.bin

# Run tests
cd ../RPS_GUI || exit 1
cmake -S . -B build-tests -DBUILD_TESTS=ON -DCODE_COVERAGE=ON -DTEST_PORT="$port"
cd build-tests/ || exit 1
cmake --build .

# Run tests with xvfb
xvfb-run -a ctest --verbose

# Save test results with timestamp
timestamp=$(date +"%Y%m%d_%H%M%S")
test_result_file="test_results_${timestamp}.log"
coverage_dir="${deploy_dir}/coverage_${timestamp}"

# Generate coverage report
echo "Generating coverage report..."
lcov --capture --directory . --output-file coverage.info
lcov --remove coverage.info '/usr/*' --output-file coverage.info
lcov --remove coverage.info '*/tests/*' --output-file coverage.info
lcov --remove coverage.info '*/RPS_GUI/SimpleIni.h' --output-file coverage.info


# Generate HTML report
genhtml coverage.info --output-directory "$coverage_dir"

# Copy test results to deploy directory
if [ -f "Testing/Temporary/LastTest.log" ]; then
    cp "Testing/Temporary/LastTest.log" "${deploy_dir}/${test_result_file}"
    echo "Test results saved to: ${deploy_dir}/${test_result_file}"
else
    echo "Warning: Test log file not found at Testing/Temporary/LastTest.log"
fi

# Copy coverage info
cp coverage.info "${deploy_dir}/coverage_${timestamp}.info"

echo "Coverage report generated in: ${coverage_dir}"

# Launch coverage report in default browser if available
if command -v xdg-open >/dev/null 2>&1; then
    echo "Opening coverage report in browser..."
    xdg-open "${coverage_dir}/index.html"
elif command -v open >/dev/null 2>&1; then
    echo "Opening coverage report in browser..."
    open "${coverage_dir}/index.html"
else
    echo "Coverage report is available at: ${coverage_dir}/index.html"
fi
