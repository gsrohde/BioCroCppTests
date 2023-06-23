#include <iostream>
#include <framework/state_map.h>

using std::cout;
using std::endl;
using std::setw;

void print_result(const state_vector_map &result) {

    cout.precision(4);
    for (auto item : result) {
        cout << setw(11) << item.first << "   ";
    }
    cout << endl;

    for (int i = 0; i < result.at("time").size(); ++i) {
        for (auto item : result) {
            cout << setw(11) << item.second[i] << "   ";
        }
        cout << endl;
    }
}
