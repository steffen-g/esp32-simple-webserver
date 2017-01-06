#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "lwip/sockets.h"
#include <string.h>
#include "main.h"
#include "httpd_files.h"

//#define DEBUG
#ifdef DEBUG
        #define debug(fmt, args...)     printf("%s %s: %d - "fmt, __FILE__, __FUNCTION__, __LINE__, ##args);
#else
        #define debug(fmt, args...)
#endif

#define HEADER_OK "HTTP/1.1 200 OK\r\n\r\n"
#define HEADER_OK_SVG "HTTP/1.1 200 OK\r\nContent-type: image/svg+xml;\r\n\r\n"
#define HEADER_OK_PNG "HTTP/1.1 200 OK\r\nContent-type: image/png;\r\n\r\n"
#define HEADER_OK_ICO "HTTP/1.1 200 OK\r\nContent-type: image/x-icon;\r\n\r\n"
#define HEADER_OK_CSS "HTTP/1.1 200 OK\r\nContent-type: text/css;\r\n\r\n"
#define HEADER_NOTFOUND "HTTP/1.1 404 Not Found\r\n\r\n"
#define TEXT_NOTFOUND "404 Not Found"

#define TIMEOUT		3000
#define REQBUFFER	(2*1024)
void tsk_httpd_child(void *pvParameter){
	char buffer[32];
	char *req, *path, *txbuf;
	int n, pos=0, receiving=1, i;
	int newsockfd = (int)pvParameter;
	TickType_t xLastWakeTime;
	debug("created httpd child for %x on core %d\n", (int)pvParameter, xPortGetCoreID());
	
	req = malloc(REQBUFFER);
	xLastWakeTime = xTaskGetTickCount();
	while(receiving){ 
		n = recv(newsockfd, buffer, sizeof(buffer), MSG_DONTWAIT);
		if(n > 0){
			//debug("Received %d bytes\n", n);
			memcpy((void*)req+pos, buffer, n);
			pos += n;
			if(pos > REQBUFFER) pos = 0;
			//debug("request: \"%s\"\npos %d\n", req, pos);
			if(!strncmp("\r\n\r\n", req+pos-4, 4)) receiving = 0;
		}
		
		if(xTaskGetTickCount() > TIMEOUT/portTICK_PERIOD_MS + xLastWakeTime){
			debug("timeout\n");
			goto exit;
		}
	}
	req[pos-4] = 0;
	path = strtok(req+4, " ");
	char *file = strtok(path, "?");
	char *end = file + strlen(file) - 3;
	path = strtok(NULL, "");
	
/*	debug("Requested \n");
	if(file) debug("file: %s \n", file);
	if(end) debug("end: %s \n", end);
	if(path) debug("path: %s\n", path);
*/	
	i=0;
	while(http_data[i].fp != 0)
	{
		if(!strcmp(file, http_data[i].file)){
			if(!strncmp(end, "svg", 3))
				write(newsockfd, HEADER_OK_SVG, strlen(HEADER_OK_SVG));
			else if(!strncmp(end, "png", 3))
				write(newsockfd, HEADER_OK_PNG, strlen(HEADER_OK_PNG));
			else if(!strncmp(end, "ico", 3))
				write(newsockfd, HEADER_OK_ICO, strlen(HEADER_OK_ICO));
			else if(!strncmp(end, "css", 3))
				write(newsockfd, HEADER_OK_CSS, strlen(HEADER_OK_CSS));
			else
				write(newsockfd, HEADER_OK, strlen(HEADER_OK));
				
			n = http_data[i].fp(&txbuf, path);
			//debug("file found, returned %d bytes:\n%s\n", n, txbuf);
			write(newsockfd, txbuf, n & ~NOFREE);
			if(!(n & NOFREE)) free(txbuf);
			goto exit;
		}
		i++;
	}
	
	printf("file %s not found\n", file);
	write(newsockfd, HEADER_NOTFOUND, sizeof(HEADER_NOTFOUND));
	write(newsockfd, TEXT_NOTFOUND, sizeof(TEXT_NOTFOUND));
	
exit:
	free(req);
	close(newsockfd);
	vTaskDelete(NULL);
}

void tsk_httpd(void *pvParameter){

	int clilen, newsockfd, sockfd = socket(AF_INET, SOCK_STREAM, 0);
	struct sockaddr_in addr, cli_addr;
	char buffer[16];
	int clientno=0;
	
	if (sockfd < 0) {
		printf("ERROR opening socket");
		while(1);
	}
	
	debug("wait for connection\n");
    xEventGroupWaitBits(wifi_event_group, CONNECTED_BIT, false, true, portMAX_DELAY);
	debug("connected\n");
	
	addr.sin_family = AF_INET;
	addr.sin_port = htons(80);
	addr.sin_addr.s_addr = htonl(INADDR_ANY);
	if (bind(sockfd, (struct sockaddr *) &addr, sizeof(addr)) < 0) {
		printf("ERROR on binding");
		while(1);
	}

	listen(sockfd,5);
	clilen = sizeof(cli_addr);
	while(1){
		/* Accept actual connection from the client */
		newsockfd = accept(sockfd, (struct sockaddr *)&cli_addr, (socklen_t*) &clilen);
		if (newsockfd < 0) {
			printf("ERROR on accept");
			while(1);
		}
		
		sprintf(buffer, "httpd_%x", clientno++);
		debug("someone connected socket: %x\n", newsockfd);
		xTaskCreatePinnedToCore(&tsk_httpd_child, buffer, 2048, (void*)newsockfd, 5, NULL, tskNO_AFFINITY);
	}
}
