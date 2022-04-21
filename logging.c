#include<stdio.h>
#include<time.h>
#include"logging.h"

time_t logging_initial_time;
FILE *logging_file_handle;

int init_logger(const char *path)
{
    logging_initial_time = time(NULL);
    logging_file_handle = fopen(path, "a");
    return logging_file_handle == NULL;
}

void infolog(const char *msg)
{
    time_t curr = time(NULL);
    curr -= logging_initial_time;
    unsigned days = curr / 86400, hours = curr / 3600 % 24;
    unsigned mins = curr / 60 % 60, secs = curr % 60;
    fprintf(logging_file_handle, "Day %u, %02u:%02u:%02u %s\n", days + 1, hours, mins, secs, msg);
    fflush(logging_file_handle);
}

int finish_logging(void)
{
    return fclose(logging_file_handle);
}
