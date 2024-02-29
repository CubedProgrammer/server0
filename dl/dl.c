#include<dirent.h>
#include<fcntl.h>
#include<stdio.h>
#include<sys/sendfile.h>
#include<sys/stat.h>
#include<unistd.h>
int main(int argl, char *argv[])
{
    const char *fmt = "<div><a href=\"dl/%s\">%s</a></div>\n";
    int succ = 0;
    if(argv[1] == NULL || argv[1][0] == '\0')
    {
        DIR *dh = opendir("dl");
        if(dh == NULL)
        {
            perror("opendir");
            puts("HTTP/1.1 500 Internal Server Error\r");
            puts("Connection: close\r\n\r");
            puts("Error 500 Internal Server Error");
            succ = 1;
        }
        else
        {
            char *name;
            puts("HTTP/1.1 200 OK\r");
            puts("Content-type: text/html\r");
            puts("Connection: close\r\n\r");
            puts("<head><link rel=\"stylesheet\"href=\"/style.css\"></head>");
            puts("<body>");
            for(struct dirent *en = readdir(dh); en != NULL; en = readdir(dh))
            {
                if(en->d_type == DT_REG)
                {
                    name = en->d_name;
                    printf(fmt, name, name);
                }
            }
            closedir(dh);
            puts("</body>");
        }
    }
    else
    {
        int fd = open(argv[1], O_RDONLY);
        if(fd == -1)
        {
            perror("open");
            puts("HTTP/1.1 404 Not Found\r");
            puts("Connection: close\r");
            puts("\r\n404 Not Found");
            succ = 1;
        }
        else
        {
            struct stat fdat;
            size_t totsent = 0;
            ssize_t sent = 1;
            stat(argv[1], &fdat);
            puts("HTTP/1.1 200 OK\r");
            puts("Connection: close\r");
            printf("Content-length: %zu\r\n", fdat.st_size);
            puts("Content-disposition: attachment\r\n\r");
            fflush(stdout);
            for(; sent > 0 && totsent < fdat.st_size; totsent += sent = sendfile(STDOUT_FILENO, fd, NULL, fdat.st_size - totsent));
            close(fd);
            succ = sent < 0;
        }
    }
    return succ;
}
