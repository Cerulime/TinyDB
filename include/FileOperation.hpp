#pragma once
#include "Include.hpp"

namespace FileOperation
{
    enum class Operation
    {
        CREATE,
        APPEND,
        MODIFY,
        DELETE
    };
    struct Task
    {
        Operation opt;
        std::string filename;
        std::string content;
    };
    std::queue<Task> tasks;
    std::mutex tasks_mutex;
    std::condition_variable_any tasks_cv;

    void produce_task(const Task &);
    Task consume_task();

    void merge_file(const std::string &, const std::string &);
}