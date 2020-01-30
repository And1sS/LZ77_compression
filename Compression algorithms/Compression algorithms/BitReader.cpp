#include <iostream>
#include "BitReader.h"

BitReader::BitReader(std::istream& stream) : reader(stream)
{
    current_byte = reader.get();
}

bool BitReader::read_bit()
{
    bool result = current_byte & (0x1 << current_bit);
    current_bit++;
    if (current_bit == 8)
    {
        current_bit = 0;
        current_byte = reader.get();
    }

    return result;
}

uint16_t BitReader::read_bits(uint8_t quantity)
{
    uint16_t result = 0;

    if (quantity > 24)
        throw "illegal argument";

    if (current_bit + quantity < 8)
    {
        result = (uint16_t) (current_byte << (16 - quantity - current_bit)) >> (16 - quantity);
        current_bit += quantity;
    }
    else if (current_bit + quantity < 16)
    {
        uint16_t first_part = (uint16_t) current_byte >> current_bit;

        current_byte = reader.get();
        uint16_t second_part = (uint16_t) ((uint32_t) current_byte << (24 - quantity - current_bit))
            >> (16 - quantity);
        result = first_part + second_part;
        current_bit = quantity + current_bit - 8;
    }
    else
    {
        uint16_t first_part = (uint16_t) current_byte >> current_bit;

        current_byte = reader.get();
        uint16_t second_part = (uint16_t) current_byte << (8 - current_bit);

        current_byte = reader.get();
        uint16_t third_part = (uint16_t) (current_byte << (32 - quantity - current_bit))
            >> (16 - quantity);

        result = first_part + second_part + third_part;
        current_bit += quantity - 16;
    }
    

    return result;
}
