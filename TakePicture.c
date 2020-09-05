#include "TakePicture.h"



void SaveBitmapToFile(BYTE* pBitmapBits, LONG lWidth, LONG lHeight, WORD wBitsPerPixel, const unsigned long padding_size, LPCTSTR lpszFileName)
{
	unsigned long headers_size = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
	unsigned long pixel_data_size = lHeight * ((lWidth * (wBitsPerPixel / 8)) + padding_size);
	BITMAPINFOHEADER bmpInfoHeader = { 0 };
	BITMAPFILEHEADER bfh = { 0 };

	bmpInfoHeader.biSize = sizeof(BITMAPINFOHEADER);
	bmpInfoHeader.biBitCount = wBitsPerPixel;
	bmpInfoHeader.biClrImportant = 0;
	bmpInfoHeader.biClrUsed = 0;
	bmpInfoHeader.biCompression = BI_RGB; 
	bmpInfoHeader.biHeight = lHeight;
	bmpInfoHeader.biWidth = lWidth;
	bmpInfoHeader.biPlanes = 1;
	bmpInfoHeader.biSizeImage = pixel_data_size;


	bfh.bfType = 0x4D42;
	bfh.bfOffBits = headers_size;
	bfh.bfSize = headers_size + pixel_data_size;

	HANDLE hFile = CreateFile(lpszFileName,
		GENERIC_WRITE,
		0,
		NULL,
		CREATE_ALWAYS,
		FILE_ATTRIBUTE_NORMAL,
		NULL);

	if (!hFile) return;
	DWORD dwWritten = 0;

	WriteFile(hFile,
		&bfh,
		sizeof(bfh),
		&dwWritten,
		NULL);

	WriteFile(hFile,
		&bmpInfoHeader,
		sizeof(bmpInfoHeader),
		&dwWritten,
		NULL);

	WriteFile(hFile,
		pBitmapBits,
		bmpInfoHeader.biSizeImage,
		&dwWritten,
		NULL);

	CloseHandle(hFile);
}

void intPxlArrToBytePxlArr(int* arr, int size, BYTE* arr2)
{
	// working!!!!!
	int c = 0;
	for (int i = size - 1; i >= 0; i--)
	{
		arr2[c] = (BYTE)(arr[i] & 0xff);
		arr2[c + 1] = (BYTE)(arr[i] >> 8 & 0xff);
		arr2[c + 2] = (BYTE)(arr[i] >> 16 & 0xff);
		c += 3;
	}


}

int TakeWebCapture()
{
	// working!!!!!
	struct SimpleCapParams capture;
	int devices = 0;
	int* targetBuffer = (int*)malloc(WIDTH * HEIGHT * PIXEL_SIZE * sizeof(int));
	BYTE* pixelBuffer = (BYTE*)malloc(WIDTH * HEIGHT * PIXEL_SIZE);
	/////////
	
	//int* targetBuffer = (int*)malloc(WIDTH * HEIGHT * PIXEL_SIZE);
	//BYTE* pixelBuffer = (BYTE*)malloc(WIDTH * HEIGHT * PIXEL_SIZE * sizeof(int));

	devices = setupESCAPI();

	if (!devices)
	{
		perror("No found devices!\n");
		return -1;
	}

	capture.mWidth = WIDTH;
	capture.mHeight = HEIGHT;
	capture.mTargetBuf = targetBuffer;

	if (initCapture(0, &capture) == 0)
	{
		perror("Capture failed - device may already be in use.\n");
		return -1;
	}

	printf("Got %d devices\n", devices);

	doCapture(0);
	while (isCaptureDone(0) == 0);

	intPxlArrToBytePxlArr(capture.mTargetBuf, capture.mWidth * capture.mHeight, pixelBuffer);
	deinitCapture(1);

	SaveBitmapToFile(pixelBuffer, capture.mWidth, capture.mHeight, 24, 0, L"picture.bmp");

	return 0;
}

int TakePicture()
{
	//char ptr[] = "mycommander1234.herokuapp.com";
	int res = TakeWebCapture();
	return res;
}