#include <order_book.h>
#include <processor.h>

#include <iostream>

using namespace solstice;

int main() {
    std::string choice;
    std::cout << "Enter any key to start order flow: ";
    std::cin >> choice;

    if (!choice.empty()) {
        Processor::start();
    }

    return 0;
}
