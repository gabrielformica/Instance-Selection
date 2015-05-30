#ifndef METAHEURISTIC_H
#define METAHEURISTIC_H

#include <iostream>
#include <deque>
#include <algorithm>
#include "is.h"

const double epsilon = 0.00001;

class Metaheuristic {
  public:
    virtual void optimize(const IS::Problem &T, IS::Solution &R) = 0;
    double quality(const IS::Problem &T, const IS::Solution &S, double alpha);
    double find_nearest(const IS::Problem &p, const IS::Instance &a);
    void oneNN(const IS::Problem &training, 
               const IS::Problem &result, 
               std::vector<double> &category);
    double calc_distance(const IS::Instance &a, const IS::Instance &b); 
    virtual void tweak(IS::Solution &S) = 0;
};

class HillClimbing : public Metaheuristic {
  public: 
    void optimize(const IS::Problem &T, IS::Solution &R);
    void tweak(IS::Solution &S);
};

class SimulatedAnnealing : public Metaheuristic {
  private:
    int temperature, dec_factor;
  public: 
    SimulatedAnnealing(int t, int d): temperature(t), dec_factor(d) { };
    void optimize(const IS::Problem &T, IS::Solution &R);
    void tweak(IS::Solution &S);
    void tweak(IS::Solution &S, int max_tweak);
};

class Tabu : public Metaheuristic {
  private:
    int length, number_of_tweaks;
  public:
    Tabu(int l, int n) : length(l), number_of_tweaks(n) { };
    void optimize(const IS::Problem &T, IS::Solution &R);
    void tweak(IS::Solution &S) { };
};

#endif
