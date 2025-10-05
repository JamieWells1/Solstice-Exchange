#include <get_random.h>

namespace solstice
{

std::random_device solstice::Random::rd;

std::string Random::getRandomUid()
{
    static std::mt19937_64 rng(std::random_device{}());
    static std::uniform_int_distribution<uint64_t> dist;
    std::string uid = std::to_string(dist(rng));

    uid.insert(uid.begin(), 20 - uid.length(), '0');

    return uid;
}

int Random::getRandomInt(int min, int max)
{
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dist(min, max);

    return dist(gen);
}

double Random::getRandomDouble(double min, double max)
{
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dist(min, max);

    double value = dist(gen);

    return std::round(value * 100.0) / 100.0;
}

int Random::getRandomBool()
{
    std::uniform_int_distribution<> dist(0, 1);
    std::mt19937 gen(rd());

    return dist(gen) == 1;
}
}  // namespace solstice
