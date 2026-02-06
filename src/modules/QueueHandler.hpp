#pragma once
#include <queue>

namespace QueueHandler {
    std::queue<std::string> getRateQueue();
    void addToRate(std::string);
}
