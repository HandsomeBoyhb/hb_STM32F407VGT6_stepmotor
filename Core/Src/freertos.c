/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
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
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"


/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

#include "config.h"
#include "semphr.h"
#include <stdlib.h>
#include <math.h>
#include "stm32f4xx_hal.h"
#include <string.h>
#include <stm32f4xx_it.h>
#include "Emm_V5.h"
#include "usart.h"

// 为每个串口创建独立的接收资源
// USART2 
extern SemaphoreHandle_t uart2_rx_sem;
extern uint8_t uart2_rx_buffer[UART_RX_BUFFER_SIZE];
extern volatile uint16_t uart2_rx_len;

// USART3 
extern SemaphoreHandle_t uart3_rx_sem;
extern uint8_t uart3_rx_buffer[UART_RX_BUFFER_SIZE];
extern volatile uint16_t uart3_rx_len;

//USART6
extern SemaphoreHandle_t uart6_rx_sem;
extern uint8_t uart6_rx_buffer[UART_RX_BUFFER_SIZE];
extern volatile uint16_t uart6_rx_len;

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */

extern UART_HandleTypeDef huart2;
extern UART_HandleTypeDef huart3;

/* USER CODE END Variables */
/* Definitions for defaultTask */
osThreadId_t defaultTaskHandle;
const osThreadAttr_t defaultTask_attributes = {
  .name = "defaultTask",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

void StartDefaultTask(void *argument);

//void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
//void MX_FREERTOS_Init(void) {
//  /* USER CODE BEGIN Init */

//  /* USER CODE END Init */

//  /* USER CODE BEGIN RTOS_MUTEX */
//  /* add mutexes, ... */
//  /* USER CODE END RTOS_MUTEX */

//  /* USER CODE BEGIN RTOS_SEMAPHORES */
//  /* add semaphores, ... */
//  /* USER CODE END RTOS_SEMAPHORES */

//  /* USER CODE BEGIN RTOS_TIMERS */
//  /* start timers, add new ones, ... */
//  /* USER CODE END RTOS_TIMERS */

//  /* USER CODE BEGIN RTOS_QUEUES */
//  /* add queues, ... */
//  /* USER CODE END RTOS_QUEUES */

//  /* Create the thread(s) */
//  /* creation of defaultTask */
//  defaultTaskHandle = osThreadNew(StartDefaultTask, NULL, &defaultTask_attributes);

//  /* USER CODE BEGIN RTOS_THREADS */
//  /* add threads, ... */
//  /* USER CODE END RTOS_THREADS */

//  /* USER CODE BEGIN RTOS_EVENTS */
//  /* add events, ... */
//  /* USER CODE END RTOS_EVENTS */

//}

/* USER CODE BEGIN Header_StartDefaultTask */
/**
  * @brief  Function implementing the defaultTask thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_StartDefaultTask */
void StartDefaultTask(void *argument)
{
  /* USER CODE BEGIN StartDefaultTask */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END StartDefaultTask */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

// 电机控制函数（非阻塞，带信号量）
void SendToLeftMotor(uint8_t dir, uint16_t vel) {
    // 反转左轮方向
    uint8_t corrected_dir = (dir == 0x00) ? 0x01 : 0x00;
    
    // 发送命令到左电机（USART3）
    Emm_V5_Vel_Control_ZUO(0x01, corrected_dir, vel, 0, false);
}

void SendToRightMotor(uint8_t dir, uint16_t vel) {
    // 反转右轮方向
    uint8_t corrected_dir = (dir == 0x00) ? 0x01 : 0x00;
    
    // 发送命令到右电机（USART2）
    Emm_V5_Vel_Control_YOU(0x01, corrected_dir, vel, 0, false);
}

void ProcessCommand(uint8_t *buffer, uint16_t len) {
    for (int start = 0; start <= len - 6; start++) {
        if (buffer[start] == 0xAA && buffer[start + 5] == 0x55) {
            uint8_t right_dir = buffer[start + 1];      // 右轮方向 
            uint8_t right_speed_val = buffer[start + 2];// 右轮速度值
            uint8_t left_dir = buffer[start + 3];       // 左轮方向 
            uint8_t left_speed_val = buffer[start + 4]; // 左轮速度值
            
            // 验证方向有效性 (0x00=前进, 0x01=后退)
            if ((right_dir != 0x00 && right_dir != 0x01) ||
                (left_dir != 0x00 && left_dir != 0x01)) 
            {
                continue;
            }
            
            // 反转右轮方向
            uint8_t corrected_right_dir = (right_dir == 0x00) ? 0x01 : 0x00;
            
            // 控制左电机 (地址0x01)
            SendToLeftMotor(left_dir, left_speed_val);
            
            // 控制右电机 (地址0x01) - 使用修正后的方向
            SendToRightMotor(corrected_right_dir, right_speed_val);
        }
    }
}

// USART6 上位机数据处理任务
void Uart6CommandTask(void *argument) {
    uint8_t local_buffer[UART_RX_BUFFER_SIZE];
    uint16_t local_len;
    
    while(1) {
        if (xSemaphoreTake(uart6_rx_sem, portMAX_DELAY) == pdTRUE) {
            // 1. 复制接收数据到本地缓冲区
            local_len = uart6_rx_len;
            memcpy(local_buffer, uart6_rx_buffer, local_len);
            
            // 2. 重置接收状态
            uart6_rx_len = 0;
            HAL_UART_Receive_IT(&huart6, uart6_rx_buffer, UART_RX_BUFFER_SIZE);
            
            // 3. 处理复制的数据
            if (local_len >= 6) {
                ProcessCommand(local_buffer, local_len);
            }
        }
    }
}

void StartMovement(void) {
	
	// 创建任务
     xTaskCreate(Uart6CommandTask, "Uart6Cmd", 1024, NULL, 5, NULL);
    
}

void MX_FREERTOS_Init(void) {
    
	StartMovement();
	
}

/* USER CODE END Application */

