#include <stdio.h>
#include "i2c.h"
#include "servo.h"
#include "keyboard.h" 
#include "bluetooth_server.h" 

int currentPosition;   // A good starting point ???
int s1CurrentPos, s2CurrentPos;
int stepsize = 10;
unsigned char i2cCmdBuff[10];
int main(int argc, char *argv[])
{ 
        // Set up i2c Command Buffer. These 2 bytes will not change
    i2cCmdBuff[1] = 0;
    i2cCmdBuff[2] = 0;      // No reason for a delay before going high
    
    s1CurrentPos = MID_RANGE;   // A good starting point ???
    s2CurrentPos = MID_RANGE;
    stepsize = 10;

    printf("Executing %s \n", argv[0]); // gets rid of warning "argv not used"
    init_i2c();
    init_keyboard();
    init_bluetooth();
    printf("argc = %d \n", argc);
    if( argc == 1 )                 // Always argv[0] is the name of the program itself, argc = 1
        get_bt_data();
    process_keystrokes();
    restore_keyboard();
    close_bluetooth();
    return 1;
}