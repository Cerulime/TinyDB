#include "..\include\FileOperation.hpp"

void FileOperation::merge_file(const std::string &file, const std::string &append)
{
    std::ifstream in2(append);
    std::ofstream out(file, std::ios::app);
    std::string line;
    while (std::getline(in2, line))
    {
        out << line << std::endl;
    }
    in2.close();
    out.close();
    std::remove(append.c_str());
}