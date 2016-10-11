/*
* SPI Generic
* Nathan Reichenberger
*
* Handles Generic SPI Master communication
*
*/

#include "SPI_Generic.h"

#include <Arduino.h>
#include "stdint.h"

/******************************************************
File Variables
******************************************************/

/******************************************************
Function Prototypes
******************************************************/
uint8_t transfer_byte( uint8_t byte_out );

/******************************************************
Constructor Functions
******************************************************/
/* Constructor */
SPI_Generic_Master_t::SPI_Generic_Master_t
    (
    void
    )
{
} /* SPI_Generic_Master_t::SPI_Generic_Master_t() */

/* Deconstructor */
SPI_Generic_Master_t::~SPI_Generic_Master_t
    (
    void
    )
{
} /* SPI_Generic_Master_t::~SPI_Generic_Master_t */

/******************************************************
Public Functions
******************************************************/
/* Settup everyting */
void SPI_Generic_Master_t::begin
    (
    void
    )
{
uint8_t clear;

/* Configure SPI */
pinMode( MOSI, OUTPUT );
pinMode( MISO, INPUT );
pinMode( SCK, OUTPUT );

/* Enable SPI and configure as master */
SPCR = ( 1 << SPE ) | ( 1 << MSTR );

clear = SPSR;
clear = SPDR;
} /* SPI_Generic_Master_t::begin() */

/* Shutdown everything */
void SPI_Generic_Master_t::end
    (
    void
    )
{
uint8_t clear;

/* Disable SPI */
SPCR = 0;

clear = SPSR;
clear = SPDR;

/* Configure SPI */
pinMode( MOSI, INPUT );
pinMode( MISO, INPUT );
pinMode( SCK, INPUT );
} /* SPI_Generic_Master_t::end() */
        
/* Read data from slave. Send only 0's */
bool SPI_Generic_Master_t::transfer_buffer_r
    (
    uint8_t * buffer_in,
    uint8_t transfer_len
    )
{
for( int pos = 0; pos < transfer_len; pos ++ )
    {
    buffer_in[ pos ] = transfer_byte( 0x00 );
    }
} /* SPI_Generic_Master_t::transfer_buffer_r() */

/* Send provided buffer but do not get a resonce */
bool SPI_Generic_Master_t::transfer_buffer_w
    (
    uint8_t *   buffer_out,
    uint8_t     transfer_len
    )
{
for( int pos = 0; pos < transfer_len; pos ++ )
    {
    transfer_byte( buffer_out[ pos ] );
    }
} /* SPI_Generic_Master_t::transfer_buffer_w() */

/* Send buffer and return responce in provided buffer */
bool SPI_Generic_Master_t::transfer_buffer_rw
    (
    uint8_t *   buffer_in_out,
    uint8_t     transfer_len
    )
{    
for( int pos = 0; pos < transfer_len; pos ++ )
    {
    buffer_in_out[ pos ] = transfer_byte( buffer_in_out[ pos ] );
    }
} /* SPI_Generic_Master_t::transfer_buffer_rw() */


/******************************************************
Private Functions
******************************************************/

/******************************************************
Local Functions
******************************************************/
uint8_t transfer_byte
    (
    uint8_t byte_out
    )
{
/* load transfer register */
SPDR = byte_out;

/* Wait for transfer to finish */
while( !(SPSR & ( 1 << SPIF ) ) )
    {
    }
return SPDR;
} /* transfer_byte() */
