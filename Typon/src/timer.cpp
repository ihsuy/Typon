
#include "timer.hpp"

#include <chrono>
#include <ctime>

using namespace std;

void timer::start() {
    if (started) {
        return;
    }

    started = true;
    stopped = false;

    start_t = chrono::high_resolution_clock::now();

    full_record.push_back(start_t);
}

double timer::split() {
    auto now = chrono::high_resolution_clock::now();

    return chrono::duration_cast<chrono::duration<double>>(now - start_t)
        .count();
}

void timer::update() {
    full_record.push_back(chrono::high_resolution_clock::now());
}

void timer::stop() {
    if (stopped) {
        return;
    }

    stopped = true;
    started = false;

    stop_t = chrono::high_resolution_clock::now();
    full_record.push_back(stop_t);
}

double timer::total_duration_in_sec() {
    if (not stopped) {
        throw runtime_error(
            "Can't calculate total diration before stopping the timer");
    }
    return chrono::duration_cast<chrono::duration<double>>(stop_t - start_t)
        .count();
}

void timer::reset() {
    start_t = chrono::high_resolution_clock::time_point{};
    stop_t = chrono::high_resolution_clock::time_point{};
    full_record = vector<chrono::high_resolution_clock::time_point>{};

    stopped = true;
    started = false;
}

vector<double> timer::durations_in_sec() {
    vector<double> all_durations{};
    for (unsigned int i = 1; i < full_record.size(); ++i) {
        all_durations.push_back(chrono::duration_cast<chrono::duration<double>>(
                                    full_record[i] - full_record[i - 1])
                                    .count());
    }
    return all_durations;
}

vector<long long> timer::durations_in_ms() {
    vector<long long> all_durations{};
    for (unsigned int i = 1; i < full_record.size(); ++i) {
        all_durations.push_back(chrono::duration_cast<chrono::milliseconds>(
                                    full_record[i] - full_record[i - 1])
                                    .count());
    }
    return all_durations;
}

vector<chrono::duration<double>> timer::durations() {
    vector<chrono::duration<double>> all_durations{};
    for (unsigned int i = 1; i < full_record.size(); ++i) {
        all_durations.push_back(chrono::duration_cast<chrono::milliseconds>(
            full_record[i] - full_record[i - 1]));
    }
    return all_durations;
}
