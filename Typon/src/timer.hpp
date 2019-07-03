
#ifndef timer_hpp
#define timer_hpp

#include <chrono>
#include <stdexcept>
#include <vector>

using namespace std;

struct timer {
    chrono::high_resolution_clock::time_point start_t{};
    chrono::high_resolution_clock::time_point stop_t{};

    vector<chrono::high_resolution_clock::time_point> full_record;

    void start();

    double split();

    void update();

    void stop();

    double total_duration_in_sec();

    void reset();

    void pause();  // not yet implemented

    void resume();  // not yet implemented

    vector<double> durations_in_sec();

    vector<long long> durations_in_ms();

    vector<chrono::duration<double>> durations();

    bool isStopped() { return stopped; }
    bool isStarted() { return started; }

   private:
    bool stopped = true;
    bool started = false;
};

#endif /* timer_hpp */
