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
deploy_test_results_dir="${project_root}/deploy/test_results"
rm -rf "${deploy_test_results_dir}" && mkdir -p "${deploy_test_results_dir}"

cd "${project_root}/RPS_Controller" || exit 1

# Build Controller
rm -rf build
source /home/rud/esp/esp-idf/export.sh
idf.py build

 Flash the board
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
source "${project_root}/scripts/TestsCI.sh"
python3 "${project_root}/scripts/TestAndCov_Report.py"

 
