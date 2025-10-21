#include <account.h>
#include <config.h>

#include "position.h"

namespace solstice::strategy
{

Account::Account(Config& cfg) { d_balance = cfg.initialBalance(); }

std::expected<Account, std::string> Account::create()
{
    auto cfg = Config::instance();
    if (!cfg)
    {
        return std::unexpected(cfg.error());
    }

    return Account(*cfg);
}

// getters

int Account::balance() { return d_balance; }

std::vector<Position>& Account::positions() { return d_positions; }

// setters

void Account::balance(int newBalance) { d_balance = newBalance; }

void Account::addPosition(Position pos) { d_positions.emplace_back(pos); }

}  // namespace solstice::strategy
