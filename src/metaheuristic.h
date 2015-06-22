#ifndef METAHEURISTIC_H
#define METAHEURISTIC_H

#include <iostream>
#include <deque>
#include <algorithm>
#include "is.h"
#include "tweaker.h"

class Metaheuristic {
  protected:
    Tweaker *tweaker;
    double max_quality = 0.95; 
  public:
    virtual void optimize(const IS::Dataset &T, IS::Solution &R) const = 0;
    virtual std::string output_params() const = 0;
    void setTweaker(Tweaker *tw) { tweaker = tw; };
    Tweaker* getTweaker() const { return tweaker; };
    double quality(const IS::Dataset &T, const IS::Solution &S, double alpha) const;
    double find_nearest(const IS::Dataset &p, const IS::Instance &a) const ;
    void oneNN(const IS::Dataset &training, 
               const IS::Dataset &result, 
               std::vector<double> &category) const;
    double calc_distance(const IS::Instance &a, const IS::Instance &b) const; 
    double get_max_quality() const { return max_quality; };
    void set_max_quality(double x) { max_quality = x; }
};

class HillClimbing : public Metaheuristic {
  public: 
    void optimize(const IS::Dataset &T, IS::Solution &R) const;
    std::string output_params() const;
};

class SimulatedAnnealing : public Metaheuristic {
  private:
    int temperature, dec_factor;
  public: 
    SimulatedAnnealing(int t, int d): temperature(t), dec_factor(d) { };
    void optimize(const IS::Dataset &T, IS::Solution &R) const;
    std::string output_params() const;
};

class ILS : public Metaheuristic {
  private:
    int li, ti, pp;
  public: 
    ILS(int l, int t, int p) : li(l), ti(t), pp(p) { };
    int  getLocalIter() const { return li; }
    int  getTotalIter() const { return ti; }
    int  getPerturbPerc() const { return pp; }
    void setLocalIter(int l) { li = l; }
    void setTotalIter(int t) { ti = t; }
    void setPerturbPerc(int p) { pp = p; }
    void optimize(const IS::Dataset &T, IS::Solution &R) const;
    std::string output_params() const;
};

class Tabu : public Metaheuristic {
  private:
    int length, number_of_tweaks, iter_limit;
  public:
    Tabu(int l, int n, int i) : length(l), number_of_tweaks(n), iter_limit(i) { };
    void optimize(const IS::Dataset &T, IS::Solution &R) const;
    std::string output_params() const;
};


#endif
