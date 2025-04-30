#include <iostream>

#include "annealer.hpp"
#include "common.hpp"
#include "state.hpp"
using namespace std;
using namespace common;

int main() {
    read();

    IterationControl<State> sera;
    State stat = sera.anneal(1.95, 10, 1e-6, State::initState());
    
    // State ini_stat = sera.climb(0.2, State::initState());
    // State stat = sera.anneal(1.75, 1, 1e-6, ini_stat);
    
    // State stat = sera.climb(1.8, State::initState());
    
    // State stat = State::initState();
    // State stat = State::generateState(State::initState());

    stat.print();
    cerr << "my score = " << stat.score << endl;
    cerr << "bfs_time = " << bfs_time << endl;
}
