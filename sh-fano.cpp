#include <iostream>
#include <cstring>
#include <string>
#include <sys/stat.h>
#include <algorithm>
#include <fstream>
#include <cassert>

#include "bitwriter.h"

struct pr_param
{
    bool compress = true,
         input_from_stdin = true;
    std::string input_file, output_file;
};
pr_param get_start_paramentrs(int argc, char *argv[]);
off_t get_size_of_file(const std::string &file_name)
{
    struct stat s;
    stat(file_name.c_str(), &s);
    return s.st_size;
}

struct symbol
{
    bitwriter<uint8_t> bw{new uint8_t, 0, 0};
    char ch = 0;
    int num = 0;
    bool operator==(char sec) {return ch == sec;}
    bool operator<(const symbol &sec) const {return num < sec.num;}
};
symbol symbol_arr[256];
void get_code(symbol *first, symbol *end, int sum, int value = -1);
bitwriter<uint8_t> get_compress(const char *input, int size, int &symbol_size);
char* get_decompress(uint8_t *input, int size, int &new_size);

int main(int argc, char *argv[])
{
    auto param = get_start_paramentrs(argc, argv);
    if (param.input_from_stdin)
    {
        std::string str;
        std::getline(std::cin, str);
        int symbol_size;
        bitwriter<uint8_t> output = get_compress(str.c_str(), str.size(), symbol_size);
        int limit = output.ind1 + (output.ind2 == 0 ? 0 : 1);
        std::cout << "ind1 = " << output.ind1 << '\n';
        std::cout << "ind2 = " << output.ind2 << '\n';
        std::cout << "Compressed (in type uint8_t):\n";
        for(int i = 0; i < limit; ++i)
            std::cout << (int)output.buffer[i] << " ";
        std::cout << '\n';
        std::cout << "Ratio in/out = "
                  << str.size() << '/' << limit << " = "
                  << str.size()*1.0/limit << '\n';
    }
    else
    {
        std::cout << "Input file: " << param.input_file << '\n';
        std::cout << "Output file: " << param.output_file << '\n';
        off_t input_size = get_size_of_file(param.input_file);
        char *input = new char[input_size];
        std::ifstream fin;
        fin.open(param.input_file, std::ios::binary);
        fin.read(input, input_size);
        if (param.compress)
        {
            int symbol_size = -1;
            bitwriter<uint8_t> output = get_compress(input, input_size, symbol_size);
            int limit = output.ind1 + (output.ind2 == 0 ? 0 : 1);
            std::ofstream fout;
            fout.open(param.output_file, std::ios::binary);
            fout.write((char*)&input_size, sizeof(input_size));
            fout.write((char*)&output.ind1, sizeof(output.ind1));
            fout.write((char*)&output.ind2, sizeof(output.ind2));
            fout.write((char*)&symbol_size, sizeof(symbol_size));
            for(int i = 0; i < symbol_size; ++i)
            {
                fout.write(&symbol_arr[i].ch, sizeof(symbol_arr[i].ch));
                fout.write((char*)&symbol_arr[i].num, sizeof(symbol_arr[i].num));
            }
            fout.write((char*)output.buffer, limit);
            std::cout << "Ratio in/out = "
                    << input_size << '/' << limit << " = "
                    << input_size*1.0/limit << '\n';
        }
        else
        {
            uint8_t *input_uint8 = (uint8_t*)input;
            int decompressed_size;
            char *decompressed = get_decompress(input_uint8, input_size, decompressed_size);
            std::ofstream fout (param.output_file);
            fout.close();
            fout.open(param.output_file, std::ios::binary);
            fout.write(decompressed, decompressed_size);
            std::cout << "Ratio in/out = "
                      << input_size << '/' << decompressed_size << " = "
                      << input_size*1.0/decompressed_size << '\n';
        }
    }
    return 0;
}

pr_param get_start_paramentrs(int argc, char *argv[])
{
    pr_param result;

    const char *variants[] = {
        "-o",
        "-d",
    };

    for(int i = 1; i < argc; ++i)
    {
        bool is_found = false;
        for (int j = 0; j < sizeof(variants)/sizeof(variants[0]) && !is_found; ++j)
        {
            if (strstr(argv[i], variants[j]) == argv[i])
            {
                if (j == 0)
                    result.output_file = argv[++i];
                else if (j == 1)
                    result.compress = false;
                is_found = true;
            }
        }
        if (!is_found && result.input_file.empty())
        {
            result.input_file = argv[i];
            result.input_from_stdin = false;
        }
        else if (!is_found && !result.input_file.empty() && argv[i][0] != '-')
        {
            std::cout << "\n\nError!\nThere must be only one input file.\n";
            exit(1);
        }
        else if (!is_found && !result.input_file.empty() && argv[i][0] == '-')
        {
            std::cout << "\nError!\nNo such command \"" << argv[i] << "\"\n";
            exit(1);
        }
    }

    std::string suff = ".sh-fano";
    if (result.input_from_stdin == false && result.compress && result.output_file.empty())
        result.output_file = result.input_file + suff;
    else if (result.input_from_stdin == false && !result.compress && result.output_file.empty())
    {
        result.output_file = result.input_file;
        auto ind = result.output_file.find(suff);
        if (ind != std::string::npos)
            result.output_file.erase(result.output_file.find(suff), suff.length());
        else
            result.output_file += suff + "_d";
    }

    return result;
}

