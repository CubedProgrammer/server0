#include<stdio.h>
#include<string.h>
#include<time.h>
int main(int argl, char *argv[])
{
    int succ = 0;
    if(argv[1] == NULL)
        succ = -1;
    else
    {
        char *arg = argv[1];
        char numstr[25], url[2601];
        if(arg[0] == '?')
        {
            ++arg;
            char *eqp = strchr(arg, '=');
            if(eqp != NULL)
            {
                *eqp = '\0';
                if(strcmp(arg, "url") == 0)
                {
                    arg = eqp + 1;
                    size_t urllen = strlen(arg);
                    if(urllen < 2601)
                    {
                        size_t ind = 0;
                        char c;
                        for(char *it = arg; *it != '\0'; ++it)
                        {
                            if(*it == '%')
                            {
                                url[ind] = '\0';
                                for(int i = 0; i < 2; i++)
                                {
                                    url[ind] <<= 4;
                                    c = *++it;
                                    if(c >= '0' && c <= '9')
                                        c -= '0';
                                    else if(c >= 'A' && c <= 'F')
                                        c -= 'A' - 10;
                                    else
                                        c = 4;
                                    url[ind] += c;
                                }
                            }
                            else
                                url[ind] = *it;
                            ++ind;
                        }
                        url[ind] = '\0';
                        FILE *redirects = fopen("redirects.txt", "a");
                        if(redirects != NULL)
                        {
                            long unsigned m = time(NULL), n = m * m;
                            m = (n * 1001) >> 3;
                            m *= urllen * (2601 - urllen);
                            for(int i = 0; i < 8; i++)
                            {
                                c = n >> (i * 4);
                                c &= 15;
                                c = c < 10 ? c + '0' : c + 'a' - 10;
                                numstr[i + 16] = c;
                            }
                            for(int i = 0; i < 16; i++)
                            {
                                c = m >> (i * 4);
                                c &= 15;
                                c = c < 10 ? c + '0' : c + 'a' - 10;
                                numstr[i] = c;
                            }
                            numstr[24] = '\0';
                            fprintf(redirects, "%s %s\n", numstr, url);
                            fclose(redirects);
                            puts("HTTP/1.1 200 OK\r\ncontent-type: text/plain\r");
                            puts("connection: close\r");
                            printf("\r\n%s", numstr);
                        }
                        else
                            succ = -1;
                    }
                    else
                        succ = -1;
                }
                else
                    succ = -1;
            }
            else
                succ = -1;
        }
        else
        {
            size_t arglen = strlen(arg);
            if(arglen == 24)
            {
                FILE *fh = fopen("redirects.txt", "r");
                if(fh)
                {
                    fscanf(fh, "%s %s", numstr, url);
                    if(strcmp(arg, numstr) == 0)
                        goto found;
                    while(!feof(fh))
                    {
                        if(strcmp(arg, numstr) == 0)
                            goto found;
                        else
                            fscanf(fh, "%s %s", numstr, url);
                    }
                    succ = -1;
                    goto closing;
                    found:
                    puts("HTTP/1.1 307 Temporary Redirect\r");
                    printf("Location: %s\r\n\r\n", url);
                    closing:
                    fclose(fh);
                }
                else
                    succ = -1;
            }
            else
                succ = -1;
        }
    }
    return succ;
}
