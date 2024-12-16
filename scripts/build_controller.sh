    # Build Controller
#        rm -rf ../Deploy/Controller/results_of_build
#        mkdir -p ../Deploy/Controller/results_of_build
        cd ../RPS_Controller
        source /home/rud/esp/esp-idf/export.sh
#        cmake -S . -B build-tests -DBUILD_TESTS=ON
        idf.py fullclean
        idf.py -D BUILD_TESTS=ON build-tests

# idf.py merge-bin
        
