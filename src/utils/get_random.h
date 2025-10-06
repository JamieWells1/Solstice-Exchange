#ifndef GETRANDOM_H
#define GETRANDOM_H

#include <random>
#include <string>

namespace solstice
{

class Random
{
   public:
    static std::string getRandomUid();

    static int getRandomInt(int min, int max);
    static double getRandomDouble(double min, double max);

    static int getRandomBool();

   private:
    static std::random_device rd;
};

}  // namespace solstice

#endif  // GETRANDOM_H