void get_code(symbol *first, symbol *end, int sum, int value)
{
    if (first >= end)
        return;
    if (value == 0 || value == 1)
        for (symbol *i = first; i < end; ++i)
            i->bw.write_next_bit(value);
    if (first + 1 >= end)
        return;
    int sum_now = 0;
    int ind = 0;
    while(sum_now < sum/2)
    {
        sum_now += first[ind].num;
        ++ind;
    }
    if (ind == end - first)
        --ind;
    get_code(first, std::min(first + ind, end), sum_now, 0);
    get_code(first + ind, end, sum - sum_now, 1);
}

bitwriter<uint8_t> get_compress(const char *input, int input_size, int &symbol_size)
{
    if (input_size == 0)
        return bitwriter<uint8_t>();
    int size = 0, sum = 0;
    for(int i = 0; i < input_size; ++i)
    {
        symbol *ind = std::find(symbol_arr, symbol_arr + size, input[i]);
        if (ind == symbol_arr + size)
        {
            ++size;
            ind->ch = input[i];
            ind->num = 0;
        }
        ++ind->num;
        ++sum;
    }
    std::stable_sort(symbol_arr, symbol_arr + size);
    if (size == 1)
        get_code(symbol_arr, symbol_arr + size, sum, 0);
    else
        get_code(symbol_arr, symbol_arr + size, sum);
    bitwriter<uint8_t> result{new uint8_t[input_size], 0, 0};
    for(int i = 0; i < input_size; ++i)
    {
        symbol *sym = std::find(symbol_arr, symbol_arr + size, input[i]);
        bitwriter<uint8_t> bw_now{sym->bw.buffer, 0, 0};
        while(bw_now.ind1 < sym->bw.ind1 || bw_now.ind2 < sym->bw.ind2)
            result.write_next_bit(bw_now.get_next_bit());
    }
    symbol_size = size;
    return result;
}

char* get_decompress(uint8_t *input, int size, int &new_size)
{
    int ind_now = 0;

    off_t output_size = *(off_t*)&input[ind_now];
    ind_now += sizeof(off_t);
    int ind1 = *(int*)&input[ind_now];
    ind_now += sizeof(int);
    int ind2 = *(int*)&input[ind_now];
    ind_now += sizeof(int);

    int symbol_size = *(int*)&input[ind_now];
    ind_now += sizeof(int);

    char *output = new char[output_size];
    new_size = output_size;

    int sum = 0;
    for(int i = 0; i < symbol_size; ++i)
    {
        symbol_arr[i].ch = input[ind_now];
        ind_now += sizeof(symbol_arr[i].ch);
        std::cout << "ch = " << symbol_arr[i].ch << '\n';
        symbol_arr[i].num = *(int*)&input[ind_now];
        ind_now += sizeof(symbol_arr[i].num);
        sum += symbol_arr[i].num;
    }

    if (size == 1)
        get_code(symbol_arr, symbol_arr + symbol_size, sum, 0);
    else
        get_code(symbol_arr, symbol_arr + symbol_size, sum);

    int output_ind = 0;
    bitwriter<uint8_t> bw_input{input + ind_now, 0, 0};
    while(output_ind < output_size && (bw_input.ind1 < ind1 || bw_input.ind2 < ind2))
    {
        bool is_find = false;
        int symbol_ind = -1;
        int ind1_now = bw_input.ind1, ind2_now = bw_input.ind2;
        for(int i = 0; i < symbol_size && !is_find; ++i)
        {
            bw_input.ind1 = ind1_now;
            bw_input.ind2 = ind2_now;
            bitwriter<uint8_t> bw{symbol_arr[i].bw.buffer, 0, 0};
            bool is_equal = true;
            while(is_equal && (bw.ind1 < symbol_arr[i].bw.ind1 || bw.ind2 < symbol_arr[i].bw.ind2))
                if (bw.get_next_bit() != bw_input.get_next_bit())
                    is_equal = false;
            if(is_equal)
            {
                is_find = true;
                symbol_ind = i;
            }
        }
        assert(symbol_ind != -1);
        output[output_ind] = symbol_arr[symbol_ind].ch;
        ++output_ind;
    }
    return output;
}