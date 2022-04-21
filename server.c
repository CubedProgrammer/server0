#include<arpa/inet.h>
#include<pthread.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/socket.h>
#include<unistd.h>
#include"logging.h"
#ifndef POET
#define PORT 8080
#endif
struct accept_routine_data
{
    socklen_t *slenp;
    struct sockaddr *sap;
    int sfd;
};
char msg200[] = "HTTP/1.1 200 OK\r\ncontent-type: text, text/html\r\nconnection: close\r\n\r\n";
char msg400[] = "HTTP/1.1 400 Bad Request\r\ncontent-type: text\r\n\r\n400 Bad Request\r\n";
char msg404[] = "HTTP/1.1 404 Not Found\r\ncontent-type: text\r\n\r\n404 Not Found\r\n";
int start_server(struct sockaddr_in *saddrp, int port);
void *accept_routine(void *arg);
void bad_request(int cli)
{
    write(cli, msg400, sizeof(msg400));
    close(cli);
}
void not_found(int cli)
{
    write(cli, msg404, sizeof(msg404));
    close(cli);
}
void fetch_file(int cli, const char *path)
{
    /*size_t pathlen = strlen(path);
    if(pathlen > 5 && pathlen < 2595)
    {
        char fname[2601];
        strcpy(fname, "pages");
        strcpy(fname + 5, path);
    }*/
    write(cli, msg200, sizeof msg200);
    char cbuf[2048];
    size_t bc = 2048;
    FILE *fh = fopen("pages/index.html", "r");
    while(bc == sizeof(cbuf))
    {
        bc = fread(cbuf, 1, sizeof(cbuf), fh);
        write(cli, cbuf, bc);
    }
    fclose(fh);
    close(cli);
}
int handle_client(int cli)
{
    char cbuf[10001];
    size_t bc = read(cli, cbuf, 10000);
    cbuf[bc] = '\0';
    char reqmeth[13];
    char *space = strchr(cbuf, ' ');
    unsigned ind = space - cbuf;
    int succ = 0;
    if(ind > 12)
    {
        bad_request(cli);
        succ = -1;
    }
    else
    {
        strncpy(reqmeth, cbuf, ind);
        reqmeth[ind] = '\0';
        if(strcmp(reqmeth, "GET") == 0)
        {
            char *path = space + 1;
            space = strchr(path, ' ');
            ind = space - path;
            path[ind] = '\0';
            fetch_file(cli, path);
        }
        else
        {
            bad_request(cli);
            succ = -1;
        }
    }
    return succ;
}
int main(int argl, char *argv[])
{
    puts("site_one");
    const char *logfile = "logs.txt";
    if(argv[1])
        logfile = argv[1];
    init_logger(logfile);
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
        getchar();
        succ = finish_logging();
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
            infolog("Accepting a client has failed");
        }
        else
            handle_client(cfd);
    }
    return NULL;
}
int start_server(struct sockaddr_in *saddrp, int port)
{
    int sfd = socket(AF_INET, SOCK_STREAM, 0);
    if(sfd == -1)
    {
         infolog("Socket file descriptor is -1");
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
                infolog("listen failed");
                sfd = -1;
            }
        }
        else
        {
            infolog("bind failed");
            sfd = -1;
        }
    }
    return sfd;
}
