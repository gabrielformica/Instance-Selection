/**
  * @file
  * @author Gabriel Formica <gabrielformica93@gmail.com>
  * @author Melecio Ponte <pontezambrano@gmail.com>
  *
  * @section Description
  *
  * Metaheuristics' classes
  */


#ifndef METAHEURISTIC_H
#define METAHEURISTIC_H

#include <iostream>
#include <deque>
#include <algorithm>
#include "is.h"
#include "tweaker.h"

/* Abstract class */
class Metaheuristic {
  protected:
    Tweaker *tweaker;
    double max_quality = 0.95; 
    int ite_limit = 1000;
    int no_change_best = 500;
  public:
    /* Optimizes is basically apply the metaheuristic */
    virtual void optimize(const IS::Dataset &T, IS::Solution &R) const = 0;

    /* Getters */
    Tweaker* getTweaker() const { return tweaker; };
    double get_max_quality() const { return max_quality; };

    /* Basic functions */
    virtual std::string output_params() const = 0;
    double quality(const IS::Dataset &T, const IS::Solution &S, double alpha) const;
    double find_nearest(const IS::Dataset &p, const IS::Instance &a) const ;
    void oneNN(const IS::Dataset &training, 
               const IS::Dataset &result, 
               std::vector<double> &category) const;
    double calc_distance(const IS::Instance &a, const IS::Instance &b) const; 

    /* Setters */
    void setTweaker(Tweaker *tw) { tweaker = tw; };
    void set_max_quality(double x) { max_quality = x; }
    void set_no_change_best(int x) { no_change_best = x; }
    void set_ite_limit(int x) {ite_limit = x; }
};

/* Hill Climbing */
class HillClimbing : public Metaheuristic {
  public: 
    HillClimbing(int i, int nc) { 
        ite_limit = i; 
        no_change_best = nc;
    };
    void optimize(const IS::Dataset &T, IS::Solution &R) const;
    std::string output_params() const;
};

/* Iterated Local Search */

class ILS : public Metaheuristic {
  private:
    // Local iterations and Perturb percentage
    int li, pp;
  public: 
    ILS(int l, int t, int i, int nc) : li(l), pp(t) { 
        ite_limit = i; 
        no_change_best = nc;
    };
    /* Getters */
    int  getLocalIter() const { return li; }
    int  getPerturbPerc() const { return pp; }

    /* Setters */
    void setLocalIter(int l) { li = l; }
    void setPerturbPerc(int p) { pp = p; }

    void optimize(const IS::Dataset &T, IS::Solution &R) const;
    std::string output_params() const;
};

/* Tabu search */
class Tabu : public Metaheuristic {
  private:
    // Length of the list, and number of tweaks to be applied 
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
