#include "parse_url.h"
#include <string.h>
#include <regex.h>
#include <stdcountof.h>

int parse_url(const char* url, enum MODE* mode, char* hostname, char* path)
{
	if(strstr(url, "http:") == NULL)
	{
		*mode = https;
	}
	else
	{
		*mode = http;
	}

	regex_t r_host, r_path;
	regmatch_t p_host, p_path;

	if(regcomp(&r_host, "([a-z0-9-]+\\.)?[a-z0-9-]+\\.[a-z0-9-]+(\\.[a-z0-9-]+)?", 
				REG_ICASE | REG_EXTENDED))
	{
		return -1;
	}
	if(regexec(&r_host, url, 1, &p_host, 0))
	{
		return -1;
	}

	strncpy(hostname, url + p_host.rm_so, p_host.rm_eo - p_host.rm_so);
	
	strcpy(path, url + p_host.rm_eo);
	if(path[0] == '\0')
	{
		strcpy(path, "/");
	}

	return 0;
}

