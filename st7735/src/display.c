#include <sys/printk.h>
#include <device.h>
#include <drivers/gpio.h>
#include <drivers/spi.h>
#include <stdio.h>
#include "display.h"
#include "font5x7.h"
// Pinout : (see the app.overlay file for details)
// CS: 1.10
// Reset : 1.13
// A0 : 1.15
// SDA : 0.02
// SCK : 0.29

static const struct spi_config cfg = {
	.operation = SPI_WORD_SET(8) | SPI_TRANSFER_MSB |  SPI_MODE_CPOL | SPI_MODE_CPHA,
	.frequency = 8000000,
	.slave = 0,
};
static const struct device *spi_display;
static const struct device *gpio0, *gpio1;		

// The RESET_PORT_BIT is on GPIO1
#define RESET_PORT_BIT 13
// The DC_PORT_BIT is on GPIO1 (A0)
#define DC_PORT_BIT 15
// The CS_PORT_BIT is on GPIO1
#define CS_PORT_BIT 10

void ResetLow()
{
	gpio_pin_set(gpio1,RESET_PORT_BIT,0);
}
void ResetHigh()
{
	gpio_pin_set(gpio1,RESET_PORT_BIT,1);
}
void DCLow()
{
	gpio_pin_set(gpio1,DC_PORT_BIT,0);
}
void DCHigh()
{
	gpio_pin_set(gpio1,DC_PORT_BIT,1);
}
void CSLow()
{
	gpio_pin_set(gpio1,CS_PORT_BIT,0);
}
void CSHigh()
{
	gpio_pin_set(gpio1,CS_PORT_BIT,1);
}
void hw_test()
{
	// Test routine
	while(1)
	{
		//ResetLow();
		printf("low\r\n");
		DCLow();
		/*uint8_t data[] = {1,2,3,4};
		struct spi_buf tx_buf = {.buf = data, .len = 4};
		struct spi_buf_set tx_bufs = {.buffers = &tx_buf, .count = 1};   
		//spi_write(spi_display, &cfg, &tx_bufs);*/
		display_command(0xa5);
		display_data(0x17);
		k_msleep(10);
		//ResetHigh();
		printf("high\r\n");
		DCHigh();
		k_msleep(10);

	}
}

