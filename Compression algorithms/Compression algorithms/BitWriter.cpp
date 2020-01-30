#include "BitWriter.h"

std::string to_binary(uint16_t a, size_t n)
{
	std::string result = "";
	for (size_t i = 0; i < n; i++)
	{
		if (a != 0)
		{
			if (a % 2 == 1)
				result.insert(result.begin(), '1');
			else
				result.insert(result.begin(), '0');
			a >>= 1;
		}
		else
			result.insert(result.begin(), '0');
	}

	return result;
}

void BitWriter::_check_last()
{
	if (current_bit >= 8)
	{
		//std::cout << "written(check_last) " << to_binary(current_byte, 8) << std::endl;
		write_byte(current_byte);
		current_bit -= 8;
		current_byte = 0;
	}
}

void BitWriter::write_bits(uint16_t num, uint8_t quantity)
{
	//std::cout << "before " << to_binary(current_byte, current_bit) << std::endl;
	//std::cout << "want to write " << to_binary(num, quantity) << std::endl;
	if (current_bit + quantity <= 8)
	{
		current_byte += (uint16_t) (uint16_t) (num << (16 - quantity)) >> (16 - quantity - current_bit);
		current_bit += quantity;
		_check_last();
	}
	else if (current_bit + quantity <= 16)
	{
		uint8_t read_to_first = 8 - current_bit;

		current_byte += (uint16_t) ((uint16_t) num << (8 + current_bit)) >> (16 - read_to_first - current_bit);
		//std::cout << "written " << to_binary(current_byte, 8) << std::endl;
		writer.put(current_byte);

		uint8_t read_to_second = quantity - read_to_first;
		current_bit = read_to_second;
		current_byte = (uint16_t) ((uint16_t) num << (16 - quantity)) >> (16 - read_to_second);
		_check_last();
	}
	else
	{
		uint8_t write_to_first = 8 - current_bit;
		current_byte += (uint16_t) ((uint16_t) num << (16 - write_to_first)) 
			>> (16 - write_to_first - current_bit);
		writer.put(current_byte);

		current_byte = (uint16_t) ((uint16_t) num << (8 - write_to_first)) >> 8;
		writer.put(current_byte);

		current_bit = quantity - 8 - write_to_first;
		current_byte = (uint16_t) ((uint16_t) num << (16 - quantity)) >> (16 - current_bit);

		_check_last();
	}
	//std::cout << "result " << to_binary(current_byte, current_bit) << std::endl << std::endl;
}

void BitWriter::write_byte(uint8_t num)
{
	writer.put(num);
}

void BitWriter::finish()
{
	if(current_bit != 0)
		write_byte(current_byte);
}
