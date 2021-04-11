#ifndef SPIMESSAGE_H
#define SPIMESSAGE_H

using namespace std;


class SpiMessage
{
   
public:
   //spi_device	0=CS0, 1=CS1
   SpiMessage(int device, unsigned char mode, unsigned char bits, unsigned int speed);
   virtual ~SpiMessage();

public:
   void start();
   void stop();
   int read( unsigned char *txdata, unsigned char *rxdata, int length, int leavecslow ); 
   
private:
   int spi_device;
   int spi_cs0_fd;				//file descriptor for the SPI device
   int spi_cs1_fd;				//file descriptor for the SPI device
   unsigned char spi_mode;
   unsigned char spi_bitsPerWord;
   unsigned int spi_speed;
};


#endif
