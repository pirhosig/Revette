#pragma once
#include "ThreadQueue.h"



template <typename T>
class ThreadPointerQueue : public ThreadQueue<std::unique_ptr<T>>
{
};
