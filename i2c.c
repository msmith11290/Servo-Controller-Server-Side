/* ioctl() plaguarized from https://elinux.org/Interfacing_with_I2C_Devices */
#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "servo.h"
#include "i2c.h"

int file;
unsigned char read_buff[10];
static unsigned char i2cCmdBuff[10];   // i2cCmdBuff[0] = desired register
int steps;

/******************************************************************************
 				 write_i2c() 
Write requires a file handle, a buffer in which the data is stored, and the 
number of bytes to write. Write will attempt to write the number of bytes 
specified and will return the actual number of bytes written, which can be used 
to detect errors. It is assumed that i2cCmdBuff has all of the data preloaded.
******************************************************************************/
int write_i2c(int numBytes, unsigned char *buff)
{
 if (write(file, buff, numBytes) != numBytes)   
   {
    /* ERROR HANDLING: i2c transaction failed */
    printf("Failed to write to the i2c bus. Error = %s \n", strerror(errno));
    return( -1 );
    }
 return 1 ;
}
/******************************************************************************
 Read requires a file handle, a buffer to store the data, and a number of bytes 
to read. Read will attempt to read the number of bytes specified and will return 
the actual number of bytes read, which can be used to detect errors. 
Afterwards, read_buff contains the data and we return the number of bytes read
******************************************************************************/
int read_i2c(unsigned char reg, int numBytes)
{
int returnVal;

    i2cCmdBuff[0] = reg;       // register we want to read
    write_i2c(1, i2cCmdBuff);             // Set device internal pointer
    // Using I2C Read
    if (read(file,read_buff,numBytes) != numBytes) 
    {
        /* ERROR HANDLING: i2c transaction failed */
        printf("Failed to read from the i2c bus. Error : %s \n",strerror(errno));
        returnVal = -1;
    } 
    else 
        returnVal = numBytes;
 return returnVal;
}
/******************************************************************************
		 Clear a bit(s) in in a register 
******************************************************************************/
void clearBit( char regNum, unsigned char bitPattern)
{
 read_i2c(regNum, 1);			// Read one byte
 i2cCmdBuff[1] = read_buff[0] & ~bitPattern; 
 write_i2c(2, i2cCmdBuff);		    // write the modified value back to the same register
 }
/******************************************************************************
		 Set a bit(s) in in a register 
******************************************************************************/
void setBit( char regNum, unsigned char bitPattern)
{
 read_i2c(regNum, 1);			// Read one byte
 i2cCmdBuff[1] = read_buff[0] | bitPattern; 
 write_i2c(2, i2cCmdBuff);		            // write the modified value back to the same register
}
/******************************************************************************
                Prescale()
* calculate and set the prescale
* The formula is
* Master Clock (25MHz) / max prescale (4096) / desired frequency (50Hz) minus 1
* Perfom the calculation and write it to the prescale register
* Note: The device must be in sleep mode to change the prescaler
50Hz =20mSec divided by 4096 = 4.8828125uSec/step
******************************************************************************/
void setPrescale( int freq )
{
    setBit( MODE1, SLEEP);
    i2cCmdBuff[0] = PRESCALE;
    i2cCmdBuff[1] = (int)(((25000000.0 / 4096.0)/freq) - 1);
    write_i2c( 2, i2cCmdBuff );
    clearBit( MODE1, SLEEP );
}
/******************************************************************************
 				Main function 
******************************************************************************/
int init_i2c(void)
{
/* open the bus for reading and writing like you would any file. A call to open 
 * must be used rather than fopen so that writes to the bus are not buffered */
if ((file = open("/dev/i2c-1", O_RDWR)) < 0) 
   {
    /* ERROR HANDLING: you can check errno to see what went wrong */
    perror("Failed to open the i2c bus");
    exit(1);
    }

/*  initiate communication with i2c address 0x40 ( the servo controller board ) */
if (ioctl(file, I2C_SLAVE, DEVICE_ADDR) < 0) 
    {
    printf("Failed to acquire bus access and/or talk to slave.\n");
    /* ERROR HANDLING; you can check errno to see what went wrong */
    close(file);
    exit(1);
    }
    
    // Set the device for auto incrementing thru the registers
setBit(MODE1,AUTO_INCR);
setPrescale( 50 );
return(1);
}
void close_i2c(void)
{
    // Close up shop and go home
close(file);
}

/******************************************************************************
 * This test starts with a narrow pulse and increases it to max width then 
 * decreases it to minimum width. Over and over and over (forever)
 * The spec says 1.0 <-> 2.0 mSec for 180 degrees. I found using a scope it 
 * took 0.7 <-> 2.7 mSec.
 * I don't know if this is a spec error or if it varies from servo to servo. 
 * Chinese shit, so you never know ! +/- 30% ???
 * 5 VDC didn't drop during test() execution.
 * ***************************************************************************/
void test(void)
{
 int i, steps;
 //One_mSec    = 4096 * 0.03;
 //Two_mSec = 4096 * 0.13  - 1;
 //midRange = (Two_mSec - One_mSec) / 2 + One_mSec
 steps = ONE_MSEC / 10;
 printf("1 mSec = %lf \n", ONE_MSEC );
 printf("2 mSec = %lf\n", TWO_MSEC );
 printf("mid Range = %lf \n", MID_RANGE );
 steps = ONE_MSEC / 10;
 while(1)
 {   
    printf("Entering positive loop \n");
    for( i = ONE_MSEC; i < TWO_MSEC; i+=steps )
    {
      i2cCmdBuff[0] = LED0;
      i2cCmdBuff[1] = 0;
      i2cCmdBuff[2] = 0;      // No reason for a delay before going high
      i2cCmdBuff[3] = i & 0xFF;
      i2cCmdBuff[4] = (i >> 8 ) & 0xFF;
      write_i2c(5, i2cCmdBuff);
      usleep(10000);
    } 
    printf("Entering negative loop \n");

    for( i = TWO_MSEC; i > ONE_MSEC; i-=steps )
    {
     i2cCmdBuff[0] = LED0;
      i2cCmdBuff[1] = 0;
      i2cCmdBuff[2] = 0;      // No reason for a delay before going high
      i2cCmdBuff[3] = i & 0xFF;
      i2cCmdBuff[4] = (i >> 8 ) & 0xFF;
      write_i2c(5, i2cCmdBuff);
      usleep(10000);
    }
  }
} 