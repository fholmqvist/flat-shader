#include "session.hpp"

using namespace std::chrono;

int main() {
    Session s;

    auto frame_time = duration<double>(1.0 / 60.0);
    auto next_frame = steady_clock::now();

    while (s.running) {
        auto start = steady_clock::now();

        s.update();
        s.render();

        next_frame += duration_cast<steady_clock::duration>(frame_time);
        std::this_thread::sleep_until(next_frame);
    }

    return 0;
}
