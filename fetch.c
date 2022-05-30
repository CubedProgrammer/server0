#include<errno.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/stat.h>
#include<sys/wait.h>
#include<unistd.h>
#include"fetch.h"
#include"logging.h"
#include"mimetype.h"

char msg200[] = "HTTP/1.1 200 OK\r\nconnection: close\r\ncontent-type: ";
char endlns[] = "\r\n\r\n";

int fetch_resource(char *path, int cli)
{
    int succ = 0;
    char cbuf[16384];
    FILE *fh;
    if(access(path, F_OK) == 0)
    {
        char ff = 0;
        struct stat fdat;
        stat(path, &fdat);
        if(S_ISDIR(fdat.st_mode))
        {
            char *indexp = cbuf;
            strcpy(indexp, path);
            size_t plen = strlen(indexp);
            if(plen > 2589)
                succ = -1;
            else
            {
                strcpy(indexp + plen, "/index.html");
                path = indexp;
                infolog("Attempted to fetch directory, fetching this index.html file");
                ff = 1;
            }
        }
        else if(access(path, X_OK) == 0)
            succ = fetch_executable(path, "", cli);
        else
            ff = 1;
        if(ff)
        {
            infolog("Fetching file");
            openf:
            infolog(path);
            fh = fopen(path, "r");
            if(fh == NULL)
                succ = -1;
            else
            {
                const char *ext = strrchr(path, '.');
                puts(ext);
                write(cli, msg200, sizeof(msg200) - 1);
                const char *ct = ext == NULL ? "text/plain" : mimetype(ext + 1);
                size_t ctlen = strlen(ct);
                write(cli, ct, ctlen);
                write(cli, endlns, sizeof(endlns) - 1);
                size_t bc = fread(cbuf, 1, sizeof(cbuf), fh);
                write(cli, cbuf, bc);
                while(bc == sizeof(cbuf))
                {
                    bc = fread(cbuf, 1, sizeof(cbuf), fh);
                    write(cli, cbuf, bc);
                }
                fclose(fh);
            }
        }
    }
    else
    {
        if(errno == ENOTDIR || errno == ENOENT || errno == EACCES)
        {
            char *slashp = strrchr(path, '/'), *last;
            *slashp = '\0';
            while(succ == 0 && access(path, F_OK) != 0)
            {
                last = slashp;
                slashp = strrchr(path, '/');
                *slashp = '\0';
                *last = '/';
                if(slashp == NULL)
                    succ = -1;
            }
            if(succ == 0)
                succ = fetch_executable(path, slashp + 1, cli);
        }
        else
            succ = -1;
    }
    return succ;
}
int fetch_executable(const char *path, const char *param, int cli)
{
    int succ = 0;
    int po[2];
    succ = pipe(po);
    if(succ == 0)
    {
        int pid = fork();
        if(pid == 0)
        {
            int rt = po[1];
            dup2(rt, STDOUT_FILENO);
            char execpath[2600];
            strcpy(execpath, "./");
            strcpy(execpath + 2, path);
            infolog("Fetching executable");
            infolog(execpath);
            infolog(param);
            int fail = execl(execpath, execpath, param, (char *)NULL);
            infolog("Could not execute executable");
            exit(fail);
        }
        else
        {
            close(po[1]);
            int rd = po[0];
            int status;
            waitpid(pid, &status, 0);
            status = WEXITSTATUS(status);
            if(status != 0)
                succ = -1;
            else
            {
                char cbuf[16384];
                size_t bc = sizeof cbuf;
                while(bc == sizeof(cbuf))
                {
                    bc = read(rd, cbuf, sizeof cbuf);
                    write(cli, cbuf, bc);
                }
            }
        }
    }
    else
        infolog("Pipe creation failed");
    return succ;
}
