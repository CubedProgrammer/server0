#ifndef Included_logging_h
#define Included_logging_h
int init_logger(const char *path);
void infolog(const char *msg);
int finish_logging(void);
#endif
