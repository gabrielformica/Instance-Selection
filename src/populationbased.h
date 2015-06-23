#ifndef POPULATIONBASED_H
#define POPULATIONBASED_H

#include <set>
#include "metaheuristic.h"
#include "is.h"

typedef pair<IS::Solution, IS::Solution> Children;
typedef set<IS::Solution> Population;
using namespace std;

class PopulationBased : public Metaheuristic {
  protected:
    int popsize;
    Metaheuristic *local_search;
  public:
    PopulationBased(int p, Metaheuristic *ls) : popsize(p), local_search(ls) { };
    set<IS::Solution> breed(const IS::Dataset &ds, const set<IS::Solution> &p) const;
    pair<IS::Solution, IS::Solution> recombination(const IS::Solution &sa, 
                                                   const IS::Solution &sb) 
                                                   const;
    Population selection(const Population &sa, const Population &sb) const;
    void generatePopulation(Population &p, int size) const;
    IS::Solution tourneySelection(const set<IS::Solution> &p, int s) const;
};

class Hybrid : public PopulationBased {
  public:
    void optimize(const IS::Dataset &ds, IS::Solution &sol) const;
    std::string output_params() const { return ""; };
    Hybrid(int p, Metaheuristic *ls): PopulationBased(p, ls) { };
    // set<IS::Solution> breed(set<IS::Solution> &p);
};

#endif
