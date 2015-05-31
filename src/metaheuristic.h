#ifndef METAHEURISTIC_H
#define METAHEURISTIC_H

#include <iostream>
#include <deque>
#include <algorithm>
#include "is.h"
#include "tweaker.h"

const double epsilon = 0.00001;

class Metaheuristic {
  protected:
    Tweaker *tweaker;
  public:
    virtual void optimize(const IS::Dataset &T, IS::Solution &R) = 0;
    void setTweaker(Tweaker *tw) { tweaker = tw; };
    Tweaker* getTweaker() { return tweaker; };
    double quality(const IS::Dataset &T, const IS::Solution &S, double alpha);
    double find_nearest(const IS::Dataset &p, const IS::Instance &a);
    void oneNN(const IS::Dataset &training, 
               const IS::Dataset &result, 
               std::vector<double> &category);
    double calc_distance(const IS::Instance &a, const IS::Instance &b); 
};

class HillClimbing : public Metaheuristic {
  public: 
    void optimize(const IS::Dataset &T, IS::Solution &R);
};

class SimulatedAnnealing : public Metaheuristic {
  private:
    int temperature, dec_factor;
  public: 
    SimulatedAnnealing(int t, int d): temperature(t), dec_factor(d) { };
    void optimize(const IS::Dataset &T, IS::Solution &R);
};


class ILS : public Metaheuristic {
  private:
    int li, ti, pp;
  public: 
    ILS(int l, int t, int p) : li(l), ti(t), pp(p) { };
    int  getLocalIter() { return li; }
    int  getTotalIter() { return ti; }
    int  getPerturbPerc() { return pp; }
    void setLocalIter(int l) { li = l; }
    void setTotalIter(int t) { ti = t; }
    void setPerturbPerc(int p) { pp = p; }
    void optimize(const IS::Dataset &T, IS::Solution &R);
};

class Tabu : public Metaheuristic {
  private:
    int length, number_of_tweaks;
  public:
    Tabu(int l, int n) : length(l), number_of_tweaks(n) { };
    void optimize(const IS::Dataset &T, IS::Solution &R);
};


#endif
