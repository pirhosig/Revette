#pragma once
#include <algorithm>
#include <memory>
#include <mutex>
#include <stdexcept>
#include <utility>
#include <vector>

#include <boost/circular_buffer.hpp>

#include "Core/RevetteCore.h"



template <typename T>
class ConcurrentQueue {
    boost::circular_buffer<T> queue;
    std::mutex queueMutex;

public:
    explicit ConcurrentQueue() :
        queue(4096) // Probably large enough
    {}

    void push(T&& item) {
        std::scoped_lock lock(queueMutex);
        if (queue.reserve() ==  0) {
            // Have some reasonable upper bound on the size.
            if (queue.size() > (1 << 20)) {
                throw std::runtime_error("Concurrent queue grew too large. Something else has likely gone wrong.");
            }
            queue.set_capacity(queue.size() * 2);
        }
        queue.push_back(std::move(item));
    }

    void drain(std::back_insert_iterator<std::vector<T>> out) {
        std::scoped_lock lock(queueMutex);
        std::ranges::move(queue, out);
        queue.clear();
    }
};
