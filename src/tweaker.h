#ifndef TWEAKER_H
#define TWEAKER_H

#include "is.h"

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


#endif
