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
    long long rollback_score;
    vector<pair<char, char>> rollback_operations;
    vector<tuple<int,int,bool>> rollback_cache;
    State() : score(-inf) {
        block_candidates.resize(n, vector<bool>(n, false));
    };
    long long calc_score(const vector<pair<char, char>> &operations);
    void print();
    void rollback();
    void nextState();
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
    for(int goal = 0; goal < 20; goal++) {
        auto [di, dj] = common::dij(dirs[ryuka.rand(4)]);
        int i = goals[goal].first + di;
        int j = goals[goal].second + dj;
        if(i < 0 || i >= n || j < 0 || j >= n) continue;
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


void State::nextState() {
    rollback_cache.clear();
    rollback_score = score;
    rollback_operations = operations;
    vector<pair<int, int>> blocks;
    for(int i = 0; i < n; i++) {
        for(int j = 0; j < n; j++) {
            if(block_candidates[i][j]) blocks.push_back({i, j});
        }
    }
    if(!blocks.empty()) {
        int cmd = ryuka.rand(4);
        if(cmd == 0) {
            int i = ryuka.rand(n);
            int j = ryuka.rand(n);
            rollback_cache.push_back({i, j, block_candidates[i][j]});
            block_candidates[i][j] = !block_candidates[i][j];
            operations = common::solve(block_candidates);
        } else if(cmd == 1) {
            int idx = ryuka.rand(blocks.size());
            auto [i, j] = blocks[idx];
            rollback_cache.push_back({i, j, block_candidates[i][j]});
            block_candidates[i][j] = false;
            int ni = ryuka.rand(n);
            int nj = ryuka.rand(n);
            rollback_cache.push_back({ni, nj, block_candidates[ni][nj]});
            block_candidates[ni][nj] = true;
            operations = common::solve(block_candidates);
        } else {
            int idx = ryuka.rand(blocks.size());
            auto [i, j] = blocks[idx];
            auto [di, dj] = common::dij(dirs[ryuka.rand(4)]);
            int ni = clamp(i + di, 0, n - 1);
            int nj = clamp(j + dj, 0, n - 1);
            rollback_cache.push_back({i, j, block_candidates[i][j]});
            rollback_cache.push_back({ni, nj, block_candidates[ni][nj]});
            swap(block_candidates[i][j], block_candidates[ni][nj]);
            operations = common::solve(block_candidates);
        }
    } else {
        int i = ryuka.rand(n);
        int j = ryuka.rand(n);
        block_candidates[i][j] = true;
        operations = common::solve(block_candidates);
    }
    calc_score(operations);
}

void State::rollback() {
    for(auto [i, j, b] : rollback_cache) {
        block_candidates[i][j] = b;
    }
    operations = rollback_operations;
    score = rollback_score;
}

#endif
