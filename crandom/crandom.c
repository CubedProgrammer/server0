#include<dirent.h>
#include<stdio.h>
#include<stdlib.h>
#include<sys/stat.h>
#include<sys/statvfs.h>
#include<time.h>
size_t proc_cnt(void)
{
    size_t cnt = 0;
    DIR *dirp = opendir("/proc");
    if(dirp != NULL)
    {
        for(struct dirent *en = readdir(dirp); en != NULL; en = readdir(dirp), ++cnt);
        closedir(dirp);
    }
    else
        cnt = 0x9249249249249249;
    return cnt;
}
int main(int argl, char *argv[])
{
    time_t curr = time(NULL);
    struct statvfs fsdat;
    int succ = statvfs(".", &fsdat);
    size_t fssz = 0xdf59375f;
    if(succ == 0)
        fssz = fsdat.f_frsize * fsdat.f_blocks;
    char selfexe[2601];
    char *ptr = realpath("/proc/self/exe", selfexe);
    size_t selfsz = 0x5f3759df, selftm;
    if(ptr != NULL)
    {
        struct stat fdat;
        succ = stat(ptr, &fdat);
        if(succ == 0)
        {
            selfsz = fdat.st_size;
            selftm = fdat.st_atime * fdat.st_mtime * fdat.st_ctime;
        }
    }
    selfsz *= selfsz * selfsz;
    size_t prcnt = proc_cnt();
    long unsigned num = curr * curr;
    num += selfsz ^ selftm;
    num *= prcnt;
    num ^= fssz * fssz;
    size_t bufsz = num % 1729 + 1;
    if(argv[1] != NULL)
    {
        bufsz = atoi(argv[1]);
        if(bufsz == 0)
            bufsz = num % 1729 + 1;
        if(bufsz > 1729)
            bufsz = 1729;
    }
    char cbuf[1729];
    for(size_t i = 0; i < bufsz; ++i)
    {
        num *= 6971;
        num += 13;
        cbuf[i] = num % 96 + 32;
    }
    puts("HTTP/1.1 200 OK\r");
    puts("content-type: text\r");
    puts("connection: close\r\n\r");
    fwrite(cbuf, 1, bufsz, stdout);
    putchar('\n');
    return 0;
}
