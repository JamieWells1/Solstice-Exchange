#include <config.h>
#include <orchestrator.h>
#include <order_book.h>

#include <iostream>

using namespace solstice;

int main()
{
    std::string choice;
    std::cout << "Enter any key to start order flow: ";
    std::cin >> choice;

    if (!choice.empty())
    {
        auto response = solstice::matching::Orchestrator::start();

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
