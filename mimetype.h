#ifndef Included_mimetype_h
#define Included_mimetype_h

struct mimetype_hashmap
{
    char **buckets;
    size_t bcnt, cnt;
};

extern struct mimetype_hashmap site_one_mimetype_table;
int load_mimetypes(const char *fpath);
const char *mimetype(const char *ext);
void free_mimetypes(void);

#endif
