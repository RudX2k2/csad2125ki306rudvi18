    # Build test on controller
#        rm -rf ../Deploy/Controller/results_of_build
#        mkdir -p ../Deploy/Controller/results_of_build
        cd ../RPS_Controller
        source /home/rud/esp/esp-idf/export.sh
#        cmake -S . -B build-tests -DBUILD_TESTS=ON
        echo $PWD
        cd RPS_Controller
        idf.py fullclean
        idf.py -D BUILD_TESTS=ON build-tests

# idf.py merge-bin
# Set the components to include the tests for.
# This can be overridden from CMake cache:
# - when invoking CMake directly: cmake -D TEST_COMPONENTS="xxxxx" ..
# - when using idf.py: idf.py -T xxxxx build
#
        
