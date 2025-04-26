#include "common.hpp"
#include "state.hpp"
#include "annealer.hpp"
#include <iostream>
using namespace std;
using namespace common;

int main() {

    read();

    IterationControl<State> sera;
    // State stat = sera.anneal(1.8, 100, 1, State::initState());
    State stat1 = sera.climb(1.0, State::initState());
    stat1.fix_block =true;
    State stat = sera.climb(0.8, stat1);
    // State stat = State::initState();
    // State stat = State::generateState(State::initState());

    stat.print();
    cerr << "my score = " << stat.score << endl;

}
