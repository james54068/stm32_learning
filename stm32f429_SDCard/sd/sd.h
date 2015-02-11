#ifndef _SD_CARD_H_
#define _SD_CARD_H_		 
#include "stm32f4xx.h"
/*
Unit concept(differ from fatfs)
Basic read/write :Block(512 Byte for SDHC)
Basic erase :->erase_blk_on->Block
			 ->erase_blk_off->Sector(64 KByte for SDHC)
*/
 						    	 
/*SD card type*/
#define SD_TYPE_ERR     0X00
#define SD_TYPE_MMC     0X01
#define SD_TYPE_V1      0X02
#define SD_TYPE_V2      0X04
#define SD_TYPE_V2HC    0X06
/******************************************************************************
Card registers

OCR
CID
CSD

**************************************************************************************/

/**********************************************************
Command Type

Byte1      			Byte 2-5     				Byte 6
0|1|Command(6bits)	Command Argument(32bits)	CRC(7bits)|1

**************************************************************
Responses

R1(1 Byte)
0|Parameter error|Address error|Erase_Seq error|CRC error|Illegal command|Eease reset|In idle state

R1b(1~n Byte)
0 -> busy 
!0 ->nonbusy 

R2(2 Byte) SEND_STATUS command
R1|Out of Range, CSD_Overwrite|Erase Param|WP Violation|Card ECC Failed|CC Error|Error|WP Erase Skip, Lock/Unlock Cmd Failed|Card is Locked

R3(5 Byte)
R1(1 Byte)+OCR(4 Byte)

R7(5 Byte)
  R1 	|command version	|reserved bits 	|voltage accepted	|check pattern
(8bits)      (31-28 )			(27-12 )         (11-8)   			(7-0)
**********************************************************


************************************************************
Control Tokens

Data Response Tokens(1 Byte)
x|x|x|0|status(3bits)|1
status   = 	010 Data accepted
			101 Data rejected due to a CRC error
			110 Data Rejected due to a Write Error


Data Tokens(4-515 Byte long)
_________________________________________________________

Single Block Read/Write & Multiple Read

Start block:
1|1|1|1|1|1|1|0(first Byte)
User data:
(2-513Byte)
CRC:
(last 2 Byte)
__________________________________________________________

Multiple Block Write

Start block:
1|1|1|1|1|1|0|0
if stop transmission is requested
1|1|1|1|1|1|0|1
________________________________________________________

Data error Tokens

0|0|0|Card is Locked|Out of Range|Card ECC Failed|CC Error|Error

**********************************************************/			   
/************************************************************
SD CARD standard Commands(in SPI mode commands work without CRC except for CMD0,8 )
CMD0   	R1 	Resets the SD Card
CMD1 	R1 	Activates the card’s initialization process
CMD6    R1  Checks switchable function (mode 0) and switches card function (mode 1).
CMD8    R7  Sends SD Memory Card interface condition that includes host supply voltage information and asks the accessed card whether card can operate in supplied voltage range.Reserved bits shall be set to '0'.
CMD9 	R1 	Asks the selected card to send its card-specific data (CSD).
CMD10  	R1 	Asks the selected card to send its card identification (CID).
CMD12	R1b Forces the card to stop transmission during a multiple block read operation.
CMD13	R2 	Asks the selected card to send its status register.
CMD16   R1 	Selects a block length (in bytes) for all following block commands (read & write). 1
CMD17   R1  Reads a block of the size selected by the SET_BLOCKLEN command. 2 
CMD18   R1  Continuously transfers data blocks from card to host until interrupted by a STOP_ TRANSMISSION command.
CMD24	R1 	Writes a block of the size selected by the SET_BLOCKLEN command. 4
CMD25   R1  Continuously writes blocks of data until a stop transmission token is sent (instead of ‘start block’)
CMD27   R1  Programming of the programmable bits of the CSD.
CMD28   R1b If the card has write protection features, this command sets the write protection bit of the addressed group. The properties of write protection are coded in the card specific data (WP_GRP_SIZE).
CMD29 	R1b If the card has write protection features, this command clears the write protection bit of the addressed group.
CMD30   R1  If the card has write protection features, this command asks the card to send the status of the write protection bits. 2
CMD32   R1  Sets the address of the first write block to be erased.
CMD33   R1  Sets the address of the last write block in a continuous range to be erased.
CMD38   R1b Erases all previously selected write blocks.
CMD55   R1  Notifies the card that the next command is an application specific command rather than a standard command.
CMD56 	R1  Used either to transfer a Data Block to the card or to get a Data Block from the card for general purpose/application specific commands. The size of the Data Block is defined with SET_BLOCK_LEN command.
CMD58   R3  Reads the OCR register of a card.
CMD59   R1  Turns the CRC option on or off. A ‘1’ in the CRC option bit will turn the option on, a ‘0’ will turn it off.
*****************************************************************************************************/ 	   
/******************************************************************************************************
SD CARD SPI MODE only Command
CMD23   R1  Set the number of write blocks to be pre-erased before writing (to be used for faster Multiple Block WR command). “1”=default (one wr block)(2).
CMD41   R1  Activates the card’s initialization process.

******************************************************************************************************/
#define CMD0    0       
#define CMD1    1
#define CMD8    8  
#define CMD9    9  
#define CMD10   10  
#define CMD12   12  
#define CMD16   16   
#define CMD17   17   
#define CMD18   18    
#define CMD23   23   
#define CMD24   24   
#define CMD25   25    
#define CMD41   41   
#define CMD55   55     
#define CMD58   58     
#define CMD59   59      

/*Data Response Tokens meaning*/
#define MSD_DATA_OK                0x05
#define MSD_DATA_CRC_ERROR         0x0B
#define MSD_DATA_WRITE_ERROR       0x0D
#define MSD_DATA_OTHER_ERROR       0xFF

/*R1 response meaning*/
#define MSD_RESPONSE_NO_ERROR      0x00
#define MSD_IN_IDLE_STATE          0x01
#define MSD_ERASE_RESET            0x02
#define MSD_ILLEGAL_COMMAND        0x04
#define MSD_COM_CRC_ERROR          0x08
#define MSD_ERASE_SEQUENCE_ERROR   0x10
#define MSD_ADDRESS_ERROR          0x20
#define MSD_PARAMETER_ERROR        0x40
#define MSD_RESPONSE_FAILURE       0xFF
 							   						 	 
/*CS pin definition*/
#define SD_CS_PORT GPIOA
#define SD_CS_CLK  RCC_AHB1Periph_GPIOA
#define SD_CS_PIN  GPIO_Pin_4
#define Set_SD_CS  GPIO_SetBits(SD_CS_PORT,SD_CS_PIN);
#define Clr_SD_CS  GPIO_ResetBits(SD_CS_PORT,SD_CS_PIN);
				    	  

extern u8  SD_Type;

void SD_DisSelect(void);
u8 SD_Select(void);
u8 SD_WaitReady(void);							
u8 SD_GetResponse(u8 Response);					
u8 SD_Initialize(void);							
u8 SD_ReadDisk(u8*buf,u32 sector,u8 cnt);		
u8 SD_WriteDisk(u8*buf,u32 sector,u8 cnt);		
u32 SD_GetSectorCount(u8 *csd);  					
u8 SD_GetCID(u8 *cid_data);                    
u8 SD_GetCSD(u8 *csd_data);                    
 
#endif





