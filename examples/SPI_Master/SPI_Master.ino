#include "PeriodicEvent.h"
#include "SPI_Generic.h"

/* Constant PIN defines */
#define PIN_STATUS_LED          ( 13 )
#define PIN_SLAVE_SELECT        ( 10 )

/* Constant periodic event delays */
#define STATUS_DELAY_MS         ( 250 )
#define SPI_DELAY_MS            ( 250 )

/* Function definitions */
void Flash_LED( void * userdata );

/* Variables */
static PeriodicEvent        Flash_Status_LED( { 0, 0, 0, STATUS_DELAY_MS }, Flash_LED, (void*)PIN_STATUS_LED );
static PeriodicEvent        Transfer_SPI_test( { 0, 0, 0, SPI_DELAY_MS }, Transfer_SPI, (void*)PIN_SLAVE_SELECT );
static SPI_Generic_Master_t SPI_master;

/* Functions */
void setup()
    {
    /* Initialize periodic events */
    Flash_Status_LED.Enable();
    Transfer_SPI_test.Enable();

    SPI_master.begin();
    } /* setup() */

void loop()
    {
	/* Check periodic events */
    Flash_Status_LED.Check();
    Transfer_SPI_test.Check();
	
	/* Do something else here */

    } /* loop() */

void Flash_LED( void * userdata )
    {
    static bool Initialized = false;
    static bool State = HIGH;

    if( !Initialized )
        {
        Initialized = true;
        pinMode( (int)userdata, OUTPUT );
        }

    State = !State;
    digitalWrite( (int)userdata, State );
    } /* Flash_LED() */

void Transfer_SPI( void * userdata )
    {
    static bool Initialized = false;
    static uint8_t transfer_buffer[ 16 ];

    /* init slave select pin */
    if( !Initialized )
        {
        Initialized = true;
        pinMode( (int)userdata, OUTPUT );
        }
    
    for( int i = 1; i < sizeof( transfer_buffer ); i ++ )
        {
        transfer_buffer[ i ] = i;
        }
    /* Select Slave */
    digitalWrite( (int)userdata, HIGH );

    /* Test spi transfer functions */
    SPI_master.transfer_buffer_w( transfer_buffer, sizeof( transfer_buffer ) );
    SPI_master.transfer_buffer_r( transfer_buffer, sizeof( transfer_buffer ) );
    SPI_master.transfer_buffer_rw( transfer_buffer, sizeof( transfer_buffer ) );

    /* Release Slave */
    digitalWrite( (int)userdata, LOW );

    } /* Transfer_SPI() */