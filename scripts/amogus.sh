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
    echo "Port $port found. Proceeding with commands..."
        cd ../RPS_Controller
#    # Build Controller
#        rm -rf build
#        source /home/rud/esp/esp-idf/export.sh
#        idf.py build
    # Flash the board
        source /home/rud/esp/esp-idf/export.sh
        esptool.py -p /dev/ttyACM0 -b 115200 --before default_reset --after hard_reset --chip esp32s3  write_flash --flash_mode dio --flash_size detect --flash_freq 40m 0x0 build/bootloader/bootloader.bin 0x8000 build/partition_table/partition-table.bin 0x10000 build/RPS_Controller.bin
    # Run tests
    cd ../RPS_GUI
    cmake -S . -B build-tests -DBUILD_TESTS=ON -DTEST_PORT="$port"
    cd build-tests/
    cmake --build .
    xvfb-run -a ctest --verbose
    # Upload tests result
    cat Testing/Temporary/LastTest.log
    mkdir -p ../../delpoy/GUI/test_results
    cp Testing/Temporary/LastTest.log ../../deploy/GUI/test_results
    break
  else
    echo "Port $port does not exist. Please enter a valid port."
  fi
done

