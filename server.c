#include<arpa/inet.h>
#include<pthread.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/socket.h>
#ifndef POET
#define PORT 8080
#endif
struct accept_routine_data
{
    socklen_t *slenp;
    struct sockaddr *sap;
    int sfd;
};
int start_server(struct sockaddr_in *saddrp, int port);
void *accept_routine(void *arg);
int main(int argl, char *argv[])
{
    puts("site_one");
    int succ = 0;
    struct sockaddr_in saddr;
    struct accept_routine_data ard;
    socklen_t slen = sizeof saddr;
    ard.sap = (struct sockaddr *)&saddr;
    ard.sfd = start_server(&saddr, PORT);
    ard.slenp = &slen;
    if(ard.sfd == -1)
        succ = 1;
    else
    {
        pthread_t pth;
        pthread_create(&pth, NULL, accept_routine, &ard);
    }
    return succ;
}
void *accept_routine(void *arg)
{
    struct accept_routine_data *datp = arg;
    struct sockaddr *sap = datp->sap;
    int sfd = datp->sfd;
    socklen_t *slen = datp->slenp;
    int cfd;
    for(;;)
    {
        cfd = accept(sfd, sap, slen);
        if(cfd == -1)
        {
            // infolog("Accepting a client has failed");
        }
    }
    return NULL;
}
int start_server(struct sockaddr_in *saddrp, int port)
{
    int sfd = socket(AF_INET, SOCK_STREAM, 0);
    if(sfd == -1)
    {
        // infolog("Socket file descriptor is -1");
    }
    else
    {
        struct sockaddr_in saddr;
        socklen_t slen = sizeof(saddr);
        saddr.sin_addr.s_addr = INADDR_ANY;
        saddr.sin_family = AF_INET;
        saddr.sin_port = htons(PORT);
        if(bind(sfd, (struct sockaddr *)&saddr, slen) == 0)
        {
            if(listen(sfd, 3) == 0)
                *saddrp = saddr;
            else
            {
                // infolog("listen failed");
                sfd = -1;
            }
        }
        else
        {
            // infolog("bind failed");
            sfd = -1;
        }
    }
    return sfd;
}
