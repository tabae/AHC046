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
    for (int goal = 0; goal < 20; goal++) {
        auto [di, dj] = common::dij(dirs[ryuka.rand(4)]);
        int i = goals[goal].first + di;
        int j = goals[goal].second + dj;
        if (i < 0 || i >= n || j < 0 || j >= n) continue;
        if (!goal_map[i][j]) res.block_candidates[i][j] = true;
    }
    res.operations = common::solve(res.block_candidates);
    res.calc_score(res.operations);
    return res;
}

State State::generateState(const State &input_state) {

    auto get_nearside = [&]() -> vector<pair<int, int>> {
        vector<vector<int>> visit_counts(n, vector<int>(n, 0));
        vector<vector<bool>> block_map(n, vector<bool>(n, false));
        int i = goals[0].first;
        int j = goals[0].second;
        for(auto [act, dir]: input_state.operations) {
            const auto [di, dj] = dij(dir);
            if (act == 'M') {
                i += di;
                j += dj;
                visit_counts[i][j]++;
            } else if (act == 'S') {
                while(true) {
                    int ni = i + di;
                    int nj = j + dj;
                    if (ni < 0 || ni >= n || nj < 0 || nj >= n) break;
                    if (block_map[ni][nj]) break;
                    i = ni;
                    j = nj;
                    visit_counts[i][j]++;
                }
            } else if(act == 'A') {
                int ni = i + di;
                int nj = j + dj;
                block_map[ni][nj] = true;
            }
        }
        i = goals[0].first;
        j = goals[0].second;
        for(int ii = 0; ii < n; ii++) {
            for(int jj = 0; jj < n; jj++) {
                block_map[ii][jj] = false;
            }
        }
        vector<pair<int,int>> block_positions;
        for(auto [act, dir]: input_state.operations) {
            const auto [di, dj] = dij(dir);
            if (act == 'M') {
                i += di;
                j += dj;
                if(i < 0 || i >= n || j < 0 || j >= n) return {};
                visit_counts[i][j]--;
            } else if (act == 'S') {
                while(true) {
                    int ni = i + di;
                    int nj = j + dj;
                    if (ni < 0 || ni >= n || nj < 0 || nj >= n) break;
                    if (block_map[ni][nj]) break;
                    i = ni;
                    j = nj;
                    visit_counts[i][j]--;
                }
            } else if (act == 'A') {
                int ni = i + di;
                int nj = j + dj;
                if(ni < 0 || ni >= n || nj < 0 || nj >= n) return {};
                block_map[ni][nj] = true;
            }
            
            if (visit_counts[i][j] == 0 && !input_state.block_candidates[i][j] && !goal_map[i][j]) {
                block_positions.push_back({i, j}); 
            }
        }
        return block_positions;
    };

    State res;
    res.block_candidates = input_state.block_candidates;
    vector<pair<int, int>> blocks;
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            if (res.block_candidates[i][j]) blocks.push_back({i, j});
        }
    }
    if (!blocks.empty()) {
        int cmd = ryuka.rand(4);
        if (cmd == 0) {
            int i = ryuka.rand(n);
            int j = ryuka.rand(n);
            res.block_candidates[i][j] = !res.block_candidates[i][j];
            res.operations = common::solve(res.block_candidates);
        } else if (cmd == 1) {
            int idx = ryuka.rand(blocks.size());
            auto [i, j] = blocks[idx];
            res.block_candidates[i][j] = false;
            auto block_positions = get_nearside();
            if( block_positions.empty()) {
                int ni = ryuka.rand(n);
                int nj = ryuka.rand(n);
                res.block_candidates[ni][nj] = true;
            } else {
                auto [ni, nj] = block_positions[ryuka.rand(block_positions.size())];
                res.block_candidates[ni][nj] = true;
            }
            int ni = ryuka.rand(n);
            int nj = ryuka.rand(n);
            res.block_candidates[ni][nj] = true;
            res.operations = common::solve(res.block_candidates);
        } else if (cmd == 2) {
            auto block_positions = get_nearside();
            if (block_positions.empty()) {
                auto [i, j] = block_positions[ryuka.rand(block_positions.size())];
                res.block_candidates[i][j] = true;
                res.operations = common::solve(res.block_candidates);
            }
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
