#include "utilities.h"


struct BASE_64 base_64_table[] = {
	{'A', 0},
	{'B', 1},
	{'C', 2},
	{'D', 3},
	{'E', 4},
	{'F', 5},
	{'G', 6},
	{'H', 7},
	{'I', 8},
	{'J', 9},
	{'K', 10},
	{'L', 11},
	{'M', 12},
	{'N', 13},
	{'O', 14},
	{'P', 15},
	{'Q', 16},
	{'R', 17},
	{'S', 18},
	{'T', 19},
	{'U', 20},
	{'V', 21},
	{'W', 22},
	{'X', 23},
	{'Y', 24},
	{'Z', 25},
	{'a', 26},
	{'b', 27},
	{'c', 28},
	{'d', 29},
	{'e', 30},
	{'f', 31},
	{'g', 32},
	{'h', 33},
	{'i', 34},
	{'j', 35},
	{'k', 36},
	{'l', 37},
	{'m', 38},
	{'n', 39},
	{'o', 40},
	{'o', 41},
	{'q', 42},
	{'r', 43},
	{'s', 44},
	{'t', 45},
	{'u', 46},
	{'v', 47},
	{'w', 48},
	{'x', 49},
	{'y', 50},
	{'z', 51},
	{'0', 52},
	{'1', 53},
	{'2', 54},
	{'3', 55},
	{'4', 56},
	{'5', 57},
	{'6', 58},
	{'7', 59},
	{'8', 60},
	{'9', 61},
	{'+', 62},
	{'/', 63}
};




int amount_of_missions(char* data)
{
	int i = 0, counter = 1/*there's at least one mission*/;

	for (i = 0; i < strlen(data); i++)
	{
		if (data[i] == ',')
			counter++;
	}

	return counter;
}


// This function returns the data allocated in te heap from the http message.
char* get_data_from_http_msg(char* server_response, int server_response_len)
{
	DWORD i = 0; // int
	unsigned int data_len = 0;
	char* temp_msg_ptr = server_response;
	char* data = NULL;

	for (i = 0; i < server_response_len; i++)
	{
		// if founded the end of the headers
		if (strncmp(temp_msg_ptr, END_OF_HEADERS, strlen(END_OF_HEADERS)) == 0)
		{

			temp_msg_ptr += strlen(END_OF_HEADERS); // go to the start of the data
			i += strlen(END_OF_HEADERS); // go to the start of the data -> helping get the data size

			data_len = server_response_len - i; // the data len

			data = (char*)malloc(data_len + 1); // with the null byte

			data[data_len] = '\0'; //adding the null byte

			strncpy(data, temp_msg_ptr, data_len);
		
		}

		temp_msg_ptr++; // increasing the response msg to one ahead -> if the end of the headers is not found
	}

	printf("data = %s \n", data);

	return data;
}


// This function copies data from one ptr to other.
int copy_data(char* destination, char* source, int destination_size)
{

	if (destination_size < sizeof(source))
	{
		printf("Problem int copy data function! \n");
		return -1;
	}

	strncpy(destination, source, destination_size);

	return 0;
}


// This function encodes file to base 64 file.
int encode_to_base_64(unsigned char* file_to_encode, int file_size, unsigned char* file_destination, int file_destination_size)
{
	int i = 0, j = 0, six_bits_to_encode = 0, index_to_write = 0;
	//unsigned char* temp_file_content = (char*)malloc(file_size);
	
	unsigned char* temp_file = (unsigned char*)malloc(file_size);
	unsigned int temp_num = 0;
	unsigned int three_bytes = 0;
	unsigned int farthest_bits = 0, right_middle_bits = 0, left_middle_bits = 0, closest_to_start_bits = 0;
	BYTE arr_of_three_bytes[3] = { 0 };


	memcpy(temp_file, file_to_encode, file_size);

	
	// getting every time 3 bytes -> 4 base 64 chars -> 24 bits, 3 bytes
	if (file_size % 3 != 0)
	{
		printf("Problem in the size of the file size / sizeof(struct THREE_BYTES_SIZE) (3 bytes size)! \n");
		return -1;
	}
	
	
	// loop that starts from the end of the file -> getting 3 bytes (4 base64 chars) and using their 6 bits for the encoding.
	for (i = 0; i <= file_size - 3; i += 3)
	{
		temp_num = *(int*)&temp_file[i]; // 4 bytes


		temp_num = temp_num & 0xffffff/*0b111111111111111111111111*/; // getting the three bytes needed from the int (4 bytes)
		
		
		// making the LSBytes change with the MSByte due to indianess changes.
		for (j = sizeof(arr_of_three_bytes) - 1; j >= 0; j--)
		{
			arr_of_three_bytes[j] = temp_num & 255;
			temp_num >>= 8;
		}


		memcpy(&three_bytes, arr_of_three_bytes, sizeof(arr_of_three_bytes));
		

		farthest_bits = GET_SIX_BITS(three_bytes); // first 6 bits
		

		three_bytes >>= 6; // shifting the 6 bits taken


		right_middle_bits = GET_SIX_BITS(three_bytes);
		

		three_bytes >>= 6; // shifting the 6 bits taken
		

		left_middle_bits = GET_SIX_BITS(three_bytes); // first 6 bits
		

		three_bytes >>= 6; // shifting the 6 bits taken
		
		

		closest_to_start_bits = GET_SIX_BITS(three_bytes);


		three_bytes >>= 6; // shifting the 6 bits taken

		// good implementation down there

		// looking for the last / the farthest from the start byte value in the base 64 array
		for (j = 0; j < sizeof(base_64_table) / sizeof(struct BASE_64); j++)
		{
			if (closest_to_start_bits == base_64_table[j].decimal_val)
			{
				file_destination[index_to_write] = base_64_table[j].char_val;
				index_to_write++;
			}
		}

		// looking for the middle byte from byte value in the base 64 array
		for (j = 0; j < sizeof(base_64_table) / sizeof(struct BASE_64); j++)
		{
			if (left_middle_bits == base_64_table[j].decimal_val)
			{
				file_destination[index_to_write] = base_64_table[j].char_val;
				//printf("right middle: %#.2x -> %c \n", left_middle_bits, base_64_table[j].char_val);
				index_to_write++;
			}
		}

		// looking for the middle byte from byte value in the base 64 array
		for (j = 0; j < sizeof(base_64_table) / sizeof(struct BASE_64); j++)
		{
			if (right_middle_bits == base_64_table[j].decimal_val)
			{
				file_destination[index_to_write] = base_64_table[j].char_val;
				//printf("left middle: %#.2x -> %c \n", right_middle_bits, base_64_table[j].char_val);
				index_to_write++;
			}
		}

		// looking for the closest byte from the start byte value in the base 64 array
		for (j = 0; j < sizeof(base_64_table) / sizeof(struct BASE_64); j++)
		{
			if (farthest_bits == base_64_table[j].decimal_val)
			{
				file_destination[index_to_write] = base_64_table[j].char_val;
				//printf("closest to start: %#.2x -> %c \n", farthest_bits, base_64_table[j].char_val);
				index_to_write++;
			}
		}
	}

	file_destination[file_destination_size - 1] = '\0'; // adding the null byte in the end of the string

	
	free(temp_file);
	
	return 0;
}