#include <iostream>
#include "tweaker.h"

void oneRandomUnset::tweak(IS::Solution &S) {
    std::bitset<MAX> bits = S.getBits();
    int size = S.getSize();
    srand(time(NULL));
    int n = (rand() % bits.count()) + 1;
    int i, j;
    for (j = 0, i = 0; j < size && i < n; j++) if (bits.test(j)) i++;
    assert(bits.test(j-1));
    bits[j-1] = 0;
    S.setBits(bits);
}
void upToNRandomFlips::tweak(IS::Solution &S) {
    std::bitset<MAX> bits = S.getBits();
    int size = S.getSize();

    srand(time(NULL));
    int t = (rand() % upto) + 1;

    for(unsigned i = 0; i < t; ++i) {
        int j = (rand() % size) + 1;
        bits.flip(j);
    }
    S.setBits(bits);
}

void nRandomFlips::tweak(IS::Solution &S) {
    std::bitset<MAX> bits = S.getBits();
    int size = S.getSize();

    srand(time(NULL));
    for(unsigned i = 0; i < n; ++i) {
        int j = (rand() % size) + 1;
        bits.flip(j);
    }
    S.setBits(bits);
}

void percRandomFlips::tweak(IS::Solution &S) {
    std::bitset<MAX> bits = S.getBits();
    int size = S.getSize();
    int n = (perc * size) / 100;

    srand(time(NULL));
    for(unsigned i = 0; i < n; ++i) {
        int j = (rand() % size) + 1;
        bits.flip(j);
    }
    S.setBits(bits);
}



