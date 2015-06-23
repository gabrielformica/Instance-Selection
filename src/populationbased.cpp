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
    
    best.setFitness(quality(ds, best, 0.5));

    int iter = 0, no_change = 0;
    while(1) {  // Main loop
        iter++;
        Population::const_iterator sit;
        Population new_pop;  // New population 
        bool best_changed = false;
        for (sit = population.begin(); sit != population.end(); sit++) {
            local_search->set_max_quality(std::min(best.getFitness() + 0.1, 0.95));
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
        generatePopulation(population, sol.getSize(), ds);
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
        //double q_children = quality(ds, c1, 0.5) + quality(ds, c2, 0.5);
        //double q_parents = quality(ds, p1, 0.5) + quality(ds, p2, 0.5);
        //if (q_children > q_parents) {  // Elitist
        breeded.insert(c1);
        breeded.insert(c2);
        //} else {
        //    breeded.insert(p1);
        //    breeded.insert(p2);
        //}
    }
    return breeded;
}

IS::Solution PopulationBased::tourneySelection(const Population &p, int s) const {
    Population::iterator sit = p.begin();
    advance(sit, (rand() % popsize));
    IS::Solution best = IS::Solution(*sit);
    best.setFitness(sit->getFitness());
    for(int i = 0; i < s-1; ++i) {
        sit = p.begin();
        advance(sit, (rand() % popsize));
        if (best.getFitness() < (*sit).getFitness()) {
            best.copy(*sit);
        }
    }
    cout << BOLDRED << "Best fitness inside tourney" << best.getFitness() << RESET << endl;
    cout << BOLDRED << "Done tourney" << RESET << endl;
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
    best.setFitness(best_fitness);
    int iter = 0, no_change = 0;
    double old_q = -1.0;
    local_search->set_max_quality(std::min(best.getFitness() + 0.1, 0.95));
    while(1) {
        iter++;
        IS::Solution p1 = tourneySelection(population, 3);
        IS::Solution p2 = tourneySelection(population, 3);
        IS::Solution p1_cand = IS::Solution(p1);
        IS::Solution p2_cand = IS::Solution(p2);
        Children recombination(p1_cand, p2_cand);
        tweaker->tweak(p1_cand);
        tweaker->tweak(p2_cand);
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
            old_q = best.getFitness();
            best.copy(candidate);
            best.setFitness(candidate.getFitness());
        }
        if (best.getFitness() - old_q < EPSILON) no_change++;
        else {
            cout << YELLOW << "CHANGE" << RESET << endl;
            old_q = best.getFitness();
            no_change = 0;
        }
        bool stop = best.getFitness() >= max_quality or iter == ite_limit;
        stop = stop or no_change == no_change_best;
        if (stop) break;
        // Population breeded = breed(ds, population);
        // population = breeded;
    }
    cout << GREEN << "iter " << iter << RESET <<endl;
    cout << GREEN << "no_change " << no_change << RESET <<endl;
    cout << GREEN << "no_change_best " << no_change_best << RESET <<endl;
    cout << GREEN << "best.getFitness() " << best.getFitness() << RESET <<endl;
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
