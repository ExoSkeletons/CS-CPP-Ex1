//
// Created by Aviad Levine on 30/03/2025.
//


#include <iostream>
#include <thread>

void sleep_milli(const long t) {
    std::this_thread::sleep_for(std::chrono::milliseconds(t));
}

void sleep(const long t) {
    std::this_thread::sleep_for(std::chrono::seconds(t));
}

void ellipse(const int n) {
    for (int i = 0; i < n; i++) {
        std::cout << '.';
        sleep_milli(750);
    }
}

void slow_type(const std::string &s) {
    for (int i = 0; i < (int) s.length(); i++) {
        std::cout << s[i];
        sleep_milli(200);
    }
}

int main() {
    std::cout << "Hello World!\n";

    sleep((2));
    ellipse(4);
    sleep((2));
    ellipse(3);
    sleep((1));
    std::cout << std::endl;
    slow_type("Ummmmm");
    ellipse(4);
    sleep((2));
    std::cout << std::endl;
    std::cout << "So..";
    ellipse(3);
    slow_type("uhhhhhhhh");
    ellipse(6);
    slow_type("yeah.");
    std::cout << std::endl;
    sleep((2));
    std::cout << "BYEEE";
    sleep((1));

    return 0;
}
