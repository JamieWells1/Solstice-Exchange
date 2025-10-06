#include <config.h>
#include <order_book.h>
#include <order_processor.h>

#include <iostream>

using namespace solstice::matching;

int main()
{
    std::string choice;
    std::cout << "Enter any key to start order flow: ";
    std::cin >> choice;

    if (!choice.empty())
    {
        auto response = OrderProcessor::start();

        if (!response)
        {
            std::cout << "\n[FATAL]: " << response.error() << std::endl;
            return -1;
        }
    }
    else
    {
        return -1;
    }

    return 0;
}
