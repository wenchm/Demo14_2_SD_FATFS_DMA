/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "dma.h"
#include "fatfs.h"
#include "rtc.h"
#include "sdio.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "keyled.h"
#include "file_opera.h"
#include <stdio.h>

//#include "diskio.h"
//#include "sd_diskio.c"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */
__IO uint8_t status = SD_PRESENT;
/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
BYTE workBuffer[4*BLOCKSIZE];	// Working cache area
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_RTC_Init();
  MX_USART6_UART_Init();
  MX_SDIO_SD_Init();
  MX_FATFS_Init();
  /* USER CODE BEGIN 2 */
  // Start Menu
  printf("Demo14_2: FatFS on SD card(DMA).\r\n\r\n");

  /**
   * @brief SD detect
   * @{
   */
  status = BSP_SD_IsDetected();
  if(status == SD_PRESENT)
  	printf("SD has been present.\r\n");
  else
  	printf("SD has been absent.\r\n");
  /**
   * @}
   */

  /**
   * @brief Mount the file system,register a volume.
   *
   * @brief Format	f_mkfs("0:",FM_FAT32,0,workBuffer,4*BLOCKSIZE)
   * @param path	"0:" first volume;"1:" second volume;""  default volume;
   * @param opt 	FM_FAT 0x01,FM_FAT32 0x02,FM_EXFAT 0x04,FM_ANY 0x07,FM_SFD 0x08
   * @param au 		The size of the cluster.Byte,Volumes are managed in clusters,
   * 				and clusters contain sectors, which are the smallest storage units.
   * @param work	work buffer,is a multiple of the cluster，pointer。
   * @param len		The size of	work buffer，Byte.
   * @{
   */
  retSD = f_mount(&SDFatFS,"0:",0);	// Mount the SD card file system
  if(retSD == FR_NO_FILESYSTEM)
  {
	  retSD=f_mkfs("0:",FM_FAT32,0,workBuffer,4*BLOCKSIZE);
	  if(retSD == FR_OK)
	  {
		  retSD = f_mount(NULL,"0:",0);		// Unmount the drive first
		  retSD = f_mount(&SDFatFS,"0:",0);	// Mount the SD card file system
	  }
  }

  // If Mounted Failure
  if(retSD != FR_OK)
	  printf("mounted, Failure.\r\n\r\n");
  /**
   * @}
   */

  //Menu Item 1
  //format/SD info/disk info
  printf("[S2]KeyUp   =Format SD card. \r\n");
  printf("[S1]KeyRight=SD card info. \r\n");
  printf("[S4]KeyLeft =FAT disk info. \r\n");
  printf("[S3]KeyDown =Next menu page. \r\n\r\n");

  KEYS waitKey;
  while(1)
  {
	  waitKey=ScanPressedKey(KEY_WAIT_ALWAYS); 	// Waiting for the button pressed

	  if(waitKey == KEY_UP) 					// KeyUp=Format SD card
	  {
		  BYTE workBuffer[4*BLOCKSIZE];   		// Working cache area
		  DWORD clusterSize=0;	 				// The cluster size must be greater than or equal to 1 sector, 0 is automatically set.
		  printf("\r\nFormating (10secs)...\r\n");
		  FRESULT res=f_mkfs("0:", FM_FAT32, clusterSize, workBuffer, 4*BLOCKSIZE); //FM_FAT32
		  if (res == FR_OK)
			  printf("Format OK, to reset.\r\n");
		  else
			  printf("Format fail, to reset.\r\n");
	  }
	  else if(waitKey == KEY_LEFT)  			//KeyLeft =FAT disk info
		  fatTest_GetDiskInfo();
	  else if (waitKey == KEY_RIGHT)  			//KeyRight=SD card info"
		  SDCard_ShowInfo();
	  else
		  break;								//turn into the next menu2

	  printf("Reselect menu item or reset.\r\n\r\n");
	  HAL_Delay(500);							//Delay, eliminate the impact of key jitter
  }

  printf("\r\n");
  //Menu Item 2
  //test read/write SD with FATFS universal function
  printf("test read/write SD with FATFS universal function.\r\n");
  printf("[S2]KeyUp   =Write files.\r\n");
  printf("[S3]KeyDown =Get a file info.\r\n");
  printf("[S4]KeyLeft =Read a TXT file and a BIN file.\r\n");
  printf("[S1]KeyRight=Next menu page.\r\n\r\n");

  HAL_Delay(500);

  /* Menu2,use FATFS function operates files,such as 'f_'.
   * For example, These functions with "f_" as prefix,
   * are located under the \Middlewares\Third_Party\FatFs\src\.
   * These functions exist as long as FATFS is turned on,
   * but they have nothing to do with DMA.
   * Whether DMA is enabled or not, these functions are implemented the same way.
   *
   */
  while(2)
  {
	  waitKey=ScanPressedKey(KEY_WAIT_ALWAYS);

	  //Create a file with the following text and name it.
	  if (waitKey==KEY_UP )
	  {
		  fatTest_WriteTXTFile("DMA_readme.txt",2025,07,14);
		  fatTest_WriteTXTFile("DMA_help.txt",  2025,07,14);
		  fatTest_WriteBinFile("DMA_ADC2000.dat",30, 2000);
		  fatTest_WriteBinFile("DMA_ADC1000.dat",100,1000);
		  f_mkdir("0:/SubDirectory");
		  f_mkdir("0:/Documents");
	  }
	  else if (waitKey==KEY_DOWN)
	  {
	  	  fatTest_GetFileInfo("DMA_ADC1000.dat");
	  }
	  else if (waitKey==KEY_LEFT )
	  {
		  printf("Read a TXT file:\r\n");
		  fatTest_ReadTXTFile("DMA_readme.txt");
		  printf("Read a BIN file:\r\n");
	  	  fatTest_ReadBinFile("DMA_ADC2000.dat");
	  }
	  else if (waitKey==KEY_RIGHT)
		  break;		//turn into the next menu3

	  printf("Reselect menu item or reset.\r\n\r\n");

	  HAL_Delay(500);
  }


  printf("\r\n");
  //Menu Item 3
  //test erase/sd info/read/write with specialized SD_Function
  printf("test erase/SD info/read/write with specialized SD_Function.\r\n");
  printf("[S2]KeyUp   =SD card info. \r\n");
  printf("[S3]KeyDown =Erase 0-10 blocks. \r\n");
  printf("[S4]KeyLeft =Write block. \r\n");
  printf("[S1]KeyRight=Read block. \r\n\r\n");

  HAL_Delay(500);

  /* Menu 3,use FATFS function operates files,such as 'SD_'.
   * For example, These functions with "SD_" as prefix,
   * are located under the \FATFS\Target\.
   * These functions exist as long as FATFS is turned on,
   * but they are related to DMA.
   * Whether DMA is enabled or not, these functions are implemented different way.
   *
   */
  while(3)
  {
	  waitKey=ScanPressedKey(KEY_WAIT_ALWAYS);

	  if (waitKey==KEY_UP)
	  {
		  SDCard_ShowInfo();
		  //printf("Reselect menu item or reset. \r\n");
	  }
	  else if (waitKey== KEY_DOWN)
	  {
		  SDCard_EraseBlocks();			//EraseBlocks 0-10
		  //printf("Reselect menu item or reset. \r\n");
	  }
	  else if (waitKey== KEY_LEFT)
		  SDCard_TestWrite_DMA();
	  else if (waitKey== KEY_RIGHT)
		  SDCard_TestRead_DMA();

	  HAL_Delay(500);
	  printf("Reselect menu item or reset.\r\n\r\n");
  }
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_LSI|RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.LSIState = RCC_LSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 15;
  RCC_OscInitStruct.PLL.PLLN = 144;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 5;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_3) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

