#include "freertos/FreeRTOS.h"
#include "httpd_files.h"
#include <string.h>
#include <stdio.h>

#include "httpdfiles/favicon.h"
#include "httpdfiles/style.css.h"
#include "httpdfiles/test.png.h"

#define HEAD "<html><head><link type=\"text/css\" rel=\"stylesheet\" "\
"href=\"style.css\" title=\"style\"/><meta http-equiv=\"Cache-Control\" "\
"content=\"no-cache\"/></head><body><div id=\"content\">"

int http_data_index(char **buffer, char *args);
int http_data_favicon(char **buffer, char *args);
int http_data_style_css(char **buffer, char *args);
int http_data_test_png(char **buffer, char *args);

struct httpd_data_struct http_data[] =
{
		{"/", http_data_index},
		{"/favicon.ico", http_data_favicon},
		{"/style.css", http_data_style_css},
		{"/test.png", http_data_test_png},
		{0, 0}
};

int http_data_index(char **buffer, char *args){
	int i=0;
	int len=0;
	*buffer = (char*)malloc(4096); //TODO size berechnen
	strcpy(*buffer, HEAD);
	sprintf(*buffer, "%s<div id=\"text\">", *buffer);
	while(http_data[i].fp != 0){
		sprintf(*buffer, "%s\r\n<a href=\"%s\">%s</a> - 0x%x<br>", *buffer, http_data[i].file, http_data[i].file, (int)http_data[i].fp);
		i++;
	}
	len = sprintf(*buffer, "%s</div></div></body></html>", *buffer);
	return len;
}

int http_data_favicon(char **buffer, char *args)
{
	*buffer = (char*) favicon;
	return sizeof(favicon) | NOFREE;
}

int http_data_style_css(char **buffer, char *args)
{
	*buffer = (char*) style_css;
	return sizeof(style_css) | NOFREE;
}

int http_data_test_png(char **buffer, char *args)
{
	*buffer = (char*) test_png;
	return sizeof(test_png) | NOFREE;
}