int display_begin()
{
	int ret;
	// Set up the SPI interface
	spi_display = device_get_binding("SPI_1");
	if (spi_display==NULL)
	{
		printf("Error acquiring SPI interface\n");
		return -1;
	}
	else
	{
		printf("Got SPI interface\n");
		if (!device_is_ready(spi_display))
		{
			printf("Device is not ready\n");
			return -2;
		}		
		
	}
	// Now configute the GPIO's for use as CS,DC and Reset
	// The MISO pin (not used) will be reassigned as Reset
	gpio0=device_get_binding("GPIO_0");
	if (gpio0 == NULL)
	{
		printf("Error acquiring GPIO 0 interface\n");
		return -3;
	}
	gpio1=device_get_binding("GPIO_1");
	if (gpio0 == NULL)
	{
		printf("Error acquiring GPIO 1 interface\n");
		return -4;
	}
	ret = gpio_pin_configure(gpio1,RESET_PORT_BIT,GPIO_OUTPUT);
	ret = gpio_pin_configure(gpio1,DC_PORT_BIT,GPIO_OUTPUT);
	ret = gpio_pin_configure(gpio1,CS_PORT_BIT,GPIO_OUTPUT);
	//hw_test();	
	// Lots of CS toggling here seems to have made the boot up more reliable
	CSHigh();
	ResetHigh();	
	k_msleep(10);
	ResetLow();
	k_msleep(200);
	ResetHigh();
	CSHigh();
	k_msleep(200);
	CSLow();
	k_msleep(20);
	display_command(0x01); // software reset
	k_msleep(100);
	CSHigh();
	k_msleep(1);
	CSLow();
	k_msleep(1);
	display_command(0x11); // exit sleep
	k_msleep(120);
	// set frame rate
	CSHigh();
	k_msleep(1);
	CSLow();
	k_msleep(1);
	display_command(0xb1);
	display_data(0x05);
	display_data(0x3c);
	display_data(0x3c);
	CSHigh();
	k_msleep(1);
	CSLow();
	k_msleep(1);
	display_command(0xb2);
	display_data(0x05);
	display_data(0x3c);
	display_data(0x3c);
	CSHigh();
	k_msleep(1);
	CSLow();
	k_msleep(1);
	display_command(0xb3);
	display_data(0x05);
	display_data(0x3c);
	display_data(0x3c);	
	display_data(0x05);
	display_data(0x3c);
	display_data(0x3c);
	CSHigh();
	k_msleep(1);
	CSLow();
	k_msleep(1);
	display_command(0xb4); // dot invert
	display_data(0x03);
	CSHigh();
	k_msleep(1);
	CSLow();
	k_msleep(1);
	display_command(0xc0); 
	display_data(0x28);
	display_data(0x08);
	display_data(0x04);
	CSHigh();
	k_msleep(1);
	CSLow();
	k_msleep(1);
	display_command(0xc1); 
	display_data(0xc0);
	CSHigh();
	k_msleep(1);
	CSLow();
	k_msleep(1);	
	display_command(0xc2); 
	display_data(0x0d);
	display_data(0x00);
	CSHigh();
	k_msleep(1);
	CSLow();
	k_msleep(1);
	display_command(0xc3); 
	display_data(0x8d);
	display_data(0x2a);
	CSHigh();
	k_msleep(1);
	CSLow();
	k_msleep(1);
	display_command(0xc4); 
	display_data(0x8d);
	display_data(0xee);
	
	// power sequence
	CSHigh();
	k_msleep(1);
	CSLow();
	k_msleep(1);
	display_command(0xc5); // VCOM
	display_data(0x1a);
	CSHigh();
	k_msleep(1);
	CSLow();
	k_msleep(1);
	display_command(0xc6); // MX,MY, RGB mode
	display_data(0xc0);
	k_msleep(100);

	// gamma sequence
	CSHigh();
	k_msleep(1);
	CSLow();
	k_msleep(1);
	display_command(0xe0);
	display_data(0x04);
	display_data(0x22);
	display_data(0x07);
	display_data(0x0a);
	display_data(0x2e);
	display_data(0x30);
	display_data(0x25);
	display_data(0x2a);
	display_data(0x28);
	display_data(0x26);
	display_data(0x2e);
	display_data(0x3a);
	display_data(0x00);
	display_data(0x01);
	display_data(0x03);
	display_data(0x13);
	CSHigh();
	k_msleep(1);
	CSLow();
	k_msleep(1);
	display_command(0xe1);
	display_data(0x04);
	display_data(0x16);
	display_data(0x06);
	display_data(0x0d);
	display_data(0x2d);
	display_data(0x26);
	display_data(0x23);
	display_data(0x27);
	display_data(0x27);
	display_data(0x25);
	display_data(0x2d);
	display_data(0x3b);
	display_data(0x00);
	display_data(0x01);
	display_data(0x04);
	display_data(0x13);
	
	CSHigh();
	k_msleep(1);
	CSLow();
	k_msleep(1);
	display_command(0x3a);// Set colour mode        
	display_data(0x05); 	
	CSHigh();
	k_msleep(1);
	CSLow();
	k_msleep(1);
    display_command(0x29);    // display on
	k_msleep(100);
	CSHigh();
	k_msleep(1);
	CSLow();
	k_msleep(1);
	display_command(0x2c);   // put display in to write mode
	
	display_fillRectangle(0,0,SCREEN_WIDTH, SCREEN_HEIGHT, 0x0000);  // black out the screen
    
	
	while(0)
		{
		uint16_t colour=0x55aa;
		struct spi_buf tx_buf = {.buf = &colour, .len = 2};
		struct spi_buf_set tx_bufs = {.buffers = &tx_buf, .count = 1};
		DCHigh();
		spi_write(spi_display, &cfg, &tx_bufs);
		k_msleep(1);
		}
	
	return 0;
}
void display_command(uint8_t cmd)
{
	DCLow();
	struct spi_buf tx_buf = {.buf = &cmd, .len = 1};
	struct spi_buf_set tx_bufs = {.buffers = &tx_buf, .count = 1};   
	spi_write(spi_display, &cfg, &tx_bufs);
}

void display_data(uint8_t data)
{
	DCHigh();
	struct spi_buf tx_buf = {.buf = &data, .len = 1};
	struct spi_buf_set tx_bufs = {.buffers = &tx_buf, .count = 1};   
	spi_write(spi_display, &cfg, &tx_bufs);
}
void display_openAperture(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2)
{
    // open up an area for drawing on the display    

	display_command(0x2A); // Set X limits    	
    display_data(x1>>8);
    display_data(x1&0xff);        
    display_data(x2>>8);
    display_data(x2&0xff);
    
    display_command(0x2B);// Set Y limits
    display_data(y1>>8);
    display_data(y1&0xff);        
    display_data(y2>>8);
    display_data(y2&0xff);    
        
    display_command(0x2c); // put display in to data write mode

}

