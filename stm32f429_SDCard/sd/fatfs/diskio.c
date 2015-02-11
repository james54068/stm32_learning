#include "sd.h"	
#include "diskio.h"
#include  <stdio.h>	 

#define SD_CARD	 0  
#define EX_FLASH 1	

#define FLASH_SECTOR_SIZE 	512

#define FLASH_SECTOR_COUNT 	4096  
#define FLASH_BLOCK_SIZE  	8     

//³õÊ¼»¯´ÅÅÌ
DSTATUS disk_initialize (
	BYTE drv				/* Physical drive nmuber (0..) */
)
{	
	u8 res;	    
	switch(drv)
	{
		case SD_CARD://SD¿¨
			res = SD_Initialize();//SD_Initialize();
			break;
//		case EX_FLASH:
//			SPI_Flash_Init();
//			if(SPI_Flash_ReadID()!=SPI_FLASH_TYPE)res=1;//W25Q16 ID=0XEF15
//			else res=0;
//			break;
		default:
			res=1; 
	}		 
	if(res)return  STA_NOINIT;
	else return 0; 
}   

DSTATUS disk_status (
	BYTE drv		/* Physical drive nmuber (0..) */
)
{		   
    return 0;
}

DRESULT disk_read (
	BYTE drv,		/* Physical drive nmuber (0..) */
	BYTE *buff,		/* Data buffer to store read data */
	DWORD sector,	/* Sector address (LBA) */
	BYTE count		/* Number of sectors to read (1..255) */
)
{
	u8 res=0; 
    if (!count)return RES_PARERR;		 	 
	switch(drv)
	{
		case SD_CARD://SD¿¨
			res=SD_ReadDisk(buff,sector,count);
			break;
//		case EX_FLASH:
//			for(;count>0;count--)
//			{
//				SPI_Flash_Read(buff,sector*FLASH_SECTOR_SIZE,FLASH_SECTOR_SIZE);
//				sector++;
//				buff+=FLASH_SECTOR_SIZE;
//			}
//			res=0;
//			break;
		default:
			res=1; 
	}
    if(res == 0x00)return RES_OK;	 
    else return RES_ERROR;	   
}  

	    
#if _READONLY == 0
DRESULT disk_write (
	BYTE drv,			/* Physical drive nmuber (0..) */
	const BYTE *buff,	        /* Data to be written */
	DWORD sector,		/* Sector address (LBA) */
	BYTE count			/* Number of sectors to write (1..255) */
)
{
	u8 res=0;  
    if (!count)return RES_PARERR;		 	 
	switch(drv)
	{
		case SD_CARD:
			res=SD_WriteDisk((u8*)buff,sector,count);
			break;
//		case EX_FLASH:
//			for(;count>0;count--)
//			{
//				//SPI_Flash_Erase_Sector(sector);
//				SPI_Flash_Write((u8*)buff,sector*FLASH_SECTOR_SIZE,FLASH_SECTOR_SIZE);
//				sector++;
//				buff+=FLASH_SECTOR_SIZE;
//			}
//			res=0;
//			break;
		default:
			res=1; 
	}
    if(res == 0x00)return RES_OK;	 
    else return RES_ERROR;		 
}
#endif /* _READONLY */


DRESULT disk_ioctl (
	BYTE drv,		/* Physical drive nmuber (0..) */
	BYTE ctrl,		/* Control code */
	void *buff		/* Buffer to send/receive control data */
)
{	
	DRESULT res;						  
	if(drv==SD_CARD)
	{
	    switch(ctrl)
	    {
		    case CTRL_SYNC:
				Clr_SD_CS;
		        if(SD_WaitReady()==0)res = RES_OK; 
		        else res = RES_ERROR;	  
				Set_SD_CS;
		        break;	 
		    case GET_SECTOR_SIZE:
		        *(WORD*)buff = 512;
		        res = RES_OK;
		        break;	 
		    case GET_BLOCK_SIZE:
		        *(WORD*)buff = 8;
		        res = RES_OK;
		        break;	 
		    case GET_SECTOR_COUNT:
		        *(DWORD*)buff = SD_GetSectorCount(buff);
		        res = RES_OK;
		        break;
		    default:
		        res = RES_PARERR;
		        break;
	    }
	}else if(drv==EX_FLASH)	  
	{
	    switch(ctrl)
	    {
		    case CTRL_SYNC:
				res = RES_OK; 
		        break;	 
		    case GET_SECTOR_SIZE:
		        *(WORD*)buff = FLASH_SECTOR_SIZE;
		        res = RES_OK;
		        break;	 
		    case GET_BLOCK_SIZE:
		        *(WORD*)buff = FLASH_BLOCK_SIZE;
		        res = RES_OK;
		        break;	 
		    case GET_SECTOR_COUNT:
		        *(DWORD*)buff = FLASH_SECTOR_COUNT;
		        res = RES_OK;
		        break;
		    default:
		        res = RES_PARERR;
		        break;
	    }
	}else res=RES_ERROR;
    return res;
}   
                                                                                                                                                                                                                                            
DWORD get_fattime (void)
{
	return 0;
}




