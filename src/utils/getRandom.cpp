#include <random>

int getRandomUid() {
    static std::mt19937 rng(std::random_device{}());
    std::uniform_int_distribution<int> dist(10000000, 99999999);
    return dist(rng);
}
