#ifndef __STATE_HPP__
#define __STATE_HPP__

#include <algorithm>
#include <cassert>
#include <numeric>
#include <queue>

#include "common.hpp"
#include "ryuka.hpp"

using namespace std;
using namespace common;

extern RandGenerator ryuka;

struct State {
    static constexpr long long inf = 1LL << 60;
    long long score;
    vector<pair<char, char>> operations;
    vector<vector<bool>> block_candidates;
    State() : score(-inf) {
        block_candidates.resize(n, vector<bool>(n, false));
    };
    long long calc_score(const vector<pair<char, char>> &operations);
    void print();
    static State initState();
    static State generateState(const State &input_state);
};

long long State::calc_score(const vector<pair<char, char>> &operations) {
    score = common::calc_score(operations);
    return score;
}

void State::print() {
    // do something
    for (auto [act, dir] : operations) {
        cout << act << " " << dir << endl;
    }
}

State State::initState() {
    State res;
    for(int x = 0; x < 20; x++) {
        int i = ryuka.rand(n);
        int j = ryuka.rand(n);
        if(!goal_map[i][j]) res.block_candidates[i][j] = true;
    }
    res.operations = common::solve(res.block_candidates);
    res.calc_score(res.operations);
    return res;
}

State State::generateState(const State &input_state) {
    State res;
    res.block_candidates = input_state.block_candidates;
    vector<pair<int, int>> blocks;
    for(int i = 0; i < n; i++) {
        for(int j = 0; j < n; j++) {
            if(res.block_candidates[i][j]) blocks.push_back({i, j});
        }
    }
    if(!blocks.empty()) {
        int cmd = ryuka.rand(4);
        if(cmd == 0) {
            int i = ryuka.rand(n);
            int j = ryuka.rand(n);
            res.block_candidates[i][j] = !res.block_candidates[i][j];
            res.operations = common::solve(res.block_candidates);
        } else if(cmd == 1) {
            int idx = ryuka.rand(blocks.size());
            auto [i, j] = blocks[idx];
            res.block_candidates[i][j] = false;
            int ni = ryuka.rand(n);
            int nj = ryuka.rand(n);
            res.block_candidates[ni][nj] = true;
            res.operations = common::solve(res.block_candidates);
        } else {
            int idx = ryuka.rand(blocks.size());
            auto [i, j] = blocks[idx];
            auto [di, dj] = common::dij(dirs[ryuka.rand(4)]);
            int ni = clamp(i + di, 0, n - 1);
            int nj = clamp(j + dj, 0, n - 1);
            swap(res.block_candidates[i][j], res.block_candidates[ni][nj]);
            res.operations = common::solve(res.block_candidates);
        }
    } else {
        int i = ryuka.rand(n);
        int j = ryuka.rand(n);
        res.block_candidates[i][j] = true;
        res.operations = common::solve(res.block_candidates);
    }
    res.calc_score(res.operations);
    return res;
}

#endif
