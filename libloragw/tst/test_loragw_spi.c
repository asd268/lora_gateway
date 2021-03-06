/*
 / _____)             _              | |
( (____  _____ ____ _| |_ _____  ____| |__
 \____ \| ___ |    (_   _) ___ |/ ___)  _ \
 _____) ) ____| | | || |_| ____( (___| | | |
(______/|_____)_|_|_| \__)_____)\____)_| |_|
  (C)2013 Semtech-Cycleo

Description:
	Minimum test program for the loragw_spi 'library'
	Use logic analyser to check the results.

License: Revised BSD License, see LICENSE.TXT file include in the project
Maintainer: Sylvain Miermont
*/


/* -------------------------------------------------------------------------- */
/* --- DEPENDANCIES --------------------------------------------------------- */

#include <stdint.h>
#include <stdio.h>
#include <mraa.h>

#include "loragw_spi.h"


/* -------------------------------------------------------------------------- */
/* --- PRIVATE MACROS ------------------------------------------------------- */

#define ARRAY_SIZE(a) (sizeof(a) / sizeof((a)[0]))

/* -------------------------------------------------------------------------- */
/* --- PRIVATE CONSTANTS ---------------------------------------------------- */

#define BURST_TEST_SIZE 2500 /* >> LGW_BURST_CHUNK */
#define TIMING_REPEAT	1	 /* repeat transactions multiple times for timing characterisation */
#define GPIO_PIN	2

/* -------------------------------------------------------------------------- */
/* --- MAIN FUNCTION -------------------------------------------------------- */

int main()
{
	int i;
	void *spi_target = NULL;
	uint8_t data = 0;
	uint8_t dataout[BURST_TEST_SIZE];
	uint8_t datain[BURST_TEST_SIZE];
	mraa_gpio context rst_pin;
	
	
	for (i = 0; i < BURST_TEST_SIZE; ++i) {
		dataout[i] = 0x30 + (i % 10); /* ASCCI code for 0 -> 9 */
		datain[i] = 0x23; /* garbage data, to be overwritten by received data */
	}
	
	printf("Beginning of test for loragw_spi.c\n");
	lgw_spi_open(&spi_target);
	
	/* normal R/W test */
	for (i = 0; i < TIMING_REPEAT; ++i)
		lgw_spi_w(spi_target, 0xAA, 0x96);
	for (i = 0; i < TIMING_REPEAT; ++i)
		lgw_spi_r(spi_target, 0x55, &data);
	
	/* burst R/W test, small bursts << LGW_BURST_CHUNK */
	for (i = 0; i < TIMING_REPEAT; ++i)
		lgw_spi_wb(spi_target, 0x55, dataout, 16);
	for (i = 0; i < TIMING_REPEAT; ++i)
		lgw_spi_rb(spi_target, 0x55, datain, 16);
	
	/* burst R/W test, large bursts >> LGW_BURST_CHUNK */
	for (i = 0; i < TIMING_REPEAT; ++i)
		lgw_spi_wb(spi_target, 0x5A, dataout, ARRAY_SIZE(dataout));
	for (i = 0; i < TIMING_REPEAT; ++i)
		lgw_spi_rb(spi_target, 0x5A, datain, ARRAY_SIZE(datain));
	
	/* last read (blocking), just to be sure no to quit before the FTDI buffer is flushed */
	lgw_spi_r(spi_target, 0x01, &data);
	printf("data received (simple read): %d\n",data);

	lgw_spi_close(spi_target);
	printf("End of test for loragw_spi.c\n");
	
	printf("GPIO_2 TEST\n");
	mraa_init();		
	rst_pin = mraa_gpio_init(GPIO_PIN);
	mraa_gpio_dir(rst_pin,MRAA_GPIO_OUT);
	
	for(i=0;i<1000;i++){
		mraa_gpio_write(rst_pin,1);
		sleep(1);
		mraa_gpio_write(rst_pin,0);
		sleep(1);	
	}
	printf("GPIO_2 TEST END\n");
	return 0;
}

/* --- EOF ------------------------------------------------------------------ */
