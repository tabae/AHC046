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
    vector is_block(n, vector<bool>(n, false));
    for(int goal = 1; goal < m; ++goal) {
        auto [gi, gj] = goals[goal];
        int min_ops = 1<<30;
        vector<pair<char,char>> best_ops;
        string dirs_i = "UDX";
        string dirs_j = "LRX";
        for(char dir_i: dirs_i) {
            for(char dir_j: dirs_j) {
                vector<pair<char, char>> ops;
                if(dir_i != 'X') {
                    int ni = i + common::dij(dir_i).first;
                    if(ni >= 0 || ni < n) {
                        ops.push_back({'S', dir_i});
                    }
                }
                if(dir_j != 'X') {
                    int nj = j + common::dij(dir_j).second;
                    if(nj >= 0 && nj < n) {
                        ops.push_back({'S', dir_j});
                    }
                }
                auto [ti, tj] = common::exec_operations(i, j, ops, is_block, false);
                while(ti != gi) {
                    if(ti < gi) {
                        ops.push_back({'M', 'D'});
                        ++ti;
                    } else if(ti > gi) {
                        ops.push_back({'M', 'U'});
                        --ti;
                    }
                }
                while(tj != gj) {
                    if(tj < gj) {
                        ops.push_back({'M', 'R'});
                        ++tj;
                    } else if(tj > gj) {
                        ops.push_back({'M', 'L'});
                        --tj;
                    }
                }     
                if(ops.size() < min_ops) {
                    min_ops = ops.size();
                    best_ops = ops;
                }
            }
        }
        i = gi;
        j = gj;
        for(auto op: best_ops) {
            res.operations.push_back(op);
        }
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
