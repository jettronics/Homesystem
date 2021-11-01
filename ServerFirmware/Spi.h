#ifndef _SPI_H
#define _SPI_H

#include "Types.h"

void Spi_SlaveInit( byte bySpiMode );
byte Spi_Put( byte byVal );
byte Spi_Get( void );

#endif
