/**
  * @file
  * @author Gabriel Formica <gabrielformica93@gmail.com>
  * @author Melecio Ponte <pontezambrano@gmail.com>
  *
  * @section Description
  *
  * Tweaker class
  */

#include <iostream>
#include "tweaker.h"

/**
  * Just turn off one random bit (but a bit that is on)
  * @param 'S'  :  Solution 
  */
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

/**
  * Apply upto Random flips on random bits of solution
  * @param 'S'  :  Solution 
  */
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

/**
  * Apply n Random flips on random bits of solution
  * @param 'S'  :  Solution 
  */
void nRandomFlips::tweak(IS::Solution &S) {
    std::bitset<MAX> bits = S.getBits();
    int size = S.getSize();

    for (int i = 0; i < n; ++i) {
        int j = (rand() % size);
        bits.flip(j);
    }
    S.setBits(bits);
}

/**
  * Apply flip over an amount of bits (percetange of size of dataset)
  * and choosen randomly
  * @param 'S'  :  Solution 
  */
void percRandomFlips::tweak(IS::Solution &S) {
    std::bitset<MAX> bits = S.getBits();
    int size = S.getSize();
    int n = (perc * size) / 100;

    for (int i = 0; i < n; ++i) {
        int j = (rand() % size);
        bits.flip(j);
    }
    S.setBits(bits);
}

/**
  * Choose an amount of bits (percentage of size of dataset) 
  * of bits and turn them off or on with a probability
  * @param 'S'  :  Solution 
  */
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

/**
  * Choose an amount of bits (percentage of size of dataset) 
  * of bits and turn them off or on with a probability, but, if 
  * a bit has to be turned off, then it has to be ramdon bit on.
  * If a bit has to be turned on, then it has to be a random bit off.
  * @param 'S'  :  Solution 
  */
void weightedRandomPlus::tweak(IS::Solution &S) {
    std::bitset<MAX> bits = S.getBits();
    int size = S.getSize();
    int n = (perc * size) / 100;

    double reduc = ((double) reduc_weight) / 100.0;
    for (int k = 0; k < n; k++) {
        double prob = ((double) rand() / (double) RAND_MAX);
        // Select a random bit on, and set it off 
        if (bits.count() > 0 and reduc - prob > EPSILON) { 
            int random_bit_on = (rand() % bits.count()) + 1;
            int i, j;
            for (j = 0, i = 0; j < size && i < random_bit_on; j++) 
                if (bits.test(j)) i++;
            assert(bits.test(j-1));
            bits[j-1] = 0;
        } else { // Select a random bit off, and set it on
            int random_bit_off = (rand() % (size - bits.count())) + 1;
            int i, j;
            for (j = 0, i = 0; j < size && i < random_bit_off; j++) 
                if (! bits.test(j)) i++;
            assert(! bits.test(j-1));
            bits[j-1] = 1;
        }
    }
    S.setBits(bits);
}
