#ifndef __STATE_HPP__
#define __STATE_HPP__

#include "common.hpp"
#include "ryuka.hpp"
#include <numeric>
#include <algorithm>
#include <cassert>

using namespace std;
using namespace common;

extern RandGenerator ryuka;

struct State {
    static constexpr long long inf = 1LL<<60;
    long long score;
    vector<pair<char, char>> operations;
    State() : score(-inf) {};
    long long calc_score(const vector<pair<char, char>>& operations);
    void print();
    static State initState();
    static State generateState(const State& input_state);
};

long long State::calc_score(const vector<pair<char, char>>& operations) {
    score = common::calc_score(operations);
    return score;
}

void State::print() {
    // do something
    for(auto [act, dir]: operations) {
        cout << act << " " << dir << endl;
    }
}

State State::initState() {
    State res;
    auto [i, j] = goals[0];
    for(int goal = 1; goal < m; ++goal) {
        auto [gi, gj] = goals[goal];
        vector<pair<int,int>> corners = {
            {0, 0},
            {0, n-1},
            {n-1, 0},
            {n-1, n-1}
        };
        int min_dist = 1<<30;
        pair<int, int> closest_corner;
        for(auto [ci, cj]: corners) {
            int dist = abs(gi - ci) + abs(gj - cj);
            if(dist < min_dist) {
                min_dist = dist;
                closest_corner = {ci, cj};
            }
        }
        auto [ci, cj] = closest_corner;
        if(ci - i > 0) {
            res.operations.push_back({'S', 'D'});
        } else if(ci - i < 0) {
            res.operations.push_back({'S', 'U'});
        }
        i = ci;
        if(cj - j > 0) {
            res.operations.push_back({'S', 'R'});
        } else if(cj - j < 0) {
            res.operations.push_back({'S', 'L'});
        }
        j = cj;
        while(i != gi) {
            if(i < gi) {
                res.operations.push_back({'M', 'D'});
                ++i;
            } else if(i > gi) {
                res.operations.push_back({'M', 'U'});
                --i;
            }
        }
        while(j != gj) {
            if(j < gj) {
                res.operations.push_back({'M', 'R'});
                ++j;
            } else if(j > gj) {
                res.operations.push_back({'M', 'L'});
                --j;
            }
        }
        assert(i == gi && j == gj);
    }
    res.calc_score(res.operations);
    return res;
}

State State::generateState(const State& input_state) {
    State res = input_state;
    // do something
    res.calc_score(res.operations);
    return res;
}

#endif
