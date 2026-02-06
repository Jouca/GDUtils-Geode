#include "QueueHandler.hpp"

std::queue<std::string> rateQueue;

std::queue<std::string> QueueHandler::getRateQueue() {
    return rateQueue;
};
void QueueHandler::addToRate(std::string value) {
    rateQueue.push(value);
}