/* Show SD Information */
void SDCard_ShowInfo()
{
	HAL_SD_CardInfoTypeDef cardInfo;
	HAL_StatusTypeDef res=HAL_SD_GetCardInfo(&hsd,&cardInfo);

	if (res!=HAL_OK)
	{
		printf("HAL_SD_GetCardInfo() error. \r\n");
		return;
	}

	printf("\r\n*** SD card info *** \r\n\r\n");

	printf("Card Type= %ld \r\n", cardInfo.CardType);
	printf("Card Version= %ld \r\n", cardInfo.CardVersion);
	printf("Card Class= %ld \r\n", cardInfo.Class);
	printf("Relative Card Address= %ld \r\n", cardInfo.RelCardAdd);
	printf("Block Count= %ld \r\n", cardInfo.BlockNbr);
	printf("Block Size(Bytes)= %ld \r\n", cardInfo.BlockSize);
	printf("LogiBlockCount= %ld \r\n", cardInfo.LogBlockNbr);
	printf("LogiBlockSize(Bytes)= %ld \r\n", cardInfo.LogBlockSize);

	uint32_t cap = cardInfo.BlockNbr/1024;	//KB
	cap	= cap*cardInfo.BlockSize;			//KB
	cap = cap/1024;							//MB

	printf("SD Card Capacity(MB)= %ld \r\n\r\n", cap);
	//return HAL_OK;
}

int __io_putchar(int ch)
{
	HAL_UART_Transmit(&huart6,(uint8_t*)&ch,1,0xFFFF);
	return ch;
}

//SD card detect
//Low level indicates that a card is inserted
uint8_t BSP_SD_IsDetected(void)
{
   if(HAL_GPIO_ReadPin(SD_DETECT_GPIO_PORT, SD_DETECT_PIN) == GPIO_PIN_RESET)
  	 status = SD_PRESENT;
   else
  	 {
	   status = SD_NOT_PRESENT;
  	 }
   return status;
}
/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}
#ifdef USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
