/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2024 STMicroelectronics.
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
#include "fatfs.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "ili9341.h"
#include "Bitmaps.h"
#include <string.h>
#include <stdio.h>
#include "fatfs_sd.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
//SPI_HandleTypeDef hspi1;
FATFS fs;
FATFS *pfs;
FIL fil;
FRESULT fres;
DWORD fre_clust;
uint32_t totalSpace, freeSpace;
char buffer[100];
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
I2C_HandleTypeDef hi2c1;

SPI_HandleTypeDef hspi1;

UART_HandleTypeDef huart2;
UART_HandleTypeDef huart3;

/* USER CODE BEGIN PV */
//Pantalla
extern uint16_t fondo[];
uint8_t Readsens = 0;

//sensor I2c
uint32_t tiempoAnterior = 0;
float temperatura = 30;
uint8_t medicionValida = 0;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_SPI1_Init(void);
static void MX_I2C1_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_USART3_UART_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
void UART_Print(char *mensaje)
{
    HAL_UART_Transmit(&huart2, (uint8_t*) mensaje, strlen(mensaje), HAL_MAX_DELAY);
}

#define LM75_ADDR (0x48 << 1)

	HAL_StatusTypeDef LeerTemperatura(float *temperatura)
	{
		uint8_t datos[2];

		HAL_StatusTypeDef estado = HAL_I2C_Mem_Read(&hi2c1, LM75_ADDR, 0x00, I2C_MEMADD_SIZE_8BIT, datos, 2, 100);

		if (estado != HAL_OK)
			return estado;

		int16_t valor = (int16_t)((datos[0] << 8) | datos[1]);
		*temperatura = (valor >> 7) * 0.5f;

		return HAL_OK;
	}



	FRESULT GuardarTemperatura(float temperatura)
	{
	    FIL archivo;
	    char texto[50];

	    // Abrir CSV sin borrar datos anteriores
	    FRESULT resultado = f_open(&archivo, "TEMP.CSV",
	                               FA_OPEN_APPEND | FA_WRITE);

	    if (resultado != FR_OK)
	    {
	        return resultado;
	    }

	    // Crear encabezado si el archivo esta vacio
	    if (f_size(&archivo) == 0)
	    {
	        if (f_puts("Tiempo_s,Temperatura_C\n", &archivo) < 0)
	        {
	            f_close(&archivo);
	            return FR_DISK_ERR;
	        }
	    }

	    // Preparar nueva medicion
	    sprintf(texto, "%lu,%.1f\n",
	            (unsigned long)(HAL_GetTick() / 1000), temperatura);

	    // Guardar nueva fila
	    if (f_puts(texto, &archivo) < 0)
	    {
	        f_close(&archivo);
	        return FR_DISK_ERR;
	    }

	    // Cerrar archivo y devolver resultado
	    return f_close(&archivo);
	}


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
  MX_SPI1_Init();
  MX_I2C1_Init();
  MX_USART2_UART_Init();
  MX_USART3_UART_Init();
  MX_FATFS_Init();
  /* USER CODE BEGIN 2 */


  fres = f_mount(&fs, "/", 0);

  if (fres == FR_OK) {
      UART_Print("Micro SD card is mounted successfully!\n");
  }
  else if (fres != FR_OK) {
      UART_Print("Micro SD card's mount error!\n");
  }



//  // Paso 1: Montar microSD
//  fres = f_mount(&fs, "/", 0);
//
//  // Paso 2: Abrir archivo
//  fres = f_open(&fil, "Prueba.txt",
//                FA_OPEN_APPEND | FA_WRITE | FA_READ);
//
//  char mensaje[50];
//
//  sprintf(mensaje, "f_open result: %d\r\n", fres);
//  UART_Print(mensaje);
//
//  // Paso 3: Leer contenido del archivo
//  while (f_gets(buffer, sizeof(buffer), &fil))
//  {
//      char mRd[100];
//      sprintf(mRd, "%s", buffer);
//      UART_Print(mRd);
//  }
//
//  // Paso 4: Escribir contenido en el archivo
//  for (uint8_t i = 0; i < 10; i++)
//  {
//      f_puts("Hola Mundo.\n", &fil);
//  }
//
//  //Paso 5
//  fres = f_close(&fil);
//
//  if (fres == FR_OK) {
//      UART_Print("The file is closed.\n");
//  }
//  else if (fres != FR_OK) {
//      UART_Print("The file was not closed.\n");
//  }
//
// //Paso 6
//  fres = f_mount(NULL, "", 1);
//
//  if (fres == FR_OK) {
//      UART_Print("The Micro SD card is unmounted!\n");
//  }
//  else if (fres != FR_OK) {
//      UART_Print("The Micro SD was not unmounted!\n");
//  }



  LCD_Init();

  //fondo azul oscuro
  LCD_Clear(0x1082);

  //barra superior
  FillRect(0, 0, 320, 40, 0x29A7);

  //panel principal de temperatura
  FillRect(15, 55, 290, 100, 0x2104);

  //panel inferior de estado
  FillRect(15, 165, 290, 70, 0x2104);


  //titulo principal
  LCD_Print("MONITOR DE TEMPERATURA", 12, 12, 1, 0xFFFF, 0x29A7);

  //Nombre
  LCD_Print("TEMPERATURA CORPORAL", 25, 65, 1, 0xFFFF, 0x2104);

  //valor temp
  LCD_Print("36.8", 75, 100, 2, 0xFFFF, 0x2104);

  //unidad
  LCD_Print("C", 165, 100, 2, 0x07FF, 0x2104);

  // simboloLo°
  FillRect(153, 98, 4, 2, 0x07FF);   // Arriba
  FillRect(153, 104, 4, 2, 0x07FF);  // Abajo
  FillRect(151, 100, 2, 4, 0x07FF);  // Izquierda
  FillRect(157, 100, 2, 4, 0x07FF);  // Derecha

  //estado
  LCD_Print("ESTADO: NORMAL", 30, 174, 1, 0x07E0, 0x2104);

  //estado de temperatura
  FillRect(30, 198, 85, 10, 0x001F);   // Baja
  FillRect(115, 198, 85, 10, 0x07E0);  // Normal
  FillRect(200, 198, 85, 10, 0xF800);  // Alta

  LCD_Print("BAJA", 56, 216, 1, 0xFFFF, 0x2104);
  LCD_Print("NORMAL", 133, 216, 1, 0xFFFF, 0x2104);
  LCD_Print("ALTA", 226, 216, 1, 0xFFFF, 0x2104);

  //temperatura actual
