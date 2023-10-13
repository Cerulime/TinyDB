#pragma once
#include "Include.hpp"

namespace FileOperation
{
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
    };
    extern std::queue<Task> tasks;
    extern std::mutex tasks_mutex;

    bool is_empty();
    void produce_task(const Task &);
    Task consume_task();

    void work(const Task &);
    void create_file_task(const std::string &, const std::string &);
    void merge_file_task(const std::string &, const std::string &);
    void delete_file_task(const std::string &);
}