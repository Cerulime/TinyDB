#include "..\include\FileOperation.hpp"
#include <gtest/gtest.h>
#include <fstream>

TEST(FileOperationTest, CreateFile)
{
    FileOperation fileOp;
    FileOperation::Task task(FileOperation::Operation::CREATE, "test.txt", "Hello, world!");
    fileOp.work(task);
    std::ifstream in("test.txt");
    std::string content((std::istreambuf_iterator<char>(in)), std::istreambuf_iterator<char>());
    EXPECT_EQ(content, "Hello, world!\n");
    in.close();
    std::remove("test.txt");
}

TEST(FileOperationTest, AppendToFile)
{
    FileOperation fileOp;
    std::ofstream out("test.txt");
    out << "Hello, world!" << std::endl;
    out.close();
    std::ofstream out2("test2.txt");
    out2 << "This is a test." << std::endl;
    out2.close();
    FileOperation::Task task(FileOperation::Operation::APPEND, "test.txt", "test2.txt");
    fileOp.work(task);
    std::ifstream in("test.txt");
    std::string content((std::istreambuf_iterator<char>(in)), std::istreambuf_iterator<char>());
    EXPECT_EQ(content, "Hello, world!\nThis is a test.\n");
    in.close();
    std::remove("test.txt");
    std::remove("test2.txt");
}

TEST(FileOperationTest, DeleteFile)
{
    FileOperation fileOp;
    std::ofstream out("test.txt");
    out << "Hello, world!" << std::endl;
    out.close();
    FileOperation::Task task(FileOperation::Operation::DELETE, "test.txt", "");
    fileOp.work(task);
    std::ifstream in("test.txt");
    EXPECT_FALSE(in.good());
    in.close();
}

TEST(FileOperationTest, MultiThread)
{
    FileOperation fileOp;
    std::ofstream out("test.txt");
    out << "Hello, world!" << std::endl;
    out.close();
    std::ofstream out2("test2.txt");
    out2 << "This is a test." << std::endl;
    out2.close();
    FileOperation::Task task(FileOperation::Operation::APPEND, "test.txt", "test2.txt");
    std::thread t1(&FileOperation::work, &fileOp, task);
    std::thread t2(&FileOperation::work, &fileOp, task);
    std::thread t3(&FileOperation::work, &fileOp, task);
    t1.join();
    t2.join();
    t3.join();
    std::ifstream in("test.txt");
    std::string content((std::istreambuf_iterator<char>(in)), std::istreambuf_iterator<char>());
    EXPECT_EQ(content, "Hello, world!\nThis is a test.\nThis is a test.\nThis is a test.\n");
    in.close();
    std::remove("test.txt");
    std::remove("test2.txt");
}