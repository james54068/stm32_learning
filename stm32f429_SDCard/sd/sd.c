#include "sd.h"			   
#include "spi.h"

u8  SD_Type=0; 

void SD_SPI_SpeedLow(void)
{
 	SPIx_SetSpeed(SPI_BaudRatePrescaler_256);	
}
void SD_SPI_SpeedHigh(void)
{
 	SPIx_SetSpeed(SPI_BaudRatePrescaler_32);	
}


void SD_DisSelect(void)
{
	Set_SD_CS;
	SPIx_ReadWriteByte(0xff);
}
u8 SD_Select(void)
{
	Clr_SD_CS;
	/*wait sd card to be ready*/
	if(SD_WaitReady()==0)return 0;
	/*wait too long then skip it*/
	SD_DisSelect();
	return 1;
}

u8 SD_WaitReady(void)
{
	u32 t=0;
	do
	{
		/*Get 0xFF means redy to accept command(Timming Diagrams)*/
		if(SPIx_ReadWriteByte(0XFF)==0XFF)return 0;	

	}while(t<0XFFFFFF && t++); 
	return 1;
}

u8 SD_GetResponse(u8 Response)
{
	u16 Count=0xFF;  
	/*read data response start byte with 0xFF*/						  
	while ((SPIx_ReadWriteByte(0XFF)!=Response)&&Count)Count--; 	  
	if (Count==0)return MSD_RESPONSE_FAILURE;  
	else return MSD_RESPONSE_NO_ERROR;
}

u8 SD_RecvData(u8*buf,u16 len)
{	
	/*check read response(first byte)*/		  	  
	if(SD_GetResponse(0xFE))return 1;
    while(len--)
    {
        *buf=SPIx_ReadWriteByte(0xFF);
        buf++;
    }
    /*CRC Byte*/
    SPIx_ReadWriteByte(0xFF);
    SPIx_ReadWriteByte(0xFF);									  					    
    return 0;
}
	
u8 SD_SendBlock(u8*buf,u8 cmd)
{	
	u16 t;		  	  
	if(SD_WaitReady())return 1;
	/*send data token(first byte)*/
	SPIx_ReadWriteByte(cmd);
	/*single block or multiple blocks write*/
	if(cmd!=0xFE||cmd!=0xFC)
	{
		/*512 bytes*/
		for(t=0;t<512;t++)SPIx_ReadWriteByte(buf[t]);
		/*2 CRC bytes*/
	    SPIx_ReadWriteByte(0xFF);
	    SPIx_ReadWriteByte(0xFF);
		t=SPIx_ReadWriteByte(0xFF);
		/*Data Response Tokens(check if receive correctly)*/
		if((t&0x1F)!=0x05)return 2;									  					    
	}						 									  					    
    return 0;
}
												  
u8 SD_SendCmd(u8 cmd, u32 arg, u8 crc)
{ 
    u8 r1;	
	u8 Retry=0; 
	SD_DisSelect();
	if(SD_Select())return 0XFF;
	/*( cmd | 0x40 ) because of command format*/
    SPIx_ReadWriteByte(cmd | 0x40);
    SPIx_ReadWriteByte(arg >> 24);
    SPIx_ReadWriteByte(arg >> 16);
    SPIx_ReadWriteByte(arg >> 8);
    SPIx_ReadWriteByte(arg);	  
    SPIx_ReadWriteByte(crc); 
    /*R1b response */
	if(cmd==CMD12)SPIx_ReadWriteByte(0xFF);
	Retry=0X1F;
	/*Wait for response*/
	do
	{
		r1 = SPIx_ReadWriteByte(0xFF);
	/*check r1 response is correct 0|~*/
	}while((r1&0X80) && Retry--);	 
    return r1;
}		    																			  
													   
u8 SD_GetCID(u8 *cid_data)
{
   	u8 retry=0xFF;
    u8 r1;	 
    do
    {
    	/*Get CID command*/
    	r1=SD_SendCmd(CMD10,0,0x01);
    }while(r1 && retry--);
    if(r1==0x00)
	{
		/*CID is 16 Byte*/
		r1=SD_RecvData(cid_data,16); 
    }
	SD_DisSelect();
	if(r1)return 1;
	else return 0;
}																				  
														   
u8 SD_GetCSD(u8 *csd_data)
{ 	
	u8 retry=0xFF;
    u8 r1;	 
    do
    {
    	/*Get CSD command*/
    	r1=SD_SendCmd(CMD9,0,0x01);
    }while(r1 && retry--);
    if(r1==0x00)
	{
		/*CSD is 16 Byte*/
    	r1=SD_RecvData(csd_data, 16);
    }
	SD_DisSelect();
	if(r1)return 1;
	else return 0;
}  
													  
u32 SD_GetSectorCount(u8 *csd)
{
    //u8 	csd[16];
    u32 Capacity;  
    u8 	n;
	u32 csize; 
	u8  bl_len;
	u8  mult;					    
    if(SD_GetCSD(csd)!=0) return 0;	
    /*check CSD version(2.0HC)*/    
    if((csd[0]&0xC0)==0x40)	 
    {	

  		csize = (u32)csd[9] + (((u32)csd[8])<<8) + (((u32)csd[7])<<16)+(((u32)(csd[6]&0x0F))<<24)+1;
		/*bl_len = 512 Bytes,mult = 1024*/
		Capacity= csize << 10 ;  
		 		   
    }else/*check CSD version(1.0)*/  
    {	
    	csize = (u16)csd[9] + ((u16)(csd[8]&0x0F) << 8) + 1;
		bl_len = csd[5]&0x0F; 
		mult = (csd[10]>>1)&0x07;
		Capacity = (u32)csize << (mult+2);
    }
    return Capacity;
}

