#include "metaheuristic.h"
#include "populationbased.h"
#include "utils.h"


void PopulationBased::generatePopulation(Population &population, int size, const IS::Dataset &ds) const {
    while (population.empty() || population.size() < popsize) {
        IS::Solution sol = IS::Solution(size);
        sol.setFitness(quality(ds, sol, 0.5));
        population.insert(sol);
    }
}


void Hybrid::optimize(const IS::Dataset &ds, IS::Solution &sol) const {
    std::cout << ">>>> Running Hybrid algorithm" << std::endl;
    Population population;
    generatePopulation(population, sol.getSize(), ds);
    IS::Solution best = IS::Solution(*(population.begin()));
    double best_fitness = quality(ds, best, 0.5);
    best.setFitness(best_fitness);
    int iter = 0, iter_old = 0;
    double old_best = -1.0;

    while(1) {
        iter++;
        Population::const_iterator sit;
        local_search->set_max_quality(std::min(best.getFitness() + 0.1, 0.95));
        Population new_pop;
        for (sit = population.begin(); sit != population.end(); sit++) {
            IS::Solution p = IS::Solution(*sit);
            local_search->optimize(ds, p);
            double p_fitness = quality(ds, p, 0.5);
            p.setFitness(p_fitness);
            new_pop.insert(p);
            if (p_fitness > best_fitness) {
                best.copy(p);
                best_fitness = p_fitness;
            }
        }
        if (best_fitness - old_best < EPSILON) {
            for (int i = 0; i < 10; i++) {
                std::cout << BOLDGREEN << "HHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHH";
                std::cout  << RESET << std::endl;
            }
            iter_old++;
        }
        else {
            std::cout << "BEST = " << best_fitness << " , old_best = " << old_best << endl;
            old_best = best_fitness;
            iter_old = 0;
        }
        population = new_pop;
        generatePopulation(population, sol.getSize(), ds);
        assert(population.size() == popsize);
        
        bool stop = best_fitness > max_quality or iter == ite_limit;
        stop = stop or iter_old == no_change_best;
        for (int i = 0; i < 10; i++) {
            std::cout << CYAN << "KKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKK";
            std::cout  << RESET << std::endl;
        }
        if (stop) break;
        Population breeded = breed(population);
    }
    sol.copy(best);
}

Population PopulationBased::breed(const Population &p) const {
    Population breeded;
    for (int i = 0; i < popsize/2; ++i) {
        IS::Solution c1 = tourneySelection(p, 3);
        IS::Solution c2 = tourneySelection(p, 3);
        while (c1 == c2) c2 = tourneySelection(p, 3);
        Children recombination(c1, c2);
        tweaker->tweak(c1);
        tweaker->tweak(c2);
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


void SGA::optimize(const IS::Dataset &ds, IS::Solution &sol) const {
    Population population;
    generatePopulation(population, sol.getSize(), ds);
    IS::Solution best = findBest(population);
    double best_fitness = quality(ds, best, 0.5);
    best.setFitness(quality(ds, best, 0.5));
    int iter = 0, iter_old = 0;
    double old_q = -1.0;

    while(1) {
        iter++;
        IS::Solution p1 = tourneySelection(population, 3);
        IS::Solution p2 = tourneySelection(population, 3);
        IS::Solution p1_cand = IS::Solution(p1);
        IS::Solution p2_cand = IS::Solution(p2);
        Children recombination(p1_cand, p2_cand);
        // tweaker->tweak(p1_cand);
        // tweaker->tweak(p2_cand);
        p1_cand.setFitness(quality(ds, p1_cand, 0.5));
        p2_cand.setFitness(quality(ds, p2_cand, 0.5));
        if(p1.getFitness() < p1_cand.getFitness() ) {
            cout << BOLDGREEN << "GOOD CANDIDATE 1" << RESET << endl;
            population.erase(p1);
            population.insert(p1_cand);             
        }
        if(p2.getFitness() < p2_cand.getFitness() ) {
            cout << BOLDGREEN << "GOOD CANDIDATE 2" << RESET << endl;
            population.erase(p2);
            population.insert(p2_cand);             
        }
        IS::Solution candidate = findBest(population);
        if (candidate.getFitness() > best.getFitness()) {
            cout << BOLDGREEN << "GOT BETTER" << RESET << endl;
            best.copy(candidate);
            best.setFitness(candidate.getFitness());
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

IS::Solution PopulationBased::findBest(const Population &p) const {
    Population::const_iterator sit = p.begin();
    IS::Solution sol = IS::Solution(*sit);
    double best = sit->getFitness();
    sol.setFitness(best);
    for (; sit != p.end(); sit++) {
        if (sit->getFitness() > best) {
            sol.copy(*sit);
            best = sit->getFitness();
            sol.setFitness(best);
        }
    }
    return sol;
}

// void PopulationBased::AssessFitness(population &p) {
//     Population::iterator sit = p.begin();
//     for(; sit != p.end(); ++sit) {
        
//     }
// }
