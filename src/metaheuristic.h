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
    int ite_limit = 1000;
    int no_change_best = 500;
  public:
    virtual void optimize(const IS::Dataset &T, IS::Solution &R) const = 0;
    virtual std::string output_params() const = 0;
    Tweaker* getTweaker() const { return tweaker; };
    double quality(const IS::Dataset &T, const IS::Solution &S, double alpha) const;
    double find_nearest(const IS::Dataset &p, const IS::Instance &a) const ;
    void oneNN(const IS::Dataset &training, 
               const IS::Dataset &result, 
               std::vector<double> &category) const;
    double calc_distance(const IS::Instance &a, const IS::Instance &b) const; 
    double get_max_quality() const { return max_quality; };
    void setTweaker(Tweaker *tw) { tweaker = tw; };
    void set_max_quality(double x) { max_quality = x; }
    void set_no_change_best(int x) { no_change_best = x; }
    void set_ite_limit(int x) {ite_limit = x; }
};

class HillClimbing : public Metaheuristic {
  public: 
    HillClimbing(int i, int nc) { 
        ite_limit = i; 
        no_change_best = nc;
    };
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
    int li, pp;
  public: 
    ILS(int l, int t, int i, int nc) : li(l), pp(t) { 
        ite_limit = i; 
        no_change_best = nc;
    };
    int  getLocalIter() const { return li; }
    int  getPerturbPerc() const { return pp; }
    void setLocalIter(int l) { li = l; }
    void setPerturbPerc(int p) { pp = p; }
    void optimize(const IS::Dataset &T, IS::Solution &R) const;
    std::string output_params() const;
};

class Tabu : public Metaheuristic {
  private:
    int length, number_of_tweaks;
  public:
    Tabu(int l, int n, int i, int nc) : length(l), number_of_tweaks(n) { 
        ite_limit = i; 
        no_change_best = nc;
    };
    void optimize(const IS::Dataset &T, IS::Solution &R) const;
    std::string output_params() const;
};

#endif
