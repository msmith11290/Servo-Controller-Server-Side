int write_i2c(int numBytes, unsigned char *buff);
int read_i2c(unsigned char reg, int numBytes);
void clearBit( char regNum, unsigned char bitPattern);
void setBit( char regNum, unsigned char bitPattern);
void setPrescale( int freq );
void test(void);
int init_i2c(void);
void close_i2c(void);

