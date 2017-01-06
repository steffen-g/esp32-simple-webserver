#include "freertos/FreeRTOS.h"
#include "driver/uart.h"
#include "esp_log.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include <string.h>

#define BUF_SIZE 128

//#define DEBUG
#ifdef DEBUG
        #define debug(fmt, args...)     printf("%s %s: %d - "fmt, __FILE__, __FUNCTION__, __LINE__, ##args);
#else
        #define debug(fmt, args...)
#endif


QueueHandle_t uart0_queue;
void tsk_uart0(void *pvParameters){
    int uart_num = UART_NUM_0;
    uart_config_t uart_config = {
       .baud_rate = 115200,
       .data_bits = UART_DATA_8_BITS,
       .parity = UART_PARITY_DISABLE,
       .stop_bits = UART_STOP_BITS_1,
       .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
       .rx_flow_ctrl_thresh = 122,
    };
    uart_param_config(uart_num, &uart_config);
    uart_driver_install(uart_num, BUF_SIZE * 2, BUF_SIZE * 2, 10, &uart0_queue, 0);

    char* data = (char*) malloc(BUF_SIZE);
    char* buf = (char*) malloc(BUF_SIZE);
    int pos = 0;
    while(1){
        int len = uart_read_bytes(uart_num, (uint8_t*) data, BUF_SIZE, 100 / portTICK_RATE_MS);
        if(len > 0) {
            uart_write_bytes(uart_num, (const char*)data, len);
            memcpy((void*)buf + pos, data, len);
            pos += len;
            //debug("buf %s, pos %d, buf[pos-1] %x\n", buf, pos, buf[pos-1]);
            if(pos >= BUF_SIZE) pos = 0;
            if(buf[pos-1] == '\n'){
				buf[pos-2] = 0;
				debug("cmd: \"%s\" core: %d\n", buf, xPortGetCoreID());
				if(!strcmp("tasks", buf)){
					char *tmp = malloc(1024);
					vTaskList(tmp);
					printf("Task\t\tState\tPrio\tStack\tNum\r\n%s\r\n", tmp);
					free(tmp);
				}else if(!strcmp("free", buf)){
					printf("Free heap: %d \n", xPortGetFreeHeapSize());
				}
				pos = 0;
			}
        }
    }
    free(data);
    free(buf);
}
