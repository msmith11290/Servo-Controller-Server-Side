#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <bluetooth/bluetooth.h>
#include <bluetooth/rfcomm.h>
#include "servo.h"
#include "i2c.h"

int client, s;
char buf[1024] = { 0 };
extern int stepsize;
extern int s1CurrentPos, s2CurrentPos;  //currentPosition;
extern unsigned char i2cCmdBuff[10];

void init_bluetooth(void)
{
    struct sockaddr_rc loc_addr = { 0 }, rem_addr = { 0 };
    socklen_t opt = sizeof(rem_addr);

    // allocate socket
    s = socket(AF_BLUETOOTH, SOCK_STREAM, BTPROTO_RFCOMM);

    // bind socket to port 1 of the first available 
    // local bluetooth adapter
    loc_addr.rc_family = AF_BLUETOOTH;
    loc_addr.rc_bdaddr = *BDADDR_ANY;
    loc_addr.rc_channel = (uint8_t) 1;
    bind(s, (struct sockaddr *)&loc_addr, sizeof(loc_addr));

    // put socket into listening mode
    listen(s, 1);

    // accept one connection
    client = accept(s, (struct sockaddr *)&rem_addr, &opt);

    ba2str( &rem_addr.rc_bdaddr, buf );
    fprintf(stderr, "accepted connection from %s\n", buf);
    memset(buf, 0, sizeof(buf));
}
void get_bt_data(void)
{
    int bytes_read;
    // read data from the client
    buf[0]='z';
    while( buf[0] != 'q' )
    {
        bytes_read = read(client, buf, sizeof(buf[0]));
        if( bytes_read > 0 ) 
        {
            printf("received [%s]\n", buf);
        }
        switch( buf[0] )
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
            case 'f':
            case 'F':   freeze();
                        break;
            default :   i2cCmdBuff[0] = LED0;
                        s1CurrentPos = MID_RANGE;
                        i2cCmdBuff[3] = s1CurrentPos & 0xFF;
                        i2cCmdBuff[4] = (s1CurrentPos >> 8 ) & 0xFF;
                        write_i2c(5, i2cCmdBuff);
                        i2cCmdBuff[0] = LED1;
                        s2CurrentPos = MID_RANGE;
                        i2cCmdBuff[3] = s2CurrentPos & 0xFF;
                        i2cCmdBuff[4] = (s2CurrentPos >> 8 ) & 0xFF;
                        break;
        
        }
    }
    // If we quit out of the keystroke method do we need to inform the user. Maybe the server ???
    printf("A quit command was received, Bluetooth server is no longer funtional\n");
}
void close_bluetooth(void)
{
    // close connection
    close(client);
    close(s);
}


