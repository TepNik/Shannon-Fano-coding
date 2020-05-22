Shannon–Fano coding compressor
======================================

This is the implementation of Shannon–Fano coding. You can find information about this algorithm [here](https://en.wikipedia.org/wiki/Shannon%E2%80%93Fano_coding).

Also here you can find file for unit-testing in Visual Studio (CppUnitTestFramework).

# File Compression

## Compile program
        make

## Compress file
        ./sh-fano input_file

## Compress input string from console
        ./sh-fano
Then write this string. Output will be in type uint8_t that is casted to int for good looking.

## Flags

### Custom output file (by default it is input_file.sh-fano)
        -o output_file