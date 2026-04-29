#include <stddef.h>

enum MODE 
{
	http,
	https
};

int parse_url(const char* url, enum MODE* mode, char* hostname, char* path);
