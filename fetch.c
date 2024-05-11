#include<errno.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/sendfile.h>
#include<sys/stat.h>
#include<sys/wait.h>
#include<unistd.h>
#include"fetch.h"
#include"logging.h"
#include"mimetype.h"

char msg200[] = "HTTP/1.1 200 OK\r\nconnection: close\r\ncontent-type: ";
char endlns[] = "\r\n\r\n";

int validate_depth(const char *path)
{
    const char *slash = strchr(path, '/');
    size_t last = 0;
    int succ = 0;
    int depth = 0;
    for(size_t curr = 1; succ == 0 && slash[curr] != '\0'; ++curr)
    {
        if(slash[curr] == '/')
        {
            if(last + 1 == curr)
            {
                last = curr;
                continue;
            }
            else if(last + 3 == curr)
            {
                if(slash[last + 1] == '.' && slash[last + 2] == '.')
                    --depth;
                else
                    ++depth;
            }
            else if(last + 2 == curr)
            {
                if(slash[last + 1] != '.')
                    ++depth;
            }
            else
                ++depth;
            last = curr;
        }
        if(depth < 0)
            succ = -1;
    }
    return succ;
}

int fetch_resource(char *path, int cli)
{
    int succ = 0;
    char cbuf[4096];
    FILE *fh;
    struct stat fdat;
    if(access(path, F_OK) == 0)
    {
        char ff = 0;
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
                if(access(path, F_OK) == 0)
                    infolog("Attempted to fetch directory, fetching this index.html file");
                else
                {
                    // to be implemented
                    infolog("The file index.html does not exists, fetching default index.html instead");
                    succ = -1;
                }
            }
        }
        if(access(path, X_OK) == 0)
            succ = fetch_executable(path, "", cli);
        else if(succ == 0)
            ff = 1;
        if(ff)
        {
            infolog("Fetching file");
            openf:
            infolog(path);
            if(validate_depth(path) == 0)
                fh = fopen(path, "r");
            else
                fh = NULL;
            if(fh == NULL)
                succ = -1;
            else
            {
                const char *ext = strrchr(path, '.');
                write(cli, msg200, sizeof(msg200) - 1);
                const char *ct = ext == NULL ? "text/plain" : mimetype(ext + 1);
                ct = ct == NULL ? "text/plain" : ct;
                size_t ctlen = strlen(ct);
                stat(path, &fdat);
                write(cli, ct, ctlen);
                write(cli, endlns, sizeof(endlns) - 1);
                if(sendfile(cli, fileno(fh), NULL, fdat.st_size) == -1)
                    infolog("sendfile system call failed");
                fclose(fh);
            }
        }
    }
    else
    {
        if(errno == ENOTDIR || errno == ENOENT || errno == EACCES)
        {
            char *slashp = strrchr(path, '/'), *last;
            if(slashp != NULL)
                *slashp = '\0';
            else
                succ = -1;
            while(succ == 0 && access(path, F_OK) != 0)
            {
                last = slashp;
                slashp = strrchr(path, '/');
                if(slashp == NULL)
                    succ = -1;
                else
                {
                    *slashp = '\0';
                    *last = '/';
                }
            }
            if(succ == 0)
            {
                stat(path, &fdat);
                if(S_ISDIR(fdat.st_mode))
                    succ = -1;
                else if(access(path, X_OK) == 0)
                    succ = fetch_executable(path, slashp + 1, cli);
                else
                    succ = -1;
            }
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
    if(validate_depth(path) == 0)
        succ = pipe(po);
    else
        succ = -1;
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
            char cbuf[16384];
            ssize_t bc = read(rd, cbuf, sizeof cbuf);
            while(bc > 0)
            {
                write(cli, cbuf, bc);
                bc = read(rd, cbuf, sizeof cbuf);
            }
            waitpid(pid, &status, 0);
            status = WEXITSTATUS(status);
            if(status != 0)
                succ = -1;
        }
    }
    else
        infolog("Pipe creation failed");
    return succ;
}
