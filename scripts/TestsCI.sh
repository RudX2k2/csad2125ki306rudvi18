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

echo "Using port: $port with baud rate: $baud_rate"

# Get the root directory of the project
project_root=$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)

# Create deploy directory structure if it doesn't exist
deploy_dir="${project_root}/deploy/test_results"
mkdir -p "$deploy_dir"

# Run tests
cd "${project_root}/RPS_Controller" || { echo "Failed to enter RPS_Controller directory"; exit 1; }
cd "${project_root}/RPS_GUI" || { echo "Failed to enter RPS_GUI directory"; exit 1; }

cmake -S . -B build-tests -DBUILD_TESTS=ON -DCODE_COVERAGE=ON -DTEST_PORT="$port"
cmake --build build-tests || { echo "Build failed"; exit 1; }

cd build-tests || { echo "Failed to enter build-tests directory"; exit 1; }

# Run tests with xvfb
timestamp=$(date +"%Y%m%d_%H%M%S")
test_log="${deploy_dir}/test_results_${timestamp}.log"
xvfb-run -a ctest --output-on-failure | tee "$test_log"

# Generate test results in XML format for integration
xvfb-run -a ctest -T Test --output-on-failure
xml_test_results="${deploy_dir}/test_results_${timestamp}.xml"
if [ -f "Testing/Temporary/LastTest.log" ]; then
  cp "Testing/Temporary/LastTest.log" "$xml_test_results"
  echo "Test results saved to: $xml_test_results"
else
  echo "Test results file not found. Ensure tests are running properly."
fi

# Generate coverage report
echo "Generating coverage report..."
coverage_dir="${deploy_dir}/coverage_${timestamp}"
mkdir -p "$coverage_dir" # Ensure the directory exists

lcov --capture --directory . --output-file coverage.info || { echo "lcov capture failed"; exit 1; }
lcov --remove coverage.info '/usr/*' '*/tests/*' '*/RPS_GUI/SimpleIni.h' --output-file coverage.info || { echo "lcov remove failed"; exit 1; }

# Generate coverage report without test integration (no test-name-file option)
echo "Generating coverage report without test integration..."
genhtml --branch-coverage --legend --title "Coverage Report" --output-directory "$coverage_dir" coverage.info

echo "Coverage report generated in: $coverage_dir"

# Copy coverage info
cp coverage.info "${deploy_dir}/coverage_${timestamp}.info"

# Open coverage report in default browser
coverage_index="${coverage_dir}/index.html"
if [ -f "$coverage_index" ]; then
  if command -v xdg-open >/dev/null 2>&1; then
    echo "Opening coverage report in browser..."
    xdg-open "$coverage_index"
  elif command -v open >/dev/null 2>&1; then
    echo "Opening coverage report in browser..."
    open "$coverage_index"
  else
    echo "Coverage report is available at: $coverage_index"
  fi
else
  echo "Coverage index file not found. Check the report generation process."
fi

