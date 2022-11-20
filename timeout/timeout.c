#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
int main(int argl, char *argv[])
{
    int succ = 0;
    puts("HTTP/1.1 200 OK\r");
    puts("content-type: text\r");
    puts("connection: close\r\n\r");
    if(*argv[1] > '9' || *argv[1] < '1')
    {
        puts("The file should be a positive integer in decimal notation.");
        puts("The amount of time the response will take in seconds.");
    }
    else
    {
        unsigned cnt = atoi(argv[1]);
        succ = sleep(cnt) != 0;
        printf("%u\n", cnt);
    }
    return succ;
}