/*u8 SD_Idle_Sta(void)
{
	u16 i;
	u8 retry;	   	  
    for(i=0;i<0xf00;i++);	 
   
    for(i=0;i<10;i++)SPIx_ReadWriteByte(0xFF); 

    retry = 0;
    do
    {	   
       
        i = SD_SendCmd(CMD0, 0, 0x95);
        retry++;
    }while((i!=0x01)&&(retry<200));
    
    if(retry==200)return 1; 
	return 0;				  
}
*/

u8 SD_Initialize(void)
{
    u8 r1;      
    u16 retry;  
    u8 buf[4];  
	u16 i;

	SPIx_Init();	
 	SD_SPI_SpeedLow();		  
	for(i=0;i<10;i++)SPIx_ReadWriteByte(0XFF);
	retry=20;
	do
	{
		/*Resets the SD Card*/
		r1=SD_SendCmd(CMD0,0,0x95);
	/*send until response is right*/
	}while((r1!=0X01) && retry--);
 	SD_Type=0;
	if(r1==0X01)
	{
		/*check voltage(get R1)*/
		if(SD_SendCmd(CMD8,0x1AA,0x87)==1)
		{   
			for(i=0;i<4;i++)buf[i]=SPIx_ReadWriteByte(0XFF);
			/*check VHS & Pattern Echo back*/	
			if(buf[2]==0X01&&buf[3]==0XAA)
			{/*support 2.7~3.6V*/
				retry=0XFFFE;
				do
				{
					/*indicate next command following command is spi only */
					SD_SendCmd(CMD55,0,0X01);	
					/*HCS = 1 -> support SDHC(high capacity) */
					r1=SD_SendCmd(CMD41,0x40000000,0X01);
				}while(r1&&retry--);
				if(retry&&SD_SendCmd(CMD58,0,0X01)==0)
				{
					/*read OCR register */
					for(i=0;i<4;i++)buf[i]=SPIx_ReadWriteByte(0XFF);
					if(buf[0]&0x40)SD_Type=SD_TYPE_V2HC;    
					else SD_Type=SD_TYPE_V2;   
				}
			}
			else
			{
				printf("Check support voltage!\r\n");
			}
		}else
		{
			/*MMC will get lots of error V1 will idle or not idle*/
			SD_SendCmd(CMD55,0,0X01);	
			r1=SD_SendCmd(CMD41,0,0X01);
			if(r1<=1)
			{		
				SD_Type=SD_TYPE_V1;
				retry=0XFFFE;
				do 
				{
					SD_SendCmd(CMD55,0,0X01);	
					r1=SD_SendCmd(CMD41,0,0X01);
				}while(r1&&retry--);
			}else
			{
				SD_Type=SD_TYPE_MMC;
				retry=0XFFFE;
				do 
				{											    
					r1=SD_SendCmd(CMD1,0,0X01);
				}while(r1&&retry--);  
			}
			/*no response || can not set 512 Byte Block*/
			if(retry==0||SD_SendCmd(CMD16,512,0X01)!=0)SD_Type=SD_TYPE_ERR;
		}
	}
	SD_DisSelect();
	SD_SPI_SpeedHigh();
	if(SD_Type)return 0;
	else if(r1)return r1; 	   
	return 0xaa;
}
 

u8 SD_ReadDisk(u8*buf,u32 sector,u8 cnt)
{
	u8 r1;
	u8 retry=0xFF;
	/*read 1 block(a sector in fatfs) 512 Byte*/
	if(SD_Type!=SD_TYPE_V2HC)sector <<= 9;
	/*read 1 block*/
	if(cnt==1)
	{		
		do
		{
			/*Reads a block of the size command*/
			r1=SD_SendCmd(CMD17,sector,0X01);
		}while(r1&&retry--);
		if(r1==0)
		{
			/*recieve 512 Bytes*/
			r1=SD_RecvData(buf,512);  
		}
	}
	else
	{
		/*Continuously transfers data blocks from card to host*/
		do
		{
			r1=SD_SendCmd(CMD18,sector,0X01);
		}while(r1&&retry--);
		if(r1==0)
		{
			do
			{
				/*recieve multiple blocks command*/
				r1=SD_RecvData(buf,512); 
				buf+=512;  
			}while(--cnt && r1==0);
			/*Forces the card to stop transmission*/ 	
			SD_SendCmd(CMD12,0,0X01);
		}
		
	}   
	SD_DisSelect();
	return r1;
}

u8 SD_WriteDisk(u8*buf,u32 sector,u8 cnt)
{
	u8 r1;
	u16 retry=0xFFFF;
	/*read 1 block(a sector in fatfs) 512 Byte*/
	if(SD_Type!=SD_TYPE_V2HC)sector *= 512;
	/*write 1 block*/
	if(cnt==1)
	{		
		do
		{
			/*Writes a block of the size command*/
			r1=SD_SendCmd(CMD24,sector,0X01);
		}while(r1&&retry--);
		if(r1==0)
		{	
			/*send 512 Bytes*/
			r1=SD_SendBlock(buf,0xFE); 
		}
	}else
	{	/*MMC write*/
		if(SD_Type!=SD_TYPE_MMC)
		{
			SD_SendCmd(CMD55,0,0X01);	
			SD_SendCmd(CMD23,cnt,0X01);	
		}

		do
		{
			/*Writes multiple blocks command*/
			r1=SD_SendCmd(CMD25,sector,0X01);
		}while(r1&&retry--);
		if(r1==0)
		{
			do
			{
				/*Writes multiple blocks(different start byte)*/
				r1=SD_SendBlock(buf,0xFC);
				buf+=512;  
			}while(--cnt && r1==0);
			r1=SD_SendBlock(0,0xFD);
		}
	}   
	SD_DisSelect();
	return r1;
}