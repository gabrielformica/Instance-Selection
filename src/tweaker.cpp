#include <iostream>
#include "tweaker.h"

void oneRandomUnset::tweak(IS::Solution &S) {
    std::bitset<MAX> bits = S.getBits();
    int size = S.getSize();
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

    int t = (rand() % upto) + 1;

    for (int i = 0; i < t; ++i) {
        int j = (rand() % size);
        bits.flip(j);
    }
    S.setBits(bits);
}

void nRandomFlips::tweak(IS::Solution &S) {
    std::bitset<MAX> bits = S.getBits();
    int size = S.getSize();

    for (int i = 0; i < n; ++i) {
        int j = (rand() % size);
        bits.flip(j);
    }
    S.setBits(bits);
}

void percRandomFlips::tweak(IS::Solution &S) {
    std::bitset<MAX> bits = S.getBits();
    int size = S.getSize();
    int n = (perc * size) / 100;

    for (int i = 0; i < n; ++i) {
        int j = (rand() % size);
        cout << "j = " << j << endl;
        bits.flip(j);
    }
    S.setBits(bits);
}

void weightedRandom::tweak(IS::Solution &S) {
    std::bitset<MAX> bits = S.getBits();
    int size = S.getSize();
    int n = (perc * size) / 100;
    double reduc = ((double) reduc_weight) / 100.0;

    for (int i = 0; i < n; ++i) {
        int j = (rand() % size);
        double prob = ((double) rand() / (double) RAND_MAX);
        bits[j] = (reduc - prob > EPSILON) ? 0 : 1;
    }
    S.setBits(bits);
}

void weightedRandomPlus::tweak(IS::Solution &S) {
    std::bitset<MAX> bits = S.getBits();
    int size = S.getSize();
    int n = (perc * size) / 100;

    for (int i = 0; i < n; ++i) {
        int j = (rand() % size);
        bits.flip(j);
    }
    S.setBits(bits);
}
