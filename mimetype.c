#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include"mimetype.h"
struct mimetype_hashmap site_one_mimetype_table;

int load_mimetypes(const char *fpath)
{
    int succ = 0;
    char **bs = malloc(30 * sizeof(*bs));
    size_t cnt = 0, capa = 30;
    FILE *fh = fopen(fpath, "r");
    if(fh == NULL)
    {
        succ = -1;
        free(bs);
    }
    else
    {
        memset(bs, 0, capa * sizeof(*bs));
        char ext[13], content[91];
        size_t hashval, og;
        size_t klen, vlen;
        char *curr;
        fscanf(fh, "%s %s", ext, content);
        succ = feof(fh);
        while(succ == 0)
        {
            printf("%s %s\n", ext, content);
            fflush(stdout);
            if(cnt + 1 > 0.6 * capa)
            {
                capa += capa >> 1;
                bs = realloc(bs, capa);
            }
            hashval = 0;
            for(const char *it = ext; *it != '\0'; ++it)
            {
                hashval *= 37;
                hashval += *it;
            }
            hashval %= capa;
            if(bs[hashval] == NULL)
            {
                ins:
                klen = strlen(ext);
                vlen = strlen(content);
                bs[hashval] = malloc(klen + vlen + 2);
                strcpy(bs[hashval], ext);
                strcpy(bs[hashval] + klen + 1, content);
            }
            else
            {
                og = hashval;
                ++hashval;
                for(; hashval != og; hashval++, hashval %= capa)
                {
                    curr = bs[hashval];
                    if(curr == NULL)
                        goto ins;
                }
            }
            ++cnt;
            fscanf(fh, "%s %s", ext, content);
            succ = feof(fh);
        }
        fclose(fh);
        site_one_mimetype_table.buckets = bs;
        site_one_mimetype_table.bcnt = capa;
        site_one_mimetype_table.cnt = cnt;
        succ = 0;
    }
    return succ;
}

const char *mimetype(const char *ext)
{
    const char *str = NULL;
    size_t mod = site_one_mimetype_table.bcnt, hashval = 0;
    for(const char *it = ext; *it != '\0'; ++it)
    {
        hashval *= 37;
        hashval += *it;
    }
    hashval %= mod;
    if(site_one_mimetype_table.buckets[hashval] != NULL)
    {
        int cmp = strcmp(site_one_mimetype_table.buckets[hashval], ext);
        if(cmp != 0)
        {
            size_t og = hashval;
            const char *curr;
            ++hashval;
            for(; hashval != og; ++hashval, hashval %= mod)
            {
                curr = site_one_mimetype_table.buckets[hashval];
                if(curr == NULL)
                    hashval = og - 1;
                else
                {
                    cmp = strcmp(curr, ext);
                    if(cmp == 0)
                    {
                        str = curr + strlen(curr) + 1;
                        hashval = og - 1;
                    }
                }
            }
        }
        else
            str = site_one_mimetype_table.buckets[hashval] + strlen(site_one_mimetype_table.buckets[hashval]) + 1;
    }
    return str;
}

void free_mimetypes(void)
{
    free(site_one_mimetype_table.buckets);
}
