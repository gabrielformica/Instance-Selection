#include <cstdlib>
#include <iostream>
#include <cstdio>
#include "is.h"

int main() {
    int insts, attrs, training_size, testing_size;
    cin >> insts >> attrs;
    training_size = (int)floor(insts * 0.80);
    testing_size     = (int)ceil(insts * 0.20);

    IS::Problem training = IS::Problem(training_size);
    IS::Problem testing = IS::Problem(testing_size);

    for(unsigned i = 0; i < training_size; ++i) {
        double attr;
        for(unsigned j = 0; j < attrs-1; ++j) {
            cin >> attr;
            training[i].push_back(attr);
        }
        cin >> attr;
        training[i].setCategory(attr);
    }

    for(unsigned i = 0; i < testing_size; ++i) {
        double attr;
        for(unsigned j = 0; j < attrs-1; ++j) {
            cin >> attr;
            testing[i].push_back(attr);
        }
        cin >> attr;
        testing[i].setCategory(attr);
    }

    // cout << "TRAINING SET" << endl;
    // for(unsigned i = 0; i < training_size; ++i) {
    //     double attr;
    //     for(unsigned j = 0; j < attrs-1; ++j) {
    //         cout << training[i][j] << " ";
    //     }
    //     cout << training[i].getCategory() << endl;
    // }

    // cout << "TESTING SET" << endl;
    // for(unsigned i = 0; i < testing_size; ++i) {
    //     double attr;
    //     for(unsigned j = 0; j < attrs-1; ++j) {
    //         cout << testing[i][j] << " ";
    //     }
    //     cout << testing[i].getCategory() << endl;
    // }

    IS::Problem result(training_size);

    for(unsigned i = 0; i < training_size; ++i) {
        for(unsigned j = 0; j < attrs-1; ++j) {
            result[i].push_back(training[i][j]);
            result[i].setCategory(-1);
        }
    }

    oneNN(training, &result);

    int count_error = 0;
    for(unsigned i = 0; i < training_size; ++i) {

        if(training[i].getCategory() != result[i].getCategory()) {
            cout << "test: " << testing[i].getCategory() << endl;
            cout << "result: " << result[i].getCategory() << endl;
            count_error++;
        }
    }
    cout << "Error in " << count_error << " instances out of ";
    cout << testing_size << ": " << (count_error*100)/testing_size << endl;

}
