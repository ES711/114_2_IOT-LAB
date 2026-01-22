#include "max7219.h"
#include "spi.h"
#include "string.h"

#define MAXInUse 4

void Init_MAX7219_8x8(void)
{
	int i;
	for(int i = 0; i < MAXInUse; i++)
  {
		 Write_Max7219(0x0f, 0x00);       //Display-Test Register Format	
		 Write_Max7219(0x0c, 0x01);       //Shutdown Register Format	
		 Write_Max7219(0x0b, 0x07);       //Scan-Limit Register Format
		 Write_Max7219(0x0a, 0x00);       //Intensity Register Format	
		 Write_Max7219(0x09, 0x00);       //No decode for digits
  }

	for(int i = 0; i < 8; i++)
	{
		//clear
		Write_Max7219(i + 1, 0);
		Write_Max7219(i + 1, 0);
		Write_Max7219(i + 1, 0);
		Write_Max7219(i + 1, 0);
	}
}

void Write_Matrix(uint8_t address, uint8_t* dat)
{
  HAL_GPIO_WritePin(GPIOF, Max7219_8x8_Pin, GPIO_PIN_RESET); // NSS1 low
	for(int i = 0; i < MAXInUse; i++)//
	{
		HAL_SPI_Transmit(&hspi1, &address, 1 ,100);   //write address            
		HAL_SPI_Transmit(&hspi1, &dat[i], 1 ,100);    //write data 	
	}
  HAL_GPIO_WritePin(GPIOF, Max7219_8x8_Pin, GPIO_PIN_SET); // NSS1 HIGH
}

void Write_Max7219(uint8_t address, uint8_t dat)
{ 
	 HAL_GPIO_WritePin(GPIOF, Max7219_8x8_Pin, GPIO_PIN_RESET); // NSS1 low
	 HAL_SPI_Transmit(&hspi1, &address, 1 ,100); //write address
	 HAL_SPI_Transmit(&hspi1, &dat, 1 ,100);     //write data
	 HAL_GPIO_WritePin(GPIOF, Max7219_8x8_Pin, GPIO_PIN_SET); // NSS1 HIGH                 
}