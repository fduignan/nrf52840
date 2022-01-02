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
void main(void)
{
	
	const struct device *gpio,*uart;
	gpio=device_get_binding("GPIO_0");
	if (gpio==NULL)
	{
		printk("Error acquiring gpio\n");
		while(1);
	}
	//uart=device_get_binding("UART_1");
	/*if (uart==NULL)
	{
		printk("Error acquiring uart\n");
		while(1);
	}*/
	gpio_pin_configure(gpio,10,GPIO_OUTPUT);
	while(1)
	{
		printf("Hello World! %s\n", CONFIG_BOARD);
		//uart_tx(uart,"hello\r\n",7,0);
		gpio_pin_set(gpio,10,1);
		k_msleep(100);
		gpio_pin_set(gpio,10,0);
		k_msleep(100);
	}
}
