#include<arpa/inet.h>
#include<pthread.h>
#include<signal.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/socket.h>
#include<unistd.h>
#include"fetch.h"
#include"logging.h"
#include"mimetype.h"
#ifndef PORT
#define PORT 8080
#endif
#ifndef MIMETYPES
#define MIMETYPES "content-type.txt"
#endif
struct accept_routine_data
{
    socklen_t *slenp;
    struct sockaddr *sap;
    int sfd;
};
char msg400[] = "HTTP/1.1 400 Bad Request\r\ncontent-type: text\r\n\r\n400 Bad Request\r\n";
char msg404[] = "HTTP/1.1 404 Not Found\r\ncontent-type: text\r\n\r\n404 Not Found\r\n";
int start_server(struct sockaddr_in *saddrp, int port);
void *accept_routine(void *arg);
void bad_request(int cli)
{
    infolog("Previous request was invalid");
    write(cli, msg400, sizeof(msg400));
    close(cli);
}
void not_found(int cli)
{
    infolog("Previous request requested for a non-existant resource");
    write(cli, msg404, sizeof(msg404));
    close(cli);
}
void fetch_file(int cli, const char *path)
{
    size_t pathlen = strlen(path);
    if(pathlen > 0 && pathlen < 2595)
    {
        char fname[2601];
        strcpy(fname, "pages");
        strcpy(fname + 5, path);
        infolog(path);
        /*size_t bc = write(cli, msg200, sizeof msg200);
        if(bc != sizeof msg200)
            infolog("Less than what should have been written was written in 200 response.");*/
        int succ = fetch_resource(fname, cli);
        if(succ != 0)
        {
            infolog("Fetching resource failed");
            close(cli);
        }
        else
            close(cli);
    }
    /*size_t bc = write(cli, msg200, sizeof msg200);
    if(bc != sizeof msg200)
        infolog("Less than what should have been written was written in 200 response.");
    char cbuf[2048];
    bc = 2048;
    FILE *fh = fopen("pages/index.html", "r");
    while(bc == sizeof(cbuf))
    {
        bc = fread(cbuf, 1, sizeof(cbuf), fh);
        write(cli, cbuf, bc);
    }
    fclose(fh);*/
}
void sigpipe_handler(int x)
{
    infolog("SIGPIPE was sent.");
}
int handle_client(int cli)
{
    char cbuf[10001];
    size_t bc = read(cli, cbuf, 10000);
    cbuf[bc] = '\0';
    infolog("Request was made.");
    infolog(cbuf);
    char reqmeth[13];
    char *space = strchr(cbuf, ' ');
    unsigned ind = space == NULL ? 13 : space - cbuf;
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
            if(space == NULL)
            {
                bad_request(cli);
                succ = -1;
            }
            else
            {
                ind = space - path;
                path[ind] = '\0';
                fetch_file(cli, path);
            }
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
    signal(SIGPIPE, sigpipe_handler);
    const char *logfile = "logs.txt";
    if(argv[1])
        logfile = argv[1];
    init_logger(logfile);
    puts("initialized logger");
    int succ = load_mimetypes(MIMETYPES);
    puts("loaded mimetypes");
    if(succ != 0)
        infolog("Could not load mimetypes file.");
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
        free_mimetypes();
        succ = finish_logging();
    }
    close(ard.sfd);
    return succ;
}
void *accept_routine(void *arg)
{
    struct accept_routine_data *datp = arg;
    struct sockaddr *sap = datp->sap;
    struct sockaddr_in *saip = (struct sockaddr_in *)sap;
    int sfd = datp->sfd;
    socklen_t *slen = datp->slenp;
    int cfd;
    char msg[1001];
    for(;;)
    {
        cfd = accept(sfd, sap, slen);
        if(cfd == -1)
        {
            infolog("Accepting a client has failed");
        }
        else
        {
		    snprintf(msg, sizeof(msg), "Accepted client %s with socket %i.\n", inet_ntoa(saip->sin_addr), cfd);
		    infolog(msg);
            handle_client(cfd);
        }
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
