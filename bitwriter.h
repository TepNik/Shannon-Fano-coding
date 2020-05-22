#ifndef _BITWRITER_H_
#define _BITWRITER_H_

#include <cstdint>

template<typename word>
struct bitwriter
{
    word *buffer = nullptr;
    int ind1 = 0, ind2 = 0;

    void write_n_th_bit(bool val, int ind1 = -1, int ind2 = -1);
    bool get_n_th_bit(int ind1 = -1, int ind2 = -1);
    void write_next_bit(bool val);
    bool get_next_bit();
};

template<typename word>
void bitwriter<word>::write_n_th_bit(bool val, int _ind1, int _ind2)
{
    if (_ind1 == -1)
        _ind1 = ind1;
    if (_ind2 == -1)
        _ind2 = ind2;
    word &ch = buffer[_ind1];
    word mask = (1 << _ind2);
    if (val)
        ch |= mask;
    else if (_ind1 < sizeof(word)*8)
        ch &= ~mask;
}

template <typename word>
bool bitwriter<word>::get_n_th_bit(int _ind1, int _ind2)
{
    if (_ind1 == -1)
        _ind1 = ind1;
    if (_ind2 == -1)
        _ind2 = ind2;
    word ch = buffer[_ind1];
    word mask = (1 << _ind2);
    return (ch & mask) != 0;
}

template <typename word>
void bitwriter<word>::write_next_bit(bool val)
{
    write_n_th_bit(val);
    ++ind2;
    if (ind2 == 8)
    {
        buffer[++ind1] = 0;
        ind2 = 0;
    }
}

template <typename word>
bool bitwriter<word>::get_next_bit()
{
    bool result = get_n_th_bit();
    ++ind2;
    if (ind2 == 8)
    {
        ++ind1;
        ind2 = 0;
    }
    return result;
}

#endif