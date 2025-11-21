#include <broadcaster.h>
#include <config.h>
#include <orchestrator.h>
#include <order_book.h>

#include <atomic>
#include <iostream>
#include <optional>
#include <thread>

using namespace solstice;

int main()
{
    auto config = Config::instance();

    if (!config)
    {
        std::cout << "\n[FATAL]: " << config.error() << std::endl;
        return -1;
    }

    // start broadcaster
    std::optional<broadcaster::Broadcaster> broadcaster;
    if (config->enableBroadcaster())
    {
        broadcaster.emplace(8080);
        std::cout << "Broadcaster started on port 8080.\n" << std::endl;
    }

    std::string choice;
    std::cout << "Enter any key to start order flow.\n";
    std::cin >> choice;

    if (!choice.empty())
    {
        bool infiniteMode = (config->ordersToGenerate() == -1);

        if (infiniteMode)
        {
            std::cout << "Running in infinite mode. Press Enter to stop...\n" << std::endl;
        }

        // For infinite mode, spawn a thread to wait for key press
        if (infiniteMode)
        {
            std::thread inputThread([]()
                                    {
                std::cin.ignore();
                std::cin.get();
                matching::Orchestrator::requestStop();
            });
            inputThread.detach();
        }

        auto response = matching::Orchestrator::start(broadcaster);

        if (!response)
        {
            std::cout << "\n[FATAL]: " << response.error() << std::endl;
            return -1;
        }

        std::cout << std::endl;

        return 0;
    }

    return -1;
}
