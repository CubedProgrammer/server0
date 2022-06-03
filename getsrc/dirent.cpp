#include<algorithm>
#include<array>
#include<filesystem>
#include<iostream>
#include<vector>
std::array<std::string, 6>allowed_exts{"s", "asm", "h", "c", "hpp", "cpp"};
bool validate_ext(const std::string &ext)
{
	return std::find(allowed_exts.cbegin(), allowed_exts.cend(), ext.substr(1)) != allowed_exts.cend();
}
void dirent()
{
    using namespace std::filesystem;
    std::cout << "HTTP 200 OK\r\n" << "connection: close\r\n" << "content-type: text/plain\r\n\r\n";
    path p = ".";
    recursive_directory_iterator enit(p);
    for(path en : enit)
    {
        if(en.has_extension() && validate_ext(en.extension()))
            std::cout << en.string() << '\n';
    }
}
