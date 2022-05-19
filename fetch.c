#include<errno.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/stat.h>
#include<sys/wait.h>
#include<unistd.h>
#include"fetch.h"
#include"mimetype.h"

char msg200[] = "HTTP/1.1 200 OK\r\nconnection: close\r\ncontent-type: ";
char endlns[] = "\r\n\r\n";

int fetch_resource(char *path, int cli)
{
    int succ = 0;
    if(access(path, F_OK) == 0)
    {
        struct stat fdat;
        stat(path, &fdat);
        if(S_ISDIR(fdat.st_mode))
        {
            succ = -1;
        }
        else if(access(path, X_OK) == 0)
            succ = fetch_executable(path, "", cli);
        else
        {
            FILE *fh = fopen(path, "r");
            if(fh == NULL)
                succ = -1;
            else
            {
                char cbuf[16384];
                const char *ext = strrchr(path, '.');
                write(cli, msg200, sizeof(msg200) - 1);
                const char *ct = ext == NULL ? "text" : mimetype(ext + 1);
                size_t ctlen = strlen(ct);
                write(cli, ct, ctlen);
                write(cli, endlns, sizeof endlns);
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
        if(errno == EACCES)
        {
            succ = -1;
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
            dup2(STDOUT_FILENO, rt);
            char execpath[2600];
            strcpy(execpath, "./");
            strcpy(execpath + 2, path);
            int fail = execl(execpath, execpath, param, (char *)NULL);
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
    return succ;
}
