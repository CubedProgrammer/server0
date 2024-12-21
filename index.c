#include<dirent.h>
#include<stdio.h>
#include"index.h"

int default_index_html(int cli, const char *dir)
{
    int succ = 0;
    DIR *dh = opendir(dir);
    if(dh != NULL)
    {
        FILE *f = fdopen(cli, "w");
        fputs("<html><head><title>%s</title></head><body style=\"color: #FFFFFF; background: #000000\">", f);
        for(struct dirent *en = readdir(dh); en != NULL; en = readdir(dh))
            out_entry(f, en->d_name);
        fputs("</body></html>", f);
        fflush(f);
    }
    else
        succ = -1;
    return succ;
}

void out_entry(FILE *f, const char *en)
{
    if(en[0] != '.')
        fprintf(f, "<a href=\"%s\">%s</a>", en, en);
}
