#include "metaheuristic.h"
#include "populationbased.h"


Population PopulationBased::generatePopulation(int size) const {
    Population population;
    for(int i = 0; i < popsize; ++i) population.insert(IS::Solution(size));
    return population;
}


void Hybrid::optimize(const IS::Dataset &ds, IS::Solution &sol) const {
    Population population = generatePopulation(sol.getSize());
    IS::Solution best = IS::Solution(*(population.begin()));
    best.setFitness(quality(ds, best, 0.5));
    int iter = 0, iter_old = 0;
    double old_q = -1.0;

    while(1) {
        iter++;
        Population::const_iterator sit;
        for (sit = population.begin(); sit != population.end(); sit++) {
            IS::Solution p = IS::Solution(*sit);
            local_search->optimize(ds, p);
            p.setFitness(quality(ds, p, 0.5));
            if (p.getFitness() > best.getFitness()) {
                best.copy(p);
            }
        }
        if (best.getFitness() - old_q < EPSILON) iter_old++;
        else {
            old_q = best.getFitness();
            iter_old = 0;
        }
        bool stop = best.getFitness() >= max_quality or iter == iterations;
        stop = stop or iter_old == no_change_best;
        if (stop) break;
        Population breeded = breed(population);
        population = breeded;
    }
    sol.copy(best);
}

Population PopulationBased::breed(const Population &p) const {
    Population breeded;
    for(int i = 0; i < popsize/2; ++i) {
        IS::Solution c1 = tourneySelection(p, 3);
        IS::Solution c2 = tourneySelection(p, 3);
        while (c1 == c2) c2 = tourneySelection(p, 3);
        Children recombination(c1, c2);
        mutation->tweak(c1);
        mutation->tweak(c2);
        breeded.insert(c1);
        breeded.insert(c2);
    }
    return breeded;
}

IS::Solution PopulationBased::tourneySelection(const Population &p, int s) const {
    Population::iterator sit = p.begin();
    advance(sit, (rand() % popsize));
    IS::Solution best = IS::Solution(*sit);

    for(int i = 0; i < s-1; ++i) {
        sit = p.begin();
        advance(sit, (rand() % popsize));
        if(best.getFitness() < (*sit).getFitness()) {
            best.copy(*sit);
        }
    }
    return best;
}

Children PopulationBased::recombination(const IS::Solution &sa, 
                                   const IS::Solution &sb) const {
    int sol_size = sa.getSize();
    int point = rand() % sol_size;
    bitset<MAX> a_bits = sa.getBits();
    bitset<MAX> b_bits = sb.getBits();
    bitset<MAX> ca, cb;    

    for(int i = 0; i < point; ++i)  {
        ca[i] = b_bits[i];
        cb[i] = a_bits[i];
    }

    for(int i = point; i < sol_size; ++i)  {
        ca[i] = a_bits[i];
        cb[i] = b_bits[i];
    }
    return make_pair(IS::Solution(ca, sol_size), IS::Solution(cb, sol_size));
}
