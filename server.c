#include<arpa/inet.h>
#include<ctype.h>
#include<errno.h>
#include<pthread.h>
#include<signal.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/select.h>
#include<sys/socket.h>
#include<time.h>
#include<unistd.h>
#include"fetch.h"
#include"logging.h"
#include"mimetype.h"
#ifndef PORT
#define PORT 80
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
char msg404[] = "HTTP/1.1 404 Not Found\r\ncontent-type: text/html\r\n\r\n";
char body404[5041];
size_t body404len;
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
    write(cli, msg404, sizeof(msg404) - 1);
    write(cli, body404, body404len);
    close(cli);
}
void fetch_file(int cli, const char *path, const char *host)
{
    size_t pathlen = strlen(path), hostlen = strlen(host);
    if(strchr(host, '/') != NULL)
    {
        char msg[] = "Don't try it!";
        infolog("Host name contains a / other than the end.");
        write(cli, msg404, sizeof(msg404) - 1);
        write(cli, msg, sizeof(msg) - 1);
        close(cli);
    }
    else if(pathlen + hostlen > 0 && pathlen + hostlen < 2600)
    {
        char fname[2601];
        memcpy(fname, host, hostlen);
        memcpy(fname + hostlen, path, pathlen + 1);
        infolog(path);
        int succ = fetch_resource(fname, cli);
        if(succ != 0)
        {
            infolog("Fetching resource failed");
            not_found(cli);
        }
        else
            close(cli);
    }
    else
    {
        infolog("Path name is too long.");
        close(cli);
    }
}
int handle_client(int cli)
{
    char cbuf[10001];
    size_t bc = 0;
    fd_set fds, *fdsp = &fds;
    struct timeval tv, *tvp = &tv;
    int succ = 0;
    FD_ZERO(fdsp);
    FD_SET(cli, fdsp);
    tv.tv_sec = 1;
    tv.tv_usec = 0;
    int ready = select(cli + 1, fdsp, NULL, NULL, tvp);
    if(ready)
    {
        bc = read(cli, cbuf, 10000);
		cbuf[bc] = '\0';
		infolog("Request was made.");
		infolog(cbuf);
		char reqmeth[13];
		char *space = strchr(cbuf, ' ');
        char *lnstart = strchr(cbuf, '\n');
        char *colon, *hostname = NULL;
		unsigned ind = space == NULL ? 13 : space - cbuf;
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
                    while(lnstart != NULL)
                    {
                        ++lnstart;
                        colon = strchr(lnstart, ':');
                        if(colon != NULL)
                        {
                            for(; colon[-1] == ' '; --colon);
                            for(char *it = lnstart; it != colon; ++it)
                                *it = toupper(*it);
                            *colon = '\0';
                            hostname = lnstart;
                            lnstart = strchr(colon + 1, '\n');
                            if(strcmp(hostname, "HOST") == 0)
                            {
                                for(hostname = colon + 1; *hostname == ' '; ++hostname);
                                lnstart[-1] = '\0';
                                lnstart = NULL;
                            }
                        }
                        else
                            lnstart = NULL;
                    }
                    if(hostname != NULL)
                    {
    		            ind = space - path;
	    	            path[ind] = '\0';
		                fetch_file(cli, path, hostname);
                    }
                    else
                    {
                        close(cli);
                        succ = -1;
                    }
		        }
		    }
		    else
		    {
		        bad_request(cli);
		        succ = -1;
		    }
		}
    }
    else
    {
        infolog("A client connected but did not make a request in time.");
        close(cli);
        succ = -1;
    }
    return succ;
}
int main(int argl, char *argv[])
{
    puts("00");
    signal(SIGPIPE, SIG_IGN);
    const char *logfile = "logs.txt";
    if(argv[1])
        logfile = argv[1];
    init_logger(logfile);
    puts("initialized logger");
    int succ = load_mimetypes(MIMETYPES);
    puts("loaded mimetypes");
    if(succ != 0)
        infolog("Could not load mimetypes file.");
    FILE *f404 = fopen("nf404.html", "r");
    if(f404 == NULL)
    {
        strcpy(body404, "404 Not Found");
        body404len = 13;
    }
    else
    {
        body404len = fread(body404, 1, sizeof(body404), f404);
        fclose(f404);
    }
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
        pthread_detach(pth);
        getchar();
        infolog("Server has been shutdown.");
        free_mimetypes();
        close(ard.sfd);
        succ = finish_logging();
    }
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
            snprintf(msg, sizeof(msg), "Accepting a client has failed, %s.", strerror(errno));
            infolog(msg);
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
                close(sfd);
                sfd = -1;
            }
        }
        else
        {
            infolog("bind failed");
            perror("bind");
            close(sfd);
            sfd = -1;
        }
    }
    return sfd;
}
