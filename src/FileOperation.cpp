#include "..\include\FileOperation.hpp"

void FileOperation::merge_file(const std::string &file1, const std::string &file2)
{
    std::ifstream in2(file2);
    std::ofstream out(file1, std::ios::app);
    std::string line;
    while (std::getline(in2, line))
    {
        out << line << std::endl;
    }
    in2.close();
    out.close();
    std::remove(file2.c_str());
}