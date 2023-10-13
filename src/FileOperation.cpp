#include "..\include\FileOperation.hpp"

std::queue<FileOperation::Task> FileOperation::tasks;
std::mutex FileOperation::tasks_mutex;

void FileOperation::produce_task(const Task &task)
{
    std::lock_guard<std::mutex> lock(tasks_mutex);
    tasks.push(task);
}

FileOperation::Task FileOperation::consume_task()
{
    std::lock_guard<std::mutex> lock(tasks_mutex);
    Task task = std::move(tasks.front());
    tasks.pop();
    return task;
}

bool FileOperation::is_empty()
{
    std::lock_guard<std::mutex> lock(tasks_mutex);
    return tasks.empty();
}

void FileOperation::work(const Task &task)
{
    switch (task.opt)
    {
        case Operation::CREATE:
        {
            std::ofstream out(task.filename);
            out << task.content << std::endl;
            out.close();
            break;
        }
        case Operation::APPEND:
        {
            std::ifstream in2(task.content);
            std::ofstream out(task.filename, std::ios::app);
            std::string line;
            while (std::getline(in2, line))
            {
                out << line << std::endl;
            }
            in2.close();
            out.close();
            std::remove(task.content.c_str());
            break;
        }
        case Operation::DELETE:
        {
            std::remove(task.filename.c_str());
            break;
        }
        default:
            break;
    }
}

void FileOperation::create_file_task(const std::string &file, const std::string &content)
{
    produce_task({Operation::CREATE, file, content});
}

void FileOperation::merge_file_task(const std::string &file, const std::string &append)
{
    produce_task({Operation::APPEND, file, append});
}

void FileOperation::delete_file_task(const std::string &file)
{
    produce_task({Operation::DELETE, file, ""});
}