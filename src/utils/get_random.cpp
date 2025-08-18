#include <get_random.h>

using namespace solstice;

std::random_device solstice::Random::rd;

std::string Random::getRandomUid() {
    static std::mt19937_64 rng(std::random_device{}());
    static std::uniform_int_distribution<uint64_t> dist;
    return std::to_string(dist(rng));
}

double Random::getRandomNumber(double min, double max) {
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dist(min, max);

    return dist(gen);
}

int Random::getRandomBool() {
    std::uniform_int_distribution<> dist(0, 1);
    std::mt19937 gen(rd());

    return dist(gen);
}
