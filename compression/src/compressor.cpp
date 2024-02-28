#include "compression/compressor.hpp"

#include <iostream>
#include <fstream>
#include <lz4.h>

enum {
    MESSAGE_MAX_BYTES   = (1024 * 512), // currently set to 512 KB, tune this to improve performance
    RING_BUFFER_BYTES   = MESSAGE_MAX_BYTES * 8 + MESSAGE_MAX_BYTES,
};

void compressFile(const char* inputFileName, const char* outputFileName)
{
    uint64_t remainingFileSize = 0; // tune the data type as per the file size

    // Open input file
    std::ifstream inputFile(inputFileName, std::ios::binary);
    if (!inputFile) {
        std::cerr << "Error opening input file." << std::endl;
        return;
    } else {
        inputFile.seekg(0 , std::ios::end);
        remainingFileSize = inputFile.tellg();
        inputFile.clear();
        inputFile.seekg(std::ios::beg);
    }

    // Open output file
    std::ofstream outputFile(outputFileName, std::ios::binary);
    if (!outputFile) {
        std::cerr << "Error opening output file." << std::endl;
        return;
    }

    std::cout << "Started performing compression of file :: " << inputFileName << std::endl;

    // Set up LZ4 compression context
    LZ4_stream_t* const lz4Stream = LZ4_createStream();

    // compressed bytes
    uint32_t compressedBytes = 0;

    // file offset
    uint64_t inputFileOffset = 0;
    uint64_t outputFileOffset = 0;

    // input block size
    const int sourceBufferSize = RING_BUFFER_BYTES;
    // compressed buffer size
    const int destinationBufferSize = LZ4_COMPRESSBOUND(MESSAGE_MAX_BYTES);

    // declare buffers
    char* inputBuffer = new char[sourceBufferSize];
    char* compressedBuffer = new char[destinationBufferSize];

    // end of data
    uint32_t endOfData = 0;

    while(true)
    {
        char* inputBufferPtr = &inputBuffer[inputFileOffset];

        if(remainingFileSize <= 0)
        {
            break;
        }

        // read data from input file in input buffer
        inputFile.read(inputBufferPtr, MESSAGE_MAX_BYTES);

        // compress data present in src buffer
        compressedBytes = LZ4_compress_fast_continue(lz4Stream , inputBufferPtr, compressedBuffer, inputFile.gcount(), destinationBufferSize, 1);
    
        // write number of bytes compressed in output file
        outputFile.write(reinterpret_cast<char*>(&compressedBytes), sizeof(compressedBytes));

        // write compressed data in the output file
        outputFile.write(compressedBuffer, compressedBytes);

        // increment input file offset
        inputFileOffset = inputFileOffset + MESSAGE_MAX_BYTES;

        // decrement remaining number of bytes to process in file
        remainingFileSize = remainingFileSize - inputFile.gcount();

        // wraparound the ring buffer offset
        if(inputFileOffset >= RING_BUFFER_BYTES - MESSAGE_MAX_BYTES)
        {
            inputFileOffset = 0;
        }
    }

    // write end of file frame
    outputFile.write(reinterpret_cast<char*>(&endOfData), sizeof(endOfData)); // to do : remove this magic number with variables

    std::cout << "Compression is successful. Output written to: " << outputFileName << std::endl;

    // Clean up
    delete[] inputBuffer;
    delete[] compressedBuffer;
    inputFile.close();
    outputFile.close();
}


// OPL ::
// 1 : How much byte we can send for compression at a time ?
// 2 : what should be the maximum buffer size for the compression buffer
// 3 : How many bytes from previous compression are needed to be stored in buffer for next compression to happen successfully



/**
 * Note : 
 * 
 * 1. Maximum input data in bytes which can be compressed is :: LZ4_MAX_INPUT_SIZE (0x7E000000 or 2113929216 bytes)
 * 
 * 2. Maximum size of compressed data in worst case can be calculated using -
 *    - LZ4_COMPRESSBOUND(input_size)
 *    - or, LZ4_compressBound(input_size)
 *    whereas, input_size is the size of the input buffer send for compressesion.
 * 
 *    Example : 
 *    if, number of bytes to be compressesed is 1024.
 *    then, in worst case the size of compressed data will be LZ4_COMPRESSBOUND(1024) = 1044 Bytes
 * 
 * 3. The previous 64KB of "source data" must be present unmodified at the same address in memory
*/

/**
 *  Tunable variables for performance enhancement : 
 * 
 *  1. MESSAGE_MAX_BYTES.
 *  2. inputFileLength.
 *  3. remainingFileSize
 */