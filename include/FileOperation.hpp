#pragma once
#include "Include.hpp"

class FileOperation
{
public:
    enum class Operation
    {
        CREATE,
        APPEND,
        DELETE
    };
    struct Task
    {
        Operation opt;
        std::string filename;
        std::string content;
        Task(Operation opt, const std::string &filename, const std::string &content) : opt(opt), filename(filename), content(content) {}
    };
    bool is_empty();
    inline void produce_task(const Task &);
    Task consume_task();
    void work(const Task &);
    void create_file_task(const std::string &, const std::string &);
    void merge_file_task(const std::string &, const std::string &);
    void delete_file_task(const std::string &);

private:
    std::queue<Task> tasks;
    std::mutex tasks_mutex;
    std::mutex map_mutex;
    std::unordered_map<std::string, std::mutex> file_mutex_map;
};