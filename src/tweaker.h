/**
  * @file
  * @author Gabriel Formica <gabrielformica93@gmail.com>
  * @author Melecio Ponte <pontezambrano@gmail.com>
  *
  * @section Description
  *
  * Tweaker class
  */


#ifndef TWEAKER_H
#define TWEAKER_H

#include "is.h"

const double EPSILON = 0.00001;

/* Abstract class */
class Tweaker {
  public:
    virtual void tweak(IS::Solution &S) = 0;
};

/**
  * oneRandomUnset Class
  * Just turn off one random bit (but a bit that is on)
  */
class oneRandomUnset: public Tweaker {
  public:
    void tweak(IS::Solution &S);
};

/**
  * upToNRandomFlips
  * Apply upto Random flips on random bits of solution
  */
class upToNRandomFlips : public Tweaker {
  private:
    int upto;
  public:
    upToNRandomFlips(int ut) : upto(ut) { }
    void tweak(IS::Solution &S);
};

/**
  * nToNRandomFlips
  * Apply n Random flips on random bits of solution
  */
class nRandomFlips : public Tweaker {
  private:
    int n;
  public:
    nRandomFlips(int x) : n(x) { }
    void tweak(IS::Solution &S);
};

/**
  * perRandomFlips Class
  * Apply flip over a percentage amount of bits (percetange of size of dataset)
  * and choosen randomly
  */
class percRandomFlips : public Tweaker {
  private:
    int perc;
  public:
    percRandomFlips(int x) : perc(x) { }
    void tweak(IS::Solution &S);
};

/**
  * Choose an amount of bits (percentage of size of dataset) 
  * of bits and turn them off or on with a probability
  * @param 'S'  :  Solution 
  */
class weightedRandom: public Tweaker {
  private:
    int perc;
    int reduc_weight;
  public:
    weightedRandom(int p, int r) : perc(p), reduc_weight(r){ }
    void tweak(IS::Solution &S);
};

/**
  * Choose an amount of bits (percentage of size of dataset) 
  * of bits and turn them off or on with a probability, but, if 
  * a bit has to be turned off, then it has to be ramdon bit on.
  * If a bit has to be turned on, then it has to be a random bit off.
  * @param 'S'  :  Solution 
  */
class weightedRandomPlus: public Tweaker {
  private:
    int perc;
    int reduc_weight;
  public:
    weightedRandomPlus(int p, int r) : perc(p), reduc_weight(r){ }
    void tweak(IS::Solution &S);
};

#endif
