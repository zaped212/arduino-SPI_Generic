/*
* SPI Generic
* Nathan Reichenberger
*
* Generic SPI class. Acts as slave and/or master
*
*/

#ifndef __SPI_GENERIC_H__
#define  __SPI_GENERIC_H__

#include <Arduino.h>
#include "stdint.h"

/* Defines */
#ifndef CNT_OF_ARRAY
    #define CNT_OF_ARRAY( x )       ( sizeof( x )/sizeof( x[0] ) )
#endif

/* Global Variables */

/* function types */
typedef void (*slave_send_data_func_t)( void * buffer, uint8_t buffer_len );
typedef void (*slave_recv_data_func_t)( void * buffer, uint8_t buffer_len );

/* Common Types */

class SPI_Generic_Master_t
    {
    public:
        /* constructors */
        SPI_Generic_Master_t( void );
        ~SPI_Generic_Master_t( void );
        
        /* functions */
        void begin( void );
        void end( void );
        
        bool transfer_buffer_r( uint8_t * buffer_in, uint8_t transfer_len );
        bool transfer_buffer_w( uint8_t * buffer_out, uint8_t transfer_len );
        bool transfer_buffer_rw( uint8_t * buffer_in_out, uint8_t transfer_len );
    private:
        /* functions */

        /* friend functions */

        /* variables */
    };

class SPI_Generic_Slave_t
    {
    public:
        /* constructors */
        SPI_Generic_Slave_t( uint8_t data_len, uint8_t ss_int_pin, slave_send_data_func_t send_data_func, slave_recv_data_func_t recv_data_func );
        ~SPI_Generic_Slave_t( void );
        
        /* functions */
        
    private:
        /* functions */
        void settup_interrupts( void );             /* Configure Interrupts */
        void release_interrupts( void );            /* Release Interrupts */

        /* freinds */
        friend void spi_ss_irq( void );             /* Interrupt for Slave select pin */
        friend void spi_data_transfer_irq( void );  /* Master sent a command to the slave */        

        /* variables */
        uint8_t                 ss_pin;             /* Interrupt pin used for Slave Select, Should be tied to SS -1 to disable */

        char *                  buff;               /* Buffer used to send and recieve data [Header][UserData]*/
        uint8_t                 buff_len;           /* Full length of the buffer */
        uint8_t                 buff_send_index;    /* Index used to step through buffer */
        uint8_t                 buff_recv_index;    /* Index used to step through buffer */

        slave_send_data_func_t  send_data_f;        /* Callback used to get the data to be sent */
        slave_recv_data_func_t  recv_data_f;        /* Callback used to provide the application with the received data ( Data in buffer when transaction finished ) */
    };

#endif
