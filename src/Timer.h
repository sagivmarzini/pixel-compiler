#ifndef TIMER_H
#define TIMER_H


#include <chrono>
#include <iostream>

using std::chrono::high_resolution_clock;

class Timer
{
public:
    Timer() = default;
    ~Timer() = default;

    void start();
    void stop(std::string proccessName);
private:
    std::chrono::_V2::system_clock::time_point startTime;
};


void Timer::start()
{
    startTime = high_resolution_clock::now();
}

void Timer::stop(std::string proccessName)
{  
    auto endTime = high_resolution_clock::now();

    auto result = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime);

    std::cout << "completed " << proccessName << " in " << result.count() << "ms\n"; 
}

#endif //TIMER_H