#include "..\include\FileOperation.hpp"

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

/**
 * This function performs file operations based on the given task.
 * @param task The task to be performed.
 */
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

/**
 * Creates a task to create a new file with the given name and content.
 * @param file The name of the file to create.
 * @param content The content to write to the new file.
 */
void FileOperation::create_file_task(const std::string &file, const std::string &content)
{
    produce_task({Operation::CREATE, file, content});
}

/**
 * Adds a task to append the contents of a file to another file.
 * @param file The file to append to.
 * @param append The file whose contents will be appended.
 */
void FileOperation::merge_file_task(const std::string &file, const std::string &append)
{
    produce_task({Operation::APPEND, file, append});
}

/**
 * Deletes a file by adding a task to the task queue.
 * @param file The path of the file to be deleted.
 */
void FileOperation::delete_file_task(const std::string &file)
{
    produce_task({Operation::DELETE, file, ""});
}