#include <cstdio>
#include <string>
#include <cassert>
#include <cmath>
#include <vector>
#include <bitset>
#include <iostream>
#include "is.h"

void IS::Solution::generateRandom() {
    for (int i = 0; i < size; i++) {
        int p = rand() % 100; 
        this->bits[i] = (p > 10) ? 1 : 0;
    }
}

void IS::Solution::copy(const Solution &a) {
    size = a.size;
    bits.reset();
    bits |= a.bits;
}

std::string IS::Instance::toString() const {
    std::string str = "";
    const IS::Instance &a = *this;
    for (int i = 0; i < a.size(); i++)
        str += std::to_string(a[i]) + " ";
    str += std::to_string(category);
    return str;
}

double IS::Instance::calcDistance(const IS::Instance &b) const {
    const IS::Instance &a = *this;
    double dist = 0.0;
    assert(a.size() == b.size());
    for (int i = 0; i < a.size(); ++i)
        dist += (a[i] - b[i]) * (a[i] - b[i]);
    return sqrt(dist);
} 

void IS::Dataset::getDispersions(std::vector<double> &dispersion) const {
    dispersion.assign(size(), 0.0);
    IS::Instance dummy;
    dummy.assign(getAttrs()-1, 0.0);
    const IS::Dataset &a = *this;
    for (int i = 0; i < a.size(); i++)
        dispersion[i] = a[i].calcDistance(dummy);
}

std::string IS::Dataset::toString() const {
    std::string str = std::to_string(size()) + " " + std::to_string(attrs);
    str += "\n";
    // Getting string representation of instances
    const IS::Dataset &a = *this;  
    for (int i = 0; i < a.size(); i++) 
        str += a[i].toString() + "\n";
    return str;
}
