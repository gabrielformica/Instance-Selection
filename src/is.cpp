#include <cstdio>
#include <string>
#include <cassert>
#include <cmath>
#include <vector>
#include <bitset>
#include <iostream>
#include "is.h"

double IS::Metaheuristic::find_nearest(const Problem &p, const Instance &a) {
    int imin = MAX;
    double category;
    for (Problem::const_iterator it = p.begin(); it < p.end(); ++it) {
        int dist = calc_distance(*it, a);
        if (dist == 0) {
            category = (*it).getCategory();
            break;
        }
        if (dist < imin) {
            imin = dist;                  
            category = (*it).getCategory();
        }
    }
    return category;
}

void IS::Metaheuristic::oneNN(const Problem &training, Problem *result) {
    Problem::iterator it;
    for(it = result->begin(); it != result->end(); ++it)
        (*it).setCategory(find_nearest(training, *it));
}

double IS::Metaheuristic::calc_distance(const Instance &a, const Instance &b) {
    double dist = 0.0;
    assert(a.size() == b.size());
    for (int i = 0; i < a.size(); ++i)
        dist += (a[i] - b[i]) * (a[i] - b[i]);
    return sqrt(dist);
}

void IS::Solution::generateRandom() {
    srand(time(NULL));
    for (int i = 0; i < size; i++) {
        int p = rand() % 100; 
        this->bits[i] = (p > 10) ? 1 : 0;
    }
}

void IS::Solution::tweak() {
    srand(time(NULL));
    int n = (rand() % bits.count()) + 1;
    int i, j;
    for (j = 0, i = 0; j < size && i < n; j++) if (bits.test(j)) i++;
    assert(bits.test(j-1));
    this->bits[j-1] = 0;
}

void IS::Metaheuristic::optimize(const IS::Problem &T, IS::Solution &S) {
    S.setSize(T.size());
    S.generateRandom();
    double q_max;
    while (1) {
        IS::Solution R(S);
        R.tweak(); 
        assert((S.getBits() ^ R.getBits()).count() == 1);
        //TESTING
        //std::cout << "---printing S---" << std::endl;
        //std::cout << S.to_str() << std::endl;
        //std::cout << "---printing S---" << std::endl;

        //std::cout << "---printing R---" << std::endl;
        //std::cout << R.to_str() << std::endl;
        //std::cout << "---printing R---" << std::endl;
        //TESTING

        // Using 0.5 beacuse paper
        double q1 = quality(T, R, 0.5), q2 = quality(T, S, 0.5);
        if (q1 > q2)
            S.setBits(R.getBits());
        
        q_max = std::max(q1, q2);
        // std::cout << "---> " << q_max << std::endl;
        if (q_max > 0.97) break;
    }
    cout << "---> " << q_max << endl;
}

double IS::Metaheuristic::quality(const IS::Problem &T, 
                                  const IS::Solution &S,
                                  double alpha) {
    std::bitset<MAX> bits = S.getBits();
    IS::Problem training, result;
    int j = 0;
    for (int i = 0; i < T.size(); i++) {
        if (bits.test(i))
            training.push_back(T[i]); 
        else {
            result.push_back(T[i]);
            result[j++].setCategory(-1);
        }
    }

    oneNN(training, &result);

    // assert(result.size() == T.size());
    int count = 0;
    //TODO : this is N * result.size()
    for (int i = 0; i < result.size(); i++) {
        for (int j = 0; j < T.size(); j++) {
            if (result[i] == T[j]) {
                if (result[i].getCategory() == T[j].getCategory()) 
                    count++;
                break;
            }
        }
    }

    double clas_rate = (double)count/(double)result.size();
    double perc_redc = ((double)(result.size()))/((double)T.size());

    // cout << "Hubo un total de " << count << " aciertos" << endl;
    // cout << "Result es de tamanio: " << result.size() << endl;
    // cout << "La relación es: " << (double)count/(double)result.size() << endl;

    // cout << "El tamaño de T es: " << T.size() << endl;
    // cout << "El porcentaje de reducción es: " << perc_redc << endl;

    double fitness = alpha*clas_rate + (1-alpha)*perc_redc;

    // cout << "Fitness: " << fitness << endl;
    return fitness;
}
