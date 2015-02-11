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
#include "init_mcu.h"
#include "can.h"
/*fatfs*/
#include "fatfs_sd.h"
#include "ff.h"
#include "integer.h"
/*freertos*/
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

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
uint8_t Block_Buffer[5000]; 
uint8_t buffer[1024]="FatFs is a generic FAT file system module for small embedded systems.The FatFs is written in compliance with ANSI C and completely separated from the disk I/O layer.Therefore it is independent of hardware architecture.It can be incorporated into low cost microcontrollers,such as AVR, 8051, PIC, ARM, Z80, 68k ... without change.\r\n";                                                        

xQueueHandle t_queue; 
xQueueHandle t_mutex;

static inline void Delay_1us(uint32_t nCnt_1us)
{
  volatile uint32_t nCnt;

  for (; nCnt_1us != 0; nCnt_1us--)
    for (nCnt = 13; nCnt != 0; nCnt--);
}

uint8_t PushButton_Read(void){

    return GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_0);
}

static void Buttom_Task(void *pvParameters)
{
  while(1)
  {
    printf("task Buttom\r\n");
    vTaskDelay(100);
      // if(PushButton_Read()){
      //   GPIO_ToggleBits(LED3);
      //   Delay_1us(100000);
      // }else{

      //   GPIO_ResetBits(LED3);
      // }

  }
}

static void SD_Card_Task(void *pvParameters)
{
  // uint8_t  retry = 0xFF;
  // do{
  //   res = f_mount(&fs,"0:",1);
  // }while(res && --retry);
  // retry = 0xFF;
  // do{
  //   res = f_open(&fsrc, "data.txt", FA_CREATE_ALWAYS);
  // }while(res && --retry);
  // retry = 0xFF;
  // do{
  //   res = f_open(&fsrc, "data.txt", FA_WRITE );
  // }while(res && --retry);

  // uint8_t count = 0;
  while (1) {
    printf("task SD\r\n");
    vTaskDelay(100);
    //   GPIO_ToggleBits(LED4);
    //   do{
    //       res = f_write(&fsrc,&Block_Buffer,strlen((char*)&Block_Buffer),&bw);
    //       // printf("%d\r\n",res);
    //       if(res){
    //         GPIO_ToggleBits(LED3);
    //         break;
    //       }
    //   }
    //   while (bw < strlen((char*)&Block_Buffer));   
    //   count ++ ;
    //   if(count>=20){
    //   f_sync(&fsrc);    
    //   count = 0; 
    //   GPIO_ToggleBits(GPIOA,GPIO_Pin_2);   
    // }
  }
}

int main(void)
{

  GPIO_Configuration();
  USART1_Configuration();
  // Timer4_Initialization();
  STM_EVAL_PBInit( BUTTON_USER, BUTTON_MODE_GPIO );

  int x=0;
  for(x=0;x<15;x++)
  {
    strcat((char*)&Block_Buffer,(char*)&buffer);
  }
  printf("Buffer:%d bytes\r\n",strlen((char*)&Block_Buffer));

  xTaskCreate(SD_Card_Task, "SD Card Task", 256, 
          NULL, tskIDLE_PRIORITY + 1, NULL);
  xTaskCreate(Buttom_Task, "Buttom Task", 256, 
          NULL, tskIDLE_PRIORITY + 1, NULL);

 
  vTaskStartScheduler();
}

void TIM4_IRQHandler(void)
{
  if (TIM_GetITStatus(TIM4, TIM_IT_Update) != RESET)
  {

    TIM_ClearITPendingBit(TIM4, TIM_IT_Update);
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
