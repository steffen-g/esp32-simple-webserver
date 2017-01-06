#ifndef HTTPD_FILES_H_
#define HTTPD_FILES_H_

struct httpd_data_struct
{
	const char *file;
	int (*fp)(char**, char*);
};

extern struct httpd_data_struct http_data[];

#define NOFREE (1<<31)

#endif /* HTTPD_FILES_H_ */
