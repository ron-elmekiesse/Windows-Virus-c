#pragma once
#include "utilities.h"
#include "escapi.h"

void SaveBitmapToFile(BYTE* pBitmapBits, LONG lWidth, LONG lHeight, WORD wBitsPerPixel, const unsigned long padding_size, LPCTSTR lpszFileName);


void intPxlArrToBytePxlArr(int* arr, int size, BYTE* arr2);


int TakeWebCapture();


int TakePicture();