#include "CppUnitTest.h"
#include "sh-fano.cpp"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace tests
{
    TEST_CLASS(tests){
    public :
        TEST_METHOD(test_compression)
        {
            char input[] = "Hello";
            int symbol_size;
            auto result = get_compress(input, sizeof(input) / sizeof(input[0]), symbol_size);
            Assert::IsTrue(symbol_size == 4);
            Assert::IsTrue(result.ind1 == 1);
            Assert::IsTrue(result.ind2 == 2);
            Assert::IsTrue(result.buffer[0] == (uint8_t)248);
            Assert::IsTrue(result.buffer[1] == (uint8_t)1);
        }
        TEST_METHOS(test_decompression)
        {
            char input[500];
            int input_ind = 0;
            *((off_t*)(input + input_ind)) = 5;
            input_ind += sizeof(off_t);
            *((int*)(input + input_ind)) = 1;
            input_ind += sizeof(int);
            *((int*)(input + input_ind)) = 2;
            input_ind += sizeof(int);
            *((int*)(input + input_ind)) = 4;
            input_ind += sizeof(int);
            *((char*)(input + input_ind)) = 'H';
            input_ind += sizeof(char);
            *((int*)(input + input_ind)) = 1;
            input_ind += sizeof(int);
            *((char*)(input + input_ind)) = 'e';
            input_ind += sizeof(char);
            *((int*)(input + input_ind)) = 1;
            input_ind += sizeof(int);
            *((char*)(input + input_ind)) = 'o';
            input_ind += sizeof(char);
            *((int*)(input + input_ind)) = 1;
            input_ind += sizeof(int);
            *((char*)(input + input_ind)) = 'l';
            input_ind += sizeof(char);
            *((int*)(input + input_ind)) = 2;
            input_ind += sizeof(int);
            *((uint8_t*)(input + input_ind)) = (uint8_t)248;
            input_ind += sizeof(uint8_t);
            *((uint8_t*)(input + input_ind)) = (uint8_t)1;
            input_ind += sizeof(uint8_t);
            int out_size;
            char *decompress = get_decompress(input, input_ind, out_size);
            decompress[out_size] = '\0';
            Assert::IsTrue(strcmp(decompress, "Hello") == 0)
        }
    };
}