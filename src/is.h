#ifndef IS_H
#define IS_H

#include <cstdio>
#include <string>
#include <cassert>
#include <cmath>
#include <vector>
#include <bitset>
#define MAX 10000000

using namespace std;

namespace IS {

  class Atribute {

  };
  
  class Instance : public std::vector<double> {
    private:
      double category;
    public:
      double getCategory() { return category; }
      void setCategory(double c) { category = c; }
  }; 

  class Problem : public std::vector<Instance> {
    public:
        Problem();
        Problem(const Problem& a) : vector<Instance>(a.begin(), a.end()) { };
        Problem(int N) : std::vector<Instance>(N) {};
  };

  typedef std::bitset<MAX> Solution;

  class Metaheuristic {
    public:
      virtual void optimize(const Problem &T, Solution &R) = 0;
  };

  double calc_distance(Instance a, Instance b) {
      int i;
      double dist = 0;
      assert(a.size() == b.size());
      for(unsigned i = 0; i < a.size(); ++i) {
          dist += pow((a[i] - b[i]), 2);
      }
      return sqrt(dist);
  }

  Instance find_nearest(Instance a, Problem p) {
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

  void oneNN(Problem training, Problem* result) {
      Problem::iterator it;
      for(it = result->begin(); it != result->end(); ++it) {
          Instance nearest = find_nearest(*it, training);
          (*it).setCategory(nearest.getCategory());
      }
  }

}

#endif
