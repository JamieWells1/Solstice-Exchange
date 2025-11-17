#include <broadcaster.h>
#include <config.h>
#include <orchestrator.h>
#include <order_book.h>

#include <iostream>
#include <optional>

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
        auto response = matching::Orchestrator::start(broadcaster);

        if (!response)
        {
            std::cout << "\n[FATAL]: " << response.error() << std::endl;
            return -1;
        }

        if (broadcaster.has_value())
        {
            std::cout << "\n\nOrder flow complete. Broadcaster still running." << std::endl;
            std::cout << "Press Enter to exit..." << std::endl;
            std::cin.ignore();
            std::cin.get();
        }

        return 0;
    }

    return -1;
}
