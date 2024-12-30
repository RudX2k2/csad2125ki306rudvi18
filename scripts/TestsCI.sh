#!/bin/bash

# Get the root directory of the project
project_root=$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)

# Create deploy directory structure if it doesn't exist
deploy_dir="${project_root}/deploy/test_results"

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

