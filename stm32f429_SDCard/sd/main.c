 /**
  ******************************************************************************
  * @file    Touch_Panel/main.c
  * @author  MCD Application Team
  * @version V1.0.1
  * @date    11-November-2013
  * @brief   This example describes how to configure and use the touch panel 
  *          mounted on STM32F429I-DISCO boards.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2013 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software 
  * distributed under the License is distributed on an "AS IS" BASIS, 
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include <string.h>
#include <math.h>
#include "main.h"
#include "drawing.h"
#include "init_mcu.h"
#include "can.h"
#include "sd.h"
#include "ff.h"
#include "integer.h"
#include "file_command.h"



/* Private macro -------------------------------------------------------------*/
#define ABS(x)                     (x < 0) ? (-x) : x
#define L3G_Sensitivity_250dps     (float)114.285f        /*!< gyroscope sensitivity with 250 dps full scale [LSB/dps]  */
#define L3G_Sensitivity_500dps     (float)57.1429f        /*!< gyroscope sensitivity with 500 dps full scale [LSB/dps]  */
#define L3G_Sensitivity_2000dps    (float)14.285f         /*!< gyroscope sensitivity with 2000 dps full scale [LSB/dps] */
  

static void Demo_GyroConfig(void);
static void Demo_GyroReadAngRate (float* pfData);


static void Demo_GyroConfig(void)
{
  L3GD20_InitTypeDef L3GD20_InitStructure;
  L3GD20_FilterConfigTypeDef L3GD20_FilterStructure;

  /* Configure Mems L3GD20 */
  L3GD20_InitStructure.Power_Mode = L3GD20_MODE_ACTIVE;
  L3GD20_InitStructure.Output_DataRate = L3GD20_OUTPUT_DATARATE_1;
  L3GD20_InitStructure.Axes_Enable = L3GD20_AXES_ENABLE;
  L3GD20_InitStructure.Band_Width = L3GD20_BANDWIDTH_4;
  L3GD20_InitStructure.BlockData_Update = L3GD20_BlockDataUpdate_Continous;
  L3GD20_InitStructure.Endianness = L3GD20_BLE_LSB;
  L3GD20_InitStructure.Full_Scale = L3GD20_FULLSCALE_500; 
  L3GD20_Init(&L3GD20_InitStructure);
  
  L3GD20_FilterStructure.HighPassFilter_Mode_Selection =L3GD20_HPM_NORMAL_MODE_RES;
  L3GD20_FilterStructure.HighPassFilter_CutOff_Frequency = L3GD20_HPFCF_0;
  L3GD20_FilterConfig(&L3GD20_FilterStructure) ;
  
  L3GD20_FilterCmd(L3GD20_HIGHPASSFILTER_ENABLE);
}

static void Demo_GyroReadAngRate (float* pfData)
{
  uint8_t tmpbuffer[6] ={0};
  int16_t RawData[3] = {0};
  uint8_t tmpreg = 0;
  float sensitivity = 0;
  int i =0;
  
  L3GD20_Read(&tmpreg,L3GD20_CTRL_REG4_ADDR,1);
  
  L3GD20_Read(tmpbuffer,L3GD20_OUT_X_L_ADDR,6);
  
  /* check in the control register 4 the data alignment (Big Endian or Little Endian)*/
  if(!(tmpreg & 0x40))
  {
    for(i=0; i<3; i++)
    {
      RawData[i]=(int16_t)(((uint16_t)tmpbuffer[2*i+1] << 8) + tmpbuffer[2*i]);
    }
  }
  else
  {
    for(i=0; i<3; i++)
    {
      RawData[i]=(int16_t)(((uint16_t)tmpbuffer[2*i] << 8) + tmpbuffer[2*i+1]);
    }
  }
  
  /* Switch the sensitivity value set in the CRTL4 */
  switch(tmpreg & 0x30)
  {
  case 0x00:
    sensitivity=L3G_Sensitivity_250dps;
    break;
    
  case 0x10:
    sensitivity=L3G_Sensitivity_500dps;
    break;
    
  case 0x20:
    sensitivity=L3G_Sensitivity_2000dps;
    break;
  }
  /* divide by sensitivity */
  for(i=0; i<3; i++)
  {
  pfData[i]=(float)RawData[i]/sensitivity;
  }
}


