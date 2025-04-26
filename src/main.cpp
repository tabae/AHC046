#include "common.hpp"
#include "state.hpp"
#include "annealer.hpp"
#include <iostream>
using namespace std;
using namespace common;

int main() {

    read();

    IterationControl<State> sera;
    State stat = sera.anneal(1.8, 100, 1, State::initState());
    // State stat = sera.climb(0.5, State::initState());
    // State stat = State::initState();
    // State stat = State::generateState(State::initState());

    stat.print();
    cerr << "my score = " << stat.score << endl;

}
