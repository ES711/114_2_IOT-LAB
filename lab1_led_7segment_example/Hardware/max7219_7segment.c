#include "spi.h"
#include "main.h"

/*
disable max7219 
	cs -> 1
enable max7219
	cs -> 0
*/
void Write_Max7219(uint8_t address, uint8_t data){
	HAL_GPIO_WritePin(CS_GPIO_Port, CS_Pin, GPIO_PIN_RESET);//cs down
	HAL_SPI_Transmit(&hspi1, &address, 1, 100);
	HAL_SPI_Transmit(&hspi1, &data, 1, 100);
	HAL_GPIO_WritePin(CS_GPIO_Port, CS_Pin, GPIO_PIN_SET);//cs up
}

void Init_Max7219(){
	//test mode -> 0x01
	Write_Max7219(0x0f, 0x00);
	//shutdown mode -> 0x00
	Write_Max7219(0x0c, 0x01);
	//scan limit
	Write_Max7219(0x0b, 0x07);
	//brightness
	Write_Max7219(0x0a, 0x00);
	//decode mode
	Write_Max7219(0x09, 0xff);
}