#ifndef IS_H
#define IS_H

#include <cstdio>
#include <string>
#include <vector>
#include <bitset>
#define MAX 10000000

namespace IS {

  class Atribute {

  };
  
  class Instance : public std::vector<int> {
    
  }; 

  class Problem : public std::vector<Instance> {

  };

  typedef std::bitset<MAX> Solution;

  class Metaheuristic {
    public:
      virtual void optimize(const Problem &T, Solution &R) = 0;
  };
}

#endif