void display_fillRectangle(uint16_t x,uint16_t y,uint16_t width, uint16_t height, uint16_t colour)
{
#define PIXEL_CACHE_SIZE 32
	static uint16_t fill_cache[PIXEL_CACHE_SIZE]; // use this to speed up writes
	uint32_t pixelcount = height * width;
	uint32_t blockcount = pixelcount / PIXEL_CACHE_SIZE;
	
	display_openAperture(x, y, x + width - 1, y + height - 1);
	DCHigh();
	struct spi_buf tx_buf = {.buf = &colour, .len = 2};
	struct spi_buf_set tx_bufs = {.buffers = &tx_buf, .count = 1};   

	if (blockcount)
	{
		for (int p=0;p<PIXEL_CACHE_SIZE;p++)
		{
			fill_cache[p]=colour;
		}
	}
	while(blockcount--)
	{
		tx_buf.buf=fill_cache;
		tx_buf.len = PIXEL_CACHE_SIZE*2;
		spi_write(spi_display, &cfg, &tx_bufs);
	}

	pixelcount = pixelcount % PIXEL_CACHE_SIZE;
	while(pixelcount--) 
	{
		tx_buf.buf = &colour;
		tx_buf.len = 2;		
		spi_write(spi_display, &cfg, &tx_bufs);
	}	
}
void display_putPixel(uint16_t x, uint16_t y, uint16_t colour)
{
    display_openAperture(x, y, x + 1, y + 1);
	struct spi_buf tx_buf = {.buf = &colour, .len = 2};
	struct spi_buf_set tx_bufs = {.buffers = &tx_buf, .count = 1};
	DCHigh();
	spi_write(spi_display, &cfg, &tx_bufs);

}
void display_putImage(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint16_t *Image)
{
	display_openAperture(x, y, x + width - 1, y + height - 1);
	struct spi_buf tx_buf = {.buf = Image, .len = width*height*2};
	struct spi_buf_set tx_bufs = {.buffers = &tx_buf, .count = 1};   
	DCHigh();
	spi_write(spi_display, &cfg, &tx_bufs);

}
int iabs(int x) // simple integer version of abs for use by graphics functions        

