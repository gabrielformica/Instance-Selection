/**
  * @file
  * @author Gabriel Formica <gabrielformica93@gmail.com>
  * @author Melecio Ponte <pontezambrano@gmail.com>
  *
  * @section Description
  *
  * Population-based algorithm classes
  */
#ifndef POPULATIONBASED_H
#define POPULATIONBASED_H

#include <set>
#include "metaheuristic.h"
#include "is.h"

/* Some alias */
typedef pair<IS::Solution, IS::Solution> Children;
typedef set<IS::Solution> Population;

using namespace std;

/* Abstrac Class */
class PopulationBased : public Metaheuristic {
  protected:
    int popsize;                  // Population size
    Metaheuristic *local_search;  // Local search
  public:
    /* Class constructor */
    PopulationBased(int p, Metaheuristic *ls) : popsize(p), local_search(ls) { };

    /* Basic Methods */
    set<IS::Solution> breed(const IS::Dataset &ds, const set<IS::Solution> &p) const;

    pair<IS::Solution, IS::Solution> recombination(const IS::Solution &sa, 
                                                   const IS::Solution &sb) 
                                                   const;

    Population selection(const Population &sa, const Population &sb) const;

    void generatePopulation(Population &p, int size, const IS::Dataset &ds) const;

    IS::Solution tourneySelection(const set<IS::Solution> &p, int s) const;

    /* Basic Methods */

    IS::Solution findBest(const Population &p) const;
};

/* Memetic implementation */
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
