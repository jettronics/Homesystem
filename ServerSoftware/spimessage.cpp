
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <iostream>
#include <fcntl.h>				//Needed for SPI port
#include <sys/ioctl.h>			//Needed for SPI port
#include <linux/spi/spidev.h>	//Needed for SPI port
#include <unistd.h>			    //Needed for SPI port

#include "spimessage.h"


SpiMessage::SpiMessage(int device, unsigned char mode, unsigned char bits, unsigned int speed)
    : spi_device(device)
    , spi_mode(mode)
    , spi_bitsPerWord(bits)
    , spi_speed(speed)
{

}

SpiMessage::~SpiMessage()
{

}


void SpiMessage::start()
{
    int status_value = -1;
    int *spi_cs_fd;

    //----- SET SPI MODE -----
    //SPI_MODE_0 (0,0) 	CPOL = 0, CPHA = 0, Clock idle low, data is clocked in on rising edge, output data (change) on falling edge
    //SPI_MODE_1 (0,1) 	CPOL = 0, CPHA = 1, Clock idle low, data is clocked in on falling edge, output data (change) on rising edge
    //SPI_MODE_2 (1,0) 	CPOL = 1, CPHA = 0, Clock idle high, data is clocked in on falling edge, output data (change) on rising edge
    //SPI_MODE_3 (1,1) 	CPOL = 1, CPHA = 1, Clock idle high, data is clocked in on rising, edge output data (change) on falling edge
    //spi_mode = SPI_MODE_0;
    
    //----- SET BITS PER WORD -----
    //spi_bitsPerWord = 8;
    
    //----- SET SPI BUS SPEED -----
    //spi_speed = 1000000;		//1000000 = 1MHz (1uS per bit) 


    if (spi_device)
    {
        spi_cs_fd = &spi_cs1_fd;
    }
    else
    {
    	spi_cs_fd = &spi_cs0_fd;
    }

    if (spi_device)
    {
        *spi_cs_fd = open("/dev/spidev0.1", O_RDWR);
    }
    else
    {
    	*spi_cs_fd = open("/dev/spidev0.0", O_RDWR);
    }
    
    if (*spi_cs_fd < 0)
    {
        cout << "\nError - Could not open SPI device\n";
        return;
    }

    status_value = ioctl(*spi_cs_fd, SPI_IOC_WR_MODE, &spi_mode);
    if(status_value < 0)
    {
        cout << "Could not set SPIMode (WR)...ioctl fail\n";
        return;
    }

    status_value = ioctl(*spi_cs_fd, SPI_IOC_RD_MODE, &spi_mode);
    if(status_value < 0)
    {
        cout << "\nCould not set SPIMode (RD)...ioctl fail\n";
        return;
    }

    status_value = ioctl(*spi_cs_fd, SPI_IOC_WR_BITS_PER_WORD, &spi_bitsPerWord);
    if(status_value < 0)
    {
        cout << "\nCould not set SPI bitsPerWord (WR)...ioctl fail\n";
        return;
    }

    status_value = ioctl(*spi_cs_fd, SPI_IOC_RD_BITS_PER_WORD, &spi_bitsPerWord);
    if(status_value < 0)
    {   
        cout << "\nCould not set SPI bitsPerWord(RD)...ioctl fail\n";
        return;
    }

    status_value = ioctl(*spi_cs_fd, SPI_IOC_WR_MAX_SPEED_HZ, &spi_speed);
    if(status_value < 0)
    {
        cout << "\nCould not set SPI speed (WR)...ioctl fail\n";
        return;
    }

    status_value = ioctl(*spi_cs_fd, SPI_IOC_RD_MAX_SPEED_HZ, &spi_speed);
    if(status_value < 0)
    {
        cout << "\nCould not set SPI speed (RD)...ioctl fail\n";
        return;
    }
    cout.flush();
    
    return;
}

void SpiMessage::stop()
{
    int status_value = -1;
    int *spi_cs_fd;

    if (spi_device)
    {
    	spi_cs_fd = &spi_cs1_fd;
    }
    else
    {
        spi_cs_fd = &spi_cs0_fd;
    }

    status_value = close(*spi_cs_fd);
    if(status_value < 0)
    {
    	cout << "\nError - Could not close SPI device\n";
    	return;
    }
    cout.flush();
    
    return;
}

//SpiDevice		0 or 1
//TxData and RxData can be the same buffer (read of each byte occurs before write)
//Length		Max 511 (a C SPI limitation it seems)
//LeaveCsLow	1=Do not return CS high at end of transfer (you will be making a further call to transfer more data), 0=Set CS high when done
int SpiMessage::read(unsigned char *txdata, unsigned char *rxdata, int length, int leavecslow)
{
    struct spi_ioc_transfer spi;
	int i = 0;
	int retVal = -1;
	int spi_cs_fd;
    
    memset(&spi, 0, sizeof(spi));

	if (spi_device)
    {
		spi_cs_fd = spi_cs1_fd;
	}
    else
	{
        spi_cs_fd = spi_cs0_fd;
    }
    
	spi.tx_buf = (unsigned long)txdata;		//transmit from "data"
	spi.rx_buf = (unsigned long)rxdata;		//receive into "data"
	spi.len = length;
	spi.delay_usecs = 0;
	spi.speed_hz = spi_speed;
	spi.bits_per_word = spi_bitsPerWord;
	spi.cs_change = leavecslow;						//0=Set CS high after a transfer, 1=leave CS set low

	retVal = ioctl(spi_cs_fd, SPI_IOC_MESSAGE(1), &spi);

	if(retVal < 0)
	{
		cout << "\nError - Problem transmitting spi data..ioctl\n";
	}
    cout.flush();

	return retVal;
}