//  FillRect(155, 195, 5, 16, 0xFFFF);

  LCD_Print("SIMULACION:", 221, 12, 1, 0xFFE0, 0x29A7);

  //imagen paciente
  LCD_Bitmap(219, 70, 64, 64, paciente);
//tal vez 235



  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
	while (1) {

//
//		if (HAL_I2C_IsDeviceReady(&hi2c1, LM75_ADDR, 3, 100) == HAL_OK)
//		{
//		    UART_Print("LM75 detectado\r\n");
//		}
//		else
//		{
//		    UART_Print("LM75 no detectado\r\n");
//		}



		//Leer temperatura cada segundo
		if (HAL_GetTick() - tiempoAnterior >= 1000) {

		    tiempoAnterior = HAL_GetTick();

		    if (LeerTemperatura(&temperatura) == HAL_OK) {

		        char texto[20];

		        sprintf(texto, "%.1f", temperatura);

		        //eliminar temperatura anterior
		        FillRect(75, 100, 75, 32, 0x2104);

		        //actualizar  temperatura
		        LCD_Print(texto, 75, 100, 2, 0xFFFF, 0x2104);

		        //eliminar estado anterior
		        FillRect(30, 174, 260, 16, 0x2104);

		        //eliminar indicador anterior
		        FillRect(30, 195, 255, 16, 0x2104);

		        //redibujar barra de temperatura
		        FillRect(30, 198, 85, 10, 0x001F);
		        FillRect(115, 198, 85, 10, 0x07E0);
		        FillRect(200, 198, 85, 10, 0xF800);

		        //evaluar estado del paciente
		        if (temperatura < 35.0f) {

		            LCD_Print("ESTADO: HIPOTERMIA", 30, 174, 1, 0x001F, 0x2104);

		            LCD_Bitmap(219, 70, 64, 64, PacienteCold);

		            FillRect(70, 195, 5, 16, 0xFFFF);

		        }
		        else if (temperatura < 37.5f) {

		            LCD_Print("ESTADO: NORMAL", 30, 174, 1, 0x07E0, 0x2104);

		            LCD_Bitmap(219, 70, 64, 64, paciente);

		            FillRect(155, 195, 5, 16, 0xFFFF);

		        }
		        else {

		            LCD_Print("ESTADO: FIEBRE", 30, 174, 1, 0xF800, 0x2104);

		            LCD_Bitmap(219, 70, 64, 64, pacienteHot);

		            FillRect(240, 195, 5, 16, 0xFFFF);

		        }
		    }
		}




		if (HAL_UART_Receive(&huart3, &Readsens, 1, 10) == HAL_OK) {

		    if (Readsens == 1) {

		        // Enviar temperatura a Nucleo 1
		        HAL_UART_Transmit(&huart3, (uint8_t*)&temperatura,sizeof(temperatura), 100);
		    }

		    else if (Readsens == 2) {

		        uint8_t respuesta;

		        // Intentar guardar temperatura
		        if (GuardarTemperatura(temperatura) == FR_OK) {

		            respuesta = 3; // Guardado exitoso

		        }
		        else {

		            respuesta = 4; // Error al guardar

		        }

		        // Enviar resultado a Nucleo 1
		        HAL_UART_Transmit(&huart3, &respuesta, 1, 100);

		    }
		}


//
//		Readsens = '1';  // Simulate receiving the character '1'
//
//		if (Readsens == '1')
//		{
//		    float temperatura;
//
//		    if (LeerTemperatura(&temperatura) == HAL_OK)
//		    {
//		        char texto[20];
//
//		        // Convert temperature to text for TFT
//		        sprintf(texto, "%.1f", temperatura);
//
//		        // Update TFT
//		        FillRect(75, 100, 75, 32, 0x2104);
//		        LCD_Print(texto, 75, 100, 2, 0xFFFF, 0x2104);
//
//		        // Send temperature to Nucleo 1 through UART3
//		        HAL_UART_Transmit(&huart3,
//		                          (uint8_t*)&temperatura,
//		                          sizeof(temperatura),
//		                          100);
//
//		        // Debug confirmation through USART2
//		        UART_Print("Temperatura leida y enviada\r\n");
//		    }
//		    else
//		    {
//		        UART_Print("ERROR LM75\r\n");
//		    }
//		}
//
//		HAL_Delay(1000);
//
	}



    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */





	}

  /* USER CODE END 3 */


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
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE3);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 8;
  RCC_OscInitStruct.PLL.PLLN = 80;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 2;
  RCC_OscInitStruct.PLL.PLLR = 2;
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
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief I2C1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C1_Init(void)
{

  /* USER CODE BEGIN I2C1_Init 0 */

  /* USER CODE END I2C1_Init 0 */

  /* USER CODE BEGIN I2C1_Init 1 */

  /* USER CODE END I2C1_Init 1 */
  hi2c1.Instance = I2C1;
  hi2c1.Init.ClockSpeed = 100000;
  hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
  hi2c1.Init.OwnAddress1 = 0;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C1_Init 2 */

  /* USER CODE END I2C1_Init 2 */

}

