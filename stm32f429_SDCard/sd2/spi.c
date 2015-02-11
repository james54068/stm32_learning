#include "spi.h"

SPI_InitTypeDef  SPI_InitStruct;
void SPIx_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
  	/* SPI1 Clk Init ************************************************************/
  	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);	
	/* SPI AF ******************************************************************/
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource5, GPIO_AF_SPI1);
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource6, GPIO_AF_SPI1);
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource7, GPIO_AF_SPI1);
	/* CSM PB12 */
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_4;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStruct.GPIO_OType = GPIO_Mode_OUT;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStruct);
	/* SCK PB13 PB14 PB15*/
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStruct);

	/*SPI Configuration*/
	/*Full-duplex synchronous transfer*/
	SPI_InitStruct.SPI_Direction = SPI_Direction_2Lines_FullDuplex; 
	/*Stm32f429 is master*/
	SPI_InitStruct.SPI_Mode = SPI_Mode_Master;		
	SPI_InitStruct.SPI_DataSize = SPI_DataSize_8b;
	/*1(voltage high) while idle*/		
	SPI_InitStruct.SPI_CPOL = SPI_CPOL_High;
	/*get value at 2nd edge*/		
	SPI_InitStruct.SPI_CPHA = SPI_CPHA_2Edge;
	/*Slave_Select by software*/	
	SPI_InitStruct.SPI_NSS = SPI_NSS_Soft;	
	/*90Mhz/SPI_BaudRatePrescaler_256*/	
	SPI_InitStruct.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_256;		
	/*Big Endian*/	
	SPI_InitStruct.SPI_FirstBit = SPI_FirstBit_MSB;	
	SPI_InitStruct.SPI_CRCPolynomial = 7;	
	SPI_Init(SPI1, &SPI_InitStruct);  
 
	SPI_Cmd(SPI1, ENABLE); 
	
	//SPIx_ReadWriteByte(0xff);		 
}   

  
void SPIx_SetSpeed(u8 SpeedSet)		
{
	SPI_InitStruct.SPI_BaudRatePrescaler = SpeedSet ;
  	SPI_Init(SPI1, &SPI_InitStruct);
	SPI_Cmd(SPI1,ENABLE);
} 


u8 SPIx_ReadWriteByte(u8 TxData)
{		
	u8 retry=0;	
	/*check "Transmit buffer empty flag"*/			 
	while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET) 
		{
			retry++;
			if(retry>200)return 0;
		}		

	SPI_I2S_SendData(SPI1, TxData);
	retry=0;
	/*check "Receive buffer not empty flag"*/
	while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET); 
		{
			retry++;
			if(retry>200)return 0;
		}	  						    
	return SPI_I2S_ReceiveData(SPI1); 
}
