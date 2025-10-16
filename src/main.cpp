#include "session.hpp"


int main() {
    Session s;
    s.init();

    while (s.running) {
        s.update();
        s.render();
    }

    return 0;
}
