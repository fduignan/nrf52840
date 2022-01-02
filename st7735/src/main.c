/*
 * Copyright (c) 2012-2014 Wind River Systems, Inc.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr.h>
#include <sys/printk.h>
#include <device.h>
#include <drivers/gpio.h>
#include <drivers/uart.h>
#include <stdio.h>
#include "display.h"
uint32_t prbs()
{
    // This is an unverified 31 bit PRBS generator
    // It should be maximum length but this has not been checked 
    static uint32_t shift_register=0xaa551199;
    unsigned long new_bit=0;
    static int busy=0; // need to prevent re-entrancy here  
    if (!busy)
    {
        busy=1;
        new_bit= ((shift_register & (1<<27))>>27) ^ ((shift_register & (1<<30))>>30);
        new_bit= ~new_bit;
        new_bit = new_bit & 1;
        shift_register=shift_register << 1;
        shift_register=shift_register | (new_bit);
        busy=0;
    }
    return shift_register & 0x7ffffff; // return 31 LSB's 
}
uint32_t random(uint32_t lower,uint32_t upper)
{
    return (prbs()%(upper-lower))+lower;
}

void main(void)
{
	
	int ret;
	int count=0;

	ret = display_begin();
	if (ret < 0)
	{
		printf("Error initializing display.  Error code = %d\r\n",ret);
		while(1);
	}
	else
	{
		printf("Display initialized successfully\r\n");
	}
	//while(1)
	{
		printf("init\r\n");
	//	ret = display_begin();
		//k_msleep(1000);

	}
	
	while(1)
	{
		printf("Hello World! %s\n", CONFIG_BOARD);
		//uart_tx(uart,"hello\r\n",7,0);
		//k_msleep(1000);
		
		display_drawRectangle(random(0,160),random(0,128),random(0,160),random(0,128),random(0,0xffff));                                
        display_fillRectangle(random(0,160),random(0,128),random(0,160),random(0,128),random(0,0xffff));                   
       // display_drawCircle(random(0,240),random(0,240),random(0,135),random(0,0xffff));
        //display_fillCircle(random(0,240),random(0,135),random(0,120),random(0,0xffff));
        count++;
        if (count >= 50) 
        {
            display_fillRectangle(0,0,SCREEN_WIDTH,SCREEN_HEIGHT,0);
                        display_print("Starting over",sizeof("Starting over")-1,20,60,0xffff,0);
            count = 0;
        }

		
		
	}
}
