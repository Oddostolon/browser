#include "parse_url.h"
#include <stdio.h>
#include <string.h>

void parse_url(const char* url, size_t len, char* hostname, char* path)
{
	const char* hostname_start = strstr(url, "://"); 
	
	if (hostname_start == NULL)
	{
		hostname_start = url;
	}
	else
	{
		hostname_start += 3;
	}

	// browser only supports http, exit if url starts with https
	if((hostname_start - url) > 7)
	{
		hostname = NULL;
		path = NULL;
		return;
	}

	const char* path_start = strstr(hostname_start, "/");
	
	strncpy(hostname, hostname_start, path_start - hostname_start);
	strcpy(path, path_start);
}

