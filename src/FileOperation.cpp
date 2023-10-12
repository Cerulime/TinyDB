#include "..\include\FileOperation.hpp"

void FileOperation::produce_task(const Task &task)
{
    std::lock_guard<std::mutex> lock(tasks_mutex);
    tasks.push(task);
    tasks_cv.notify_one();
}

FileOperation::Task FileOperation::consume_task()
{
    std::unique_lock<std::mutex> lock(tasks_mutex);
    tasks_cv.wait(lock, [] { return !tasks.empty(); });
    Task task = std::move(tasks.front());
    tasks.pop();
    return task;
}

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