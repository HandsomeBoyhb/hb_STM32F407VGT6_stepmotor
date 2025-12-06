#include "main.h"
#include "FreeRTOS.h"
#include "task.h"
#include "stdio.h"
#include "cmsis_os.h"
#include "semphr.h"

extern UART_HandleTypeDef huart3;

#ifdef __GNUC__
    #define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
    int __io_putchar(int ch);  // 声明 __io_putchar
#else
    #define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
    int fputc(int ch, FILE *f);  // 声明 fputc，参数类型为 FILE*
#endif

// 添加串口接收相关定义
#define UART_RX_BUFFER_SIZE 64
extern SemaphoreHandle_t uart_rx_sem;
extern uint8_t uart_rx_buffer[UART_RX_BUFFER_SIZE];
extern volatile uint16_t uart_rx_len;	

