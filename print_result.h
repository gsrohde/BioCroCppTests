#include <iostream>
#include "BioCro.h"

using std::cout;
using std::endl;
using std::setw;

inline void print_result(const BioCro::Simulation_result &result) {

    cout.precision(4);

    for (auto item : result) {
        cout << setw(11) << item.first << "   ";
    }
    cout << endl;

    // Get an arbitrary column from the result and find its length.
    auto an_item = result.begin();
    vector<double> a_column {an_item->second};
    size_t length {a_column.size()};

    for (size_t i = 0; i < length; ++i) {
        for (auto item : result) {
            cout << setw(11) << item.second[i] << "   ";
        }
        cout << endl;
    }
}