uint32_t L3GD20_TIMEOUT_UserCallback(void)
{
  return 0;
}


static inline void Delay_1us(uint32_t nCnt_1us)
{
  volatile uint32_t nCnt;

  for (; nCnt_1us != 0; nCnt_1us--)
    for (nCnt = 13; nCnt != 0; nCnt--);
}

float Buffer[6];
/* File system object structure (FATFS) */
FATFS     fs;
/* File object structure (FIL) */
FIL       fsrc, fdst;
/* Directory object structure (DIR) */
DIR       dir;
 /*File status structure (FILINFO) */
FILINFO   fileInfo;
/* File function return code (FRESULT) */
FRESULT   res;
/* File read/write count*/
UINT      br, bw;
/*root*/
uint8_t filedir[]="0:/";
/*Max file number in root is 50(no long name <= 8 byte)*/
uint8_t Block_Buffer[512] = "FatFs is a generic FAT file system module for small embedded systems. The FatFs is written in compliance with ANSI C and completely separated from the disk I/O layer. Therefore it is independent of hardware architecture. It can be incorporated into low cost microcontrollers, such as AVR, 8051, PIC, ARM, Z80, 68k and etc..., without any change. \r\n ";                           

int main(void)
{
  u8  csd[16];
  u8  cid[16];
 
  GPIO_Configuration();
  USART1_Configuration();
 
  CANx_Config();
  CANx_NVIC_Config();
  /*SD card init*/
  while(SD_Initialize())
  {
    printf("Please insert SD card!\r\n");
    Delay_1us(1000000);
  }
  printf("SD card ready!\r\n");
 
  float sd_size;
  sd_size = (SD_GetSectorCount(csd)*512.0f)/1024.0f/1024.0f/1024.0f;
  printf("SD card size %f GB\r\n",sd_size);

  if(SD_GetCID(cid)!=0X01)
  {
    printf("Manufacturer ID     %d\r\n",cid[0]);
    printf("OEM/Application ID  %c%c\r\n",cid[1],cid[2]);
    printf("Product name        %c%c%c%c%c\r\n",cid[3],cid[4],cid[5],cid[6],cid[7]);
    printf("Manufacturing date  20%d/%d\r\n",((cid[15]&0xF0)>>4),(cid[15]&0x0F));
  }
  else printf("Wrong CID");
  /*SD card ready*/
  uint8_t res;
  /*FATFS init*/
  f_mount(0,&fs);
  res = f_mkdir("new");
  // printf("%d\r\n",res);
  //ls(&filedir);
  //ls_all(&filedir);      
  //read_file(&filedir); 
  // write_file();
  // f_mount(0,NULL);
  uint8_t retry;

  retry = 0xFF;
  do
  {
    res = f_open(&fsrc, "/new/data.txt", FA_CREATE_ALWAYS); 
    printf("%d\r\n",res); 
  }while(res&&retry--);
 
  retry = 0xFF;
  do
  {
    res = f_open(&fsrc, "/new/data.txt", FA_WRITE ); 
    printf("%d\r\n",res); 
  }while(res&&retry--);

  uint16_t count = 0;
  while (1)
  {
    // CANx_Transmit();

    // retry = 0xFF;
    // do
    // {
    res = f_printf(&fsrc,"12345 %d\r\n",count);  
    // }while(res&&retry--);

    // printf("%d\r\n",res);
    if (res != 255) GPIO_ToggleBits(LED4);
    else if (res == 255) GPIO_ResetBits(LED4);

    // count ++ ;
    // if(count>=10)
    // {
    //   retry = 0xFF;
 
      // res = f_sync(&fsrc);
     
    //   count = 0;     
    // }
    GPIO_ToggleBits(GPIOG,GPIO_Pin_2);
     
  }
  
}

/**
* @brief  Configure the IO Expander and the Touch Panel.
* @param  None
* @retval None
*/
#ifdef  USE_FULL_ASSERT

/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}
#endif

/**
  * @}
  */ 

/**
  * @}
  */ 

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
