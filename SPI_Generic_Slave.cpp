/*
* SPI Generic
* Nathan Reichenberger
*
* Handles Generic SPI Slave communication
*
*/

#include "SPI_Generic.h"

#include <Arduino.h>
#include "stdint.h"

/******************************************************
File Variables
******************************************************/
static SPI_Generic_Slave_t * spi_slave_ptr = NULL; /* To be set by begin function */

/******************************************************
Function Prototypes
******************************************************/
void spi_ss_irq( void );
void spi_data_transfer_irq( void );

/******************************************************
Constructor Functions
******************************************************/
/* Constructor */
SPI_Generic_Slave_t::SPI_Generic_Slave_t
    (
    uint8_t data_len,
    uint8_t ss_int_pin,
    slave_send_data_func_t send_data_func,
    slave_recv_data_func_t recv_data_func
    )
{
if( spi_slave_ptr )
    {
    /* Another Slave has already been settup */
    return;
    }

/* Assign global ptr for friendly functions */
spi_slave_ptr = this;

this->ss_pin = ss_int_pin;

/* Allocate Buffer space */
this->buff = (char*)malloc( data_len );
this->buff_len = data_len;   
memset( spi_slave_ptr->buff, 0x00, spi_slave_ptr->buff_len );

this->buff_send_index = 0;         
this->buff_recv_index = 0;

/* Configure callback functions */
this->send_data_f = send_data_func;
this->recv_data_f = recv_data_func;

/* Settup interrupts */
this->settup_interrupts();

return;
} /* SPI_Generic_Slave_t::SPI_Generic_Slave_t() */

/* Deconstructor */
SPI_Generic_Slave_t::~SPI_Generic_Slave_t
    (
    void
    )
{
if( spi_slave_ptr == this )
    {
    spi_slave_ptr = NULL;

    /* Release interrupts */
    this->release_interrupts();
    }

if( this->buff )
    {
    free( this->buff );
    }
this->buff = NULL;            
this->buff_len = 0;           
this->buff_send_index = 0;         
this->buff_recv_index = 0;         

this->send_data_f = NULL;    
this->recv_data_f = NULL; 
} /* SPI_Generic_Slave_t::~SPI_Generic_Slave_t */

/******************************************************
Public Functions
******************************************************/

/******************************************************
Private Functions
******************************************************/
/* Configure SS and Data transfer interrupts */
void SPI_Generic_Slave_t::settup_interrupts
    (
    void
    )
{
/* Configure SS Interrupt */
if( this->ss_pin != (uint8_t)-1 )
    {
    pinMode(this->ss_pin, INPUT);
    attachInterrupt( digitalPinToInterrupt(this->ss_pin), spi_ss_irq, CHANGE );
    }

/* Configure SPI data interrupt */
pinMode( MISO, OUTPUT );
SPCR |= _BV(SPE);   /* Configure slave mode */
SPCR |= _BV(SPIE);  /* Enable interrupts */
} /* SPI_Generic_Slave_t::settup_interrupts() */

/* Disable SS and Data transfer interrupts */
void SPI_Generic_Slave_t::release_interrupts
    (
    void
    )
{
/* Release SS Interrupt */
if( this->ss_pin != (uint8_t)-1 )
    {
    detachInterrupt( digitalPinToInterrupt(this->ss_pin) );
    }

/* Configure SPI data interrupt */
SPCR &= !_BV(SPIE);  /* Disable interrupts */
SPCR &= !_BV(SPE);   /* Disable slave mode */
pinMode( MISO, INPUT );
} /* SPI_Generic_Slave_t::release_interrupts() */

/******************************************************
Local Functions
******************************************************/
/* Slave Select Interrupt */
void spi_ss_irq
    (
    void
    )
{
static bool init_done = false;
static bool cur_state = false;

/* Switch the current state */
cur_state = !cur_state;

/* Check if initialization has been completed */
if( !init_done )
    {
    /* Read the first state, then just use "math" */
    cur_state = digitalRead( spi_slave_ptr->ss_pin );
    init_done = true;
    }

if( cur_state )
    {
    /* Slave select triggered */
    spi_slave_ptr->buff_recv_index = 0;
    spi_slave_ptr->buff_send_index = 0;

    /* Get buffer to send */
    if( spi_slave_ptr->send_data_f )
        {
        spi_slave_ptr->send_data_f( spi_slave_ptr->buff, spi_slave_ptr->buff_len );
        }

    /* Load first byte of transfer register */
    SPDR = spi_slave_ptr->buff[spi_slave_ptr->buff_send_index];
    spi_slave_ptr->buff_send_index = ( spi_slave_ptr->buff_send_index + 1 ) % spi_slave_ptr->buff_len;
    }
else
    {
    /* Slave select released give application received data */
    if( spi_slave_ptr->recv_data_f )
        {
        spi_slave_ptr->recv_data_f( spi_slave_ptr->buff, spi_slave_ptr->buff_len );
        }
    memset( spi_slave_ptr->buff, 0x00, spi_slave_ptr->buff_len );
    }
} /* spi_ss_irq() */

/* Data transfer Interrupt */
void spi_data_transfer_irq
    (
    void
    )
{
/* Read received data */
spi_slave_ptr->buff[spi_slave_ptr->buff_recv_index] = SPDR;
spi_slave_ptr->buff_recv_index = ( spi_slave_ptr->buff_recv_index + 1 ) % spi_slave_ptr->buff_len;

/* Load next byte of transfer register */
SPDR = spi_slave_ptr->buff[spi_slave_ptr->buff_send_index];
spi_slave_ptr->buff_send_index = ( spi_slave_ptr->buff_send_index + 1 ) % spi_slave_ptr->buff_len;
} /* spi_data_transfer_irq() */

/* SPI data interrupt vector ISR */
ISR(SPI_STC_vect)
{
spi_data_transfer_irq();
} /* ISR(SPI_STC_vect) */
