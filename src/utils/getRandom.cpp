#include <random>

std::string getRandomUid() {
    static std::mt19937_64 rng(std::random_device{}());
    static std::uniform_int_distribution<uint64_t> dist;
    return std::to_string(dist(rng));
}
