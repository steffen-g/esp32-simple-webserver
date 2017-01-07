# esp32-simple-webserver

esp-idf/components/freertos/include/freertos/FreeRTOSConfig.h must be changed from

```
#define configUSE_TRACE_FACILITY		0		/* Used by vTaskList in main.c */
#define configUSE_STATS_FORMATTING_FUNCTIONS	0	/* Used by vTaskList in main.c */
```

to 
```
#define configUSE_TRACE_FACILITY		1		/* Used by vTaskList in main.c */
#define configUSE_STATS_FORMATTING_FUNCTIONS	1	/* Used by vTaskList in main.c */
```

to have vTaskList() in freertos.

The latest version of the wifi driver is not working properly with this example. 
So go back a few commits:
esp-idf/components/esp32/lib$ git checkout 02232f974b0ff1568ddd6d7015a41fb4f4870994
