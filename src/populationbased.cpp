#include "metaheuristic.h"
#include "populationbased.h"
#include "utils.h"


void PopulationBased::generatePopulation(Population &population, int size) const {
    while (population.empty() || population.size() < popsize)
        population.insert(IS::Solution(size));
}


void Hybrid::optimize(const IS::Dataset &ds, IS::Solution &sol) const {
    std::cout << ">>>> Running Hybrid algorithm" << std::endl;
    Population population;
    generatePopulation(population, sol.getSize());
    IS::Solution best = IS::Solution(*(population.begin()));
    
    best.setFitness(quality(ds, best, 0.5));

    int iter = 0, no_change = 0;
    while(1) {  // Main loop
        iter++;
        Population::const_iterator sit;
        Population new_pop;  // New population 
        bool best_changed = false;
        for (sit = population.begin(); sit != population.end(); sit++) {
            local_search->set_max_quality(std::min(best.getFitness() + 0.95, 0.95));
            IS::Solution p = IS::Solution(*sit);
            local_search->optimize(ds, p);
            p.setFitness(quality(ds, p, 0.5)); 
            new_pop.insert(p);
            if (p.getFitness() > best.getFitness()) {
                best.copy(p);
                best.setFitness(p.getFitness());
                best_changed = true;
            }
        }
        if (! best_changed) {
            for (int i = 0; i < 10; i++) {
                std::cout << BOLDGREEN << "HHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHH";
                std::cout  << RESET << std::endl;
            }
            no_change++;
        } else {
            no_change = 0;
        }
        population = new_pop;
        generatePopulation(population, sol.getSize());
        assert(population.size() == popsize);
        
        bool stop = best.getFitness() > max_quality or iter == ite_limit;
        stop = stop or no_change == no_change_best;
        for (int i = 0; i < 30; i++) {
            std::cout << CYAN << "KKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKK";
            std::cout  << RESET << std::endl;
        }
        if (stop) break;
        Population breeded = breed(ds, population);
    }
    sol.copy(best);
}

Population PopulationBased::breed(const IS::Dataset &ds, const Population &p) const {
    Population breeded;
    for (int i = 0; i < popsize/2; ++i) {
        IS::Solution p1 = tourneySelection(p, 3);
        IS::Solution p2 = tourneySelection(p, 3);
        while (p1 == p2) p2 = tourneySelection(p, 3);
        Children children = recombination(p1, p2);
        IS::Solution c1(children.first), c2(children.second);
        tweaker->tweak(c1);
        tweaker->tweak(c2);
        double q_children = quality(ds, c1, 0.5) + quality(ds, c2, 0.5);
        double q_parents = quality(ds, p1, 0.5) + quality(ds, p2, 0.5);
        if (q_children > q_parents) {  // Elitist
            breeded.insert(c1);
            breeded.insert(c2);
        } else {
            breeded.insert(p1);
            breeded.insert(p2);
        }
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
        if (best.getFitness() < (*sit).getFitness()) {
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
