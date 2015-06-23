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
    set<IS::Solution> breed(const set<IS::Solution> &p) const;
    Children recombination(const IS::Solution &sa, 
                                                   const IS::Solution &sb) 
                                                   const;
    Population selection(const Population &sa, const Population &sb) const;
    void generatePopulation(Population &population, int size, const IS::Dataset &ds) const;
    IS::Solution tourneySelection(const set<IS::Solution> &p, int s) const;
    IS::Solution findBest(const Population &p) const;
};

class Hybrid : public PopulationBased {
  public:
    void optimize(const IS::Dataset &ds, IS::Solution &sol) const;
    std::string output_params() const { return ""; };
    Hybrid(int p, Metaheuristic *ls): PopulationBased(p, ls) { };
};

class SGA : public PopulationBased {
  protected:
    double cross_p;
    double mutation_p;
  public:
    void optimize(const IS::Dataset &ds, IS::Solution &sol) const;
    std::string output_params() const { return ""; };
    SGA(int p, Metaheuristic *ls, double cp, double mp): 
        PopulationBased(p, ls), cross_p(cp), mutation_p(mp) { };
    SGA(int p, Metaheuristic *ls) : PopulationBased(p, ls) { };
};

#endif
