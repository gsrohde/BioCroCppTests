#ifndef TEST_UTILITY_RANDOM
#define TEST_UTILITY_RANDOM

#include <random>

using namespace std;

class Rand_int {
public:
    Rand_int(int lo, int hi);
    int operator() () const;
private:
    function<int()> r;
    static int seed_offset;
};

class Rand_double {
public:
    Rand_double( double low, double high);
    double operator() () const;
private:
    function<double()> r;
    static int seed_offset;
};

#endif





// Adapted from: Stroustrup, Bjarne (2013-07-10). The C++ Programming Language (Kindle Locations 45279-45284). Pearson Education. Kindle Edition.

int Rand_int::seed_offset = 0;

Rand_int::Rand_int(int lo, int hi)
    :r(bind( uniform_int_distribution<>(lo, hi), default_random_engine(time(nullptr) + seed_offset++))) { }


int Rand_int::operator() () const { return r(); }


int Rand_double::seed_offset = 0;

Rand_double::Rand_double( double low, double high)
    :r(bind(uniform_real_distribution<>(low, high), default_random_engine(time(nullptr) + seed_offset++))) { }

double Rand_double::operator() () const { return r(); }

