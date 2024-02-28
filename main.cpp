#include "compression/compressor.hpp"
#include "compressionHC/compressorHC.hpp"

#include <iostream>
#include <fstream>
#include <lz4.h>
#include <chrono>

using namespace std;
using namespace std::chrono;

int main()
{
    const char* inputFileName = "apricot-image-ui-adelegg.ext4";   
    const char* outputFileName = "apricot-image-ui-adelegg.ext4.lz4";
    const char* outputFileNameHC = "apricot-image-ui-adelegg.ext4.lz4hc";

    std::chrono::_V2::system_clock::time_point start, stop;

    start = high_resolution_clock::now();
    compressFile(inputFileName, outputFileName);
    stop = high_resolution_clock::now();

    std::cout << "Time taken to compress :: " << inputFileName << " with lz4 is :: " << duration_cast<minutes>(stop - start).count() << " minutes" << std::endl;

    start = high_resolution_clock::now();
    compressFileHC(inputFileName, outputFileNameHC);
    stop = high_resolution_clock::now();

    std::cout << "Time taken to compress :: " << inputFileName << " with lz4hc is :: " << duration_cast<minutes>(stop - start).count() << " minutes" << std::endl;

    return 0;
}