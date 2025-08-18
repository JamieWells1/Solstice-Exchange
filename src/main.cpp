#include <config.h>
#include <order_book.h>
#include <order_processor.h>

#include <iostream>

using namespace solstice;

int main() {
    std::string choice;
    std::cout << "Enter any key to start order flow: ";
    std::cin >> choice;

    if (!choice.empty()) {
        OrderProcessor::start();
    }

    return 0;
}
