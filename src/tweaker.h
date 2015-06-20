#ifndef TWEAKER_H
#define TWEAKER_H

#include "is.h"

const double EPSILON = 0.00001;

class Tweaker {
  public:
    virtual void tweak(IS::Solution &S) = 0;
};

class oneRandomUnset: public Tweaker {
  public:
    void tweak(IS::Solution &S);
};

class upToNRandomFlips : public Tweaker {
  private:
    int upto;
  public:
    upToNRandomFlips(int ut) : upto(ut) { }
    void tweak(IS::Solution &S);
};

class nRandomFlips : public Tweaker {
  private:
    int n;
  public:
    nRandomFlips(int x) : n(x) { }
    void tweak(IS::Solution &S);
};

class percRandomFlips : public Tweaker {
  private:
    int perc;
  public:
    percRandomFlips(int x) : perc(x) { }
    void tweak(IS::Solution &S);
};

class weightedRandom: public Tweaker {
  private:
    int perc;
    int reduc_weight;
  public:
    weightedRandom(int p, int r) : perc(p), reduc_weight(r){ }
    void tweak(IS::Solution &S);
};

class weightedRandomPlus: public Tweaker {
  private:
    int perc;
    int reduc_weight;
  public:
    weightedRandomPlus(int p, int r) : perc(p), reduc_weight(r){ }
    void tweak(IS::Solution &S);
};

#endif
