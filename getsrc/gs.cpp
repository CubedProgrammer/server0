#include<algorithm>
#include<array>
#include<filesystem>
#include<fstream>
#include<iostream>
#include<vector>
#include"dirent.hpp"
void print_file(const std::string &s)
{
    using namespace std;
    std::array<char, 16384>cbuf;
    ifstream ifs(s);
    streamsize sz;
    while(!ifs.eof())
    {
        ifs.read(cbuf.data(), cbuf.size());
        sz = ifs.gcount();
        cout.write(cbuf.data(), sz);
    }
}
int main_pp(std::vector<std::string>&args)
{
    std::ios_base::sync_with_stdio(false);
    int succ = 0;
    if(args.empty() || args[0].empty())
        dirent();
    else
    {
        using namespace std::filesystem;
        path requested = args[0];
        auto ext = requested.extension().string();
        if(ext.size() > 1)
        {
            if(validate_ext(ext))
            {
                if(exists(requested))
                {
                    std::cout << "HTTP/1.1 200 OK\r\n" << "connection: close\r\n" << "content-type: text/plain\r\n\r\n";
                    print_file(requested.string());
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
    return succ;
}
int main(int argl,char**argv)
{
    std::vector<std::string>args(argv + 1, argv + argl);
    return main_pp(args);
}
