#include <cstdio>
#include <string>
#include <cassert>
#include <cmath>
#include <vector>
#include <bitset>
#include "is.h"
#define MAX 10000000

IS::Instance IS::Metaheuristic::find_nearest(Problem p, Instance a) {
      Problem::iterator it;
      int min = MAX;
      Instance ret;
      for(it = p.begin(); it < p.end(); ++it) {
          int dist = calc_distance(*it, a);
          if (dist < min) {
            min = dist;                  
            ret = *it;
          }
      }
      return ret;
}

void IS::Metaheuristic::oneNN(Problem training, Problem* result) {
    Problem::iterator it;
    for(it = result->begin(); it != result->end(); ++it) {
        Instance nearest = find_nearest(training, *it);
        (*it).setCategory(nearest.getCategory());
    }
}

double IS::Metaheuristic::calc_distance(Instance a, Instance b) {
    double dist = 0;
    assert(a.size() == b.size());
    for(unsigned i = 0; i < a.size(); ++i) {
        dist += pow((a[i] - b[i]), 2);
    }
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
        double q1 = quality(T, R), q2 = quality(T, S);
        if (q1 > q2)
            S.setBits(R.getBits());

        if (std::max(q1, q2) > 0.95) break;
    }
}

double IS::Metaheuristic::quality(const IS::Problem &T, const IS::Solution &S) {
    std::bitset<MAX> bits = S.getBits();
    IS::Problem training, result;
    for (int i = 0; i < T.size(); i++) {
        if (bits.test(i))
            training.push_back(T[i]); 
        result.push_back(T[i]);
        result[i].setCategory(-1);
    }

    oneNN(training, &result);

    assert(result.size() == T.size());
    int count = 0;
    for (int i = 0; i < result.size(); i++) {
        if (result[i].getCategory() != T[i].getCategory()) 
            count++;
    }
    return double(count / result.size());
}

