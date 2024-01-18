#include<algorithm>
#include<array>
#include<filesystem>
#include<format>
#include<iostream>
#include<vector>
std::array<std::string, 6>allowed_exts{"s", "asm", "h", "c", "hpp", "cpp"};
bool validate_ext(const std::string &ext)
{
	return std::find(allowed_exts.cbegin(), allowed_exts.cend(), ext.substr(1)) != allowed_exts.cend();
}
std::string mkelem(const std::string &file)
{
    return std::format("<div><a href=\"{}\">{}</a></div>", file, file);
}
void dirent()
{
    using namespace std::filesystem;
    std::cout << "HTTP/1.1 200 OK\r\n" << "connection: close\r\n" << "content-type: text/html\r\n\r\n";
    path p = ".";
    recursive_directory_iterator enit(p);
    for(path en : enit)
    {
        if(en.has_extension() && validate_ext(en.extension()))
        {
            if((status(en).permissions() & perms::owner_exec) != perms::owner_exec)
                std::cout << mkelem(en.string().substr(2)) << '\n';
        }
    }
}