{
	if (x < 0)
		return -x;
	else
		return x;
}
void display_drawLineLowSlope(uint16_t x0, uint16_t y0, uint16_t x1,uint16_t y1, uint16_t Colour)
{
    // Reference : https://en.wikipedia.org/wiki/Bresenham%27s_line_algorithm    
  int dx = x1 - x0;
  int dy = y1 - y0;
  int yi = 1;
  if (dy < 0)
  {
    yi = -1;
    dy = -dy;
  }
  int D = 2*dy - dx;
  
  int y = y0;

  for (int x=x0; x <= x1;x++)
  {
    display_putPixel(x,y,Colour);    
    if (D > 0)
    {
       y = y + yi;
       D = D - 2*dx;
    }
    D = D + 2*dy;
    
  }
}
void display_drawLineHighSlope(uint16_t x0, uint16_t y0, uint16_t x1,uint16_t y1, uint16_t Colour)
{
        // Reference : https://en.wikipedia.org/wiki/Bresenham%27s_line_algorithm

  int dx = x1 - x0;
  int dy = y1 - y0;
  int xi = 1;
  if (dx < 0)
  {
    xi = -1;
    dx = -dx;
  }  
  int D = 2*dx - dy;
  int x = x0;

  for (int y=y0; y <= y1; y++)
  {
    display_putPixel(x,y,Colour);
    if (D > 0)
    {
       x = x + xi;
       D = D - 2*dy;
    }
    D = D + 2*dx;
  }
}
void display_drawLine(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t Colour)
{
    // Reference : https://en.wikipedia.org/wiki/Bresenham%27s_line_algorithm    
    if ( iabs(y1 - y0) < iabs(x1 - x0) )
    {
        if (x0 > x1)
        {
            display_drawLineLowSlope(x1, y1, x0, y0, Colour);
        }
        else
        {
            display_drawLineLowSlope(x0, y0, x1, y1, Colour);
        }
    }
    else
    {
        if (y0 > y1) 
        {
            display_drawLineHighSlope(x1, y1, x0, y0, Colour);
        }
        else
        {
            display_drawLineHighSlope(x0, y0, x1, y1, Colour);
        }
        
    }    
}
void display_drawRectangle(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t Colour)
{
    display_drawLine(x,y,x+w,y,Colour);
    display_drawLine(x,y,x,y+h,Colour);
    display_drawLine(x+w,y,x+w,y+h,Colour);
    display_drawLine(x,y+h,x+w,y+h,Colour);
}
void display_drawCircle(uint16_t x0, uint16_t y0, uint16_t radius, uint16_t Colour)
{
// Reference : https://en.wikipedia.org/wiki/Midpoint_circle_algorithm
    int x = radius-1;
    int y = 0;
    int dx = 1;
    int dy = 1;
    int err = dx - (radius << 1);
    if (radius > x0)
        return; // don't draw even parially off-screen circles
    if (radius > y0)
        return; // don't draw even parially off-screen circles
        
    if ((x0+radius) > SCREEN_WIDTH)
        return; // don't draw even parially off-screen circles
    if ((y0+radius) > SCREEN_HEIGHT)
        return; // don't draw even parially off-screen circles    
    while (x >= y)
    {
        display_putPixel(x0 + x, y0 + y, Colour);
        display_putPixel(x0 + y, y0 + x, Colour);
        display_putPixel(x0 - y, y0 + x, Colour);
        display_putPixel(x0 - x, y0 + y, Colour);
        display_putPixel(x0 - x, y0 - y, Colour);
        display_putPixel(x0 - y, y0 - x, Colour);
        display_putPixel(x0 + y, y0 - x, Colour);
        display_putPixel(x0 + x, y0 - y, Colour);

        if (err <= 0)
        {
            y++;
            err += dy;
            dy += 2;
        }
        
        if (err > 0)
        {
            x--;
            dx += 2;
            err += dx - (radius << 1);
        }
    }
}
void display_fillCircle(uint16_t x0, uint16_t y0, uint16_t radius, uint16_t Colour)
{
// Reference : https://en.wikipedia.org/wiki/Midpoint_circle_algorithm
// Similar to drawCircle but fills the circle with lines instead
    int x = radius-1;
    int y = 0;
    int dx = 1;
    int dy = 1;
    int err = dx - (radius << 1);

    if (radius > x0)
        return; // don't draw even parially off-screen circles
    if (radius > y0)
        return; // don't draw even parially off-screen circles
        
    if ((x0+radius) > SCREEN_WIDTH)
        return; // don't draw even parially off-screen circles
    if ((y0+radius) > SCREEN_HEIGHT)
        return; // don't draw even parially off-screen circles        
    while (x >= y)
    {
        display_drawLine(x0 - x, y0 + y,x0 + x, y0 + y, Colour);        
        display_drawLine(x0 - y, y0 + x,x0 + y, y0 + x, Colour);        
        display_drawLine(x0 - x, y0 - y,x0 + x, y0 - y, Colour);        
        display_drawLine(x0 - y, y0 - x,x0 + y, y0 - x, Colour);        

        if (err <= 0)
        {
            y++;
            err += dy;
            dy += 2;
        }
        
        if (err > 0)
        {
            x--;
            dx += 2;
            err += dx - (radius << 1);
        }
    }
}
void display_print(const char *Text, uint16_t len, uint16_t x, uint16_t y, uint16_t ForeColour, uint16_t BackColour)
{
        // This function draws each character individually.  It uses an array called TextBox as a temporary storage
    // location to hold the dots for the character in question.  It constructs the image of the character and then
    // calls on putImage to place it on the screen
    uint8_t Index = 0;
    uint8_t Row, Col;
    const uint8_t *CharacterCode = 0;    
    uint16_t TextBox[FONT_WIDTH * FONT_HEIGHT];
    for (Index = 0; Index < len; Index++)
    {
        CharacterCode = &Font5x7[FONT_WIDTH * (Text[Index] - 32)];
        Col = 0;
        while (Col < FONT_WIDTH)
        {
            Row = 0;
            while (Row < FONT_HEIGHT)
            {
                if (CharacterCode[Col] & (1 << Row))
                {
                    TextBox[(Row * FONT_WIDTH) + Col] = ForeColour;
                }
                else
                {
                    TextBox[(Row * FONT_WIDTH) + Col] = BackColour;
                }
                Row++;
            }
            Col++;
        }
        display_putImage(x, y, FONT_WIDTH, FONT_HEIGHT, (uint16_t *)TextBox);
        x = x + FONT_WIDTH + 2;
    }
}
uint16_t display_RGBToWord(uint16_t R, uint16_t G, uint16_t B)
{
    uint16_t rvalue = 0;
    rvalue += G >> 5;
    rvalue += (G & (0b111)) << 13;
    rvalue += (R >> 3) << 8;
    rvalue += (B >> 3) << 3;
    return rvalue;
}