/**
  * @brief SPI1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_SPI1_Init(void)
{

  /* USER CODE BEGIN SPI1_Init 0 */

  /* USER CODE END SPI1_Init 0 */

  /* USER CODE BEGIN SPI1_Init 1 */

  /* USER CODE END SPI1_Init 1 */
  /* SPI1 parameter configuration*/
  hspi1.Instance = SPI1;
  hspi1.Init.Mode = SPI_MODE_MASTER;
  hspi1.Init.Direction = SPI_DIRECTION_2LINES;
  hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi1.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi1.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi1.Init.NSS = SPI_NSS_SOFT;
  hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_8;
  hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi1.Init.CRCPolynomial = 10;
  if (HAL_SPI_Init(&hspi1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SPI1_Init 2 */

  /* USER CODE END SPI1_Init 2 */

}

/**
  * @brief USART2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART2_UART_Init(void)
{

  /* USER CODE BEGIN USART2_Init 0 */

  /* USER CODE END USART2_Init 0 */

  /* USER CODE BEGIN USART2_Init 1 */

  /* USER CODE END USART2_Init 1 */
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 115200;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART2_Init 2 */

  /* USER CODE END USART2_Init 2 */

}

/**
  * @brief USART3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART3_UART_Init(void)
{

  /* USER CODE BEGIN USART3_Init 0 */

  /* USER CODE END USART3_Init 0 */

  /* USER CODE BEGIN USART3_Init 1 */

  /* USER CODE END USART3_Init 1 */
  huart3.Instance = USART3;
  huart3.Init.BaudRate = 115200;
  huart3.Init.WordLength = UART_WORDLENGTH_8B;
  huart3.Init.StopBits = UART_STOPBITS_1;
  huart3.Init.Parity = UART_PARITY_NONE;
  huart3.Init.Mode = UART_MODE_TX_RX;
  huart3.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart3.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart3) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART3_Init 2 */

  /* USER CODE END USART3_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  /* USER CODE BEGIN MX_GPIO_Init_1 */
  /* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, LCD_RST_Pin|LCD_D1_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, LCD_RD_Pin|LCD_WR_Pin|LCD_RS_Pin|LCD_D7_Pin
                          |LCD_D0_Pin|LCD_D2_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, LCD_CS_Pin|LCD_D6_Pin|LCD_D3_Pin|LCD_D5_Pin
                          |LCD_D4_Pin|SD_SS_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pins : LCD_RST_Pin LCD_D1_Pin */
  GPIO_InitStruct.Pin = LCD_RST_Pin|LCD_D1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pins : LCD_RD_Pin LCD_WR_Pin LCD_RS_Pin LCD_D7_Pin
                           LCD_D0_Pin LCD_D2_Pin */
  GPIO_InitStruct.Pin = LCD_RD_Pin|LCD_WR_Pin|LCD_RS_Pin|LCD_D7_Pin
                          |LCD_D0_Pin|LCD_D2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : LCD_CS_Pin LCD_D6_Pin LCD_D3_Pin LCD_D5_Pin
                           LCD_D4_Pin SD_SS_Pin */
  GPIO_InitStruct.Pin = LCD_CS_Pin|LCD_D6_Pin|LCD_D3_Pin|LCD_D5_Pin
                          |LCD_D4_Pin|SD_SS_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /* USER CODE BEGIN MX_GPIO_Init_2 */
  /* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */

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
	while (1) {
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
