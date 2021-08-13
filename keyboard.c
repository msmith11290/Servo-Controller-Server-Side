/*  Initialize keyboard by putting it into curses mode. i.e. don't echo the 
    keystroke and no waiting for enter to be hit before being able to read 
    each keystroke
    Install : sudo apt-get install libncurses5-dev libncursesw5-dev*/
#include <stdio.h>
#include <termios.h>    //termios, TCSANOW, ECHO, ICANON
#include <unistd.h>     //STDIN_FILENO/*tcgetattr gets the parameters of the current terminal
#include "i2c.h"
#include "servo.h"
static struct termios oldt, newt;
extern int s1CurrentPos, s2CurrentPos;  //currentPosition;
extern int stepsize;
extern unsigned char i2cCmdBuff[10];
    
    void init_keyboard(void)
    {
        /* STDIN_FILENO will tell tcgetattr that it should write the settings
           of stdin to oldt*/
        tcgetattr( STDIN_FILENO, &oldt);
                    /*now the settings will be copied*/
        newt = oldt;

            /*ICANON normally takes care that one line at a time will be processed
            that means it will return if it sees a "\n" or an EOF or an EOL*/
        newt.c_lflag &= ~(ICANON | ECHO);          

          /*Those new settings will be set to STDIN
            TCSANOW tells tcsetattr to change attributes immediately. */
        tcsetattr( STDIN_FILENO, TCSANOW, &newt);
    }
void restore_keyboard(void)
{
    /*restore the old settings*/
    tcsetattr( STDIN_FILENO, TCSANOW, &oldt);
}
void process_keystrokes(void)
{
    int c;  
    while((c=getchar())!= 'e')  
    {
        if( c == 27)    // 27d = ESCAPE
        {
            c = getchar();
            if( c == '[')
                c = getchar();
        }
     //   putchar(c);                 
        printf("Keystroke = %c \n", c);
        switch( c )
        {
            case 'w' :
            case 'A' :  s1CurrentPos += stepsize;
            if( s1CurrentPos > MAX_VALUE)
                s1CurrentPos = MAX_VALUE;
                        i2cCmdBuff[0] = LED0;
                        i2cCmdBuff[3] = s1CurrentPos & 0xFF;
                        i2cCmdBuff[4] = (s1CurrentPos >> 8 ) & 0xFF;
                        write_i2c(5, i2cCmdBuff);
                        break;
            case 'z' :
            case 'C' :  s2CurrentPos += stepsize;
                        if( s2CurrentPos < MIN_VALUE)
                            s2CurrentPos = MIN_VALUE;
                        i2cCmdBuff[0] = LED1;
                        i2cCmdBuff[3] = s2CurrentPos & 0xFF;
                        i2cCmdBuff[4] = (s2CurrentPos >> 8 ) & 0xFF;
                        write_i2c(5, i2cCmdBuff);
                        break;
            case 'a' :
            case 'B' :  s1CurrentPos -= stepsize;
                        if( s1CurrentPos > MAX_VALUE)
                            s1CurrentPos = MAX_VALUE;
                        i2cCmdBuff[0] = LED0;
                        i2cCmdBuff[3] = s1CurrentPos & 0xFF;
                        i2cCmdBuff[4] = (s1CurrentPos >> 8 ) & 0xFF;
                        write_i2c(5, i2cCmdBuff);
                        break;
            case 's' :
            case 'D' :  s2CurrentPos -= stepsize;
                        if( s2CurrentPos < MIN_VALUE)
                            s2CurrentPos = MIN_VALUE;
                        i2cCmdBuff[0] = LED1;
                        i2cCmdBuff[3] = s2CurrentPos & 0xFF;
                        i2cCmdBuff[4] = (s2CurrentPos >> 8 ) & 0xFF;
                        write_i2c(5, i2cCmdBuff);
                        break;
            default :   
                        i2cCmdBuff[0] = LED0;
                        s1CurrentPos = MID_RANGE;
                        i2cCmdBuff[3] = s1CurrentPos & 0xFF;
                        i2cCmdBuff[4] = (s1CurrentPos >> 8 ) & 0xFF;
                        write_i2c(5, i2cCmdBuff);
                        i2cCmdBuff[0] = LED1;
                        s2CurrentPos = MID_RANGE;
                        i2cCmdBuff[3] = s2CurrentPos & 0xFF;
                        i2cCmdBuff[4] = (s2CurrentPos >> 8 ) & 0xFF;
                        write_i2c(5, i2cCmdBuff);
                        break;
        
        }
    }
}