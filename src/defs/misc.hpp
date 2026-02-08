#pragma once
#include <string>
#include <vector>
#include "geode.hpp"

namespace misc {
    template<typename T>
    std::vector<std::vector<T>> paginate(const std::vector<T>& arr, size_t size) {
        std::vector<std::vector<T>> paginated;
        for (size_t i = 0; i < arr.size(); i += size) {
            std::vector<T> page;
            for (size_t j = i; j < std::min(i + size, arr.size()); ++j) {
                page.push_back(arr[j]);
            }
            paginated.push_back(page);
        }
        return paginated;
    }
};
