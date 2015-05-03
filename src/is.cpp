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
        this->bits[i] = (p > 50) ? 1 : 0;
    }
}

void IS::Solution::tweak() {
    srand(time(NULL));
    int n = rand() % size;
    this->bits[n] = ! this->bits[n];
}

void IS::Metaheuristic::optimize(const IS::Problem &T, IS::Solution &S) {
    S.setSize(T.size());
    S.generateRandom();
    while (1) {
        IS::Solution R(S);
        R.tweak(); 
        //TESTING
        //std::cout << "---printing S---" << std::endl;
        //std::cout << S.to_str() << std::endl;
        //std::cout << "---printing S---" << std::endl;

        //std::cout << "---printing R---" << std::endl;
        //std::cout << R.to_str() << std::endl;
        //std::cout << "---printing R---" << std::endl;
        //TESTING
        double q1 = quality(T, R), q2 = quality(T, S);
        if (q1 > q2)
            S.setBits(R.getBits());
        
        //TESTING
        std::cout << "---> " << max(q1, q2) << std::endl;
        //TESTING
        if (std::max(q1, q2) > 0.95) break;
    }
}

double IS::Metaheuristic::quality(const IS::Problem &T, const IS::Solution &S) {
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

    //assert(result.size() == T.size());
    int count = 0;
    //both sets will be disjoint, so this need to be change
    for (int i = 0; i < result.size(); i++) {
        for (int j = 0; j < T.size(); j++) {
            if (result[i] == T[j]) {
                if (result[i].getCategory() == T[j].getCategory()) 
                    count++;
                break;
            }
        }
    }
    return (1.0 * count) / (1.0 * result.size());
}

