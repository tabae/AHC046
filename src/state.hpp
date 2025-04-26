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
                    if(ni >= 0 && ni < n) {
                        ops.push_back({'S', dir_i});
                    }
                }
                if(dir_j != 'X') {
                    int nj = j + common::dij(dir_j).second;
                    if(nj >= 0 && nj < n) {
                        ops.push_back({'S', dir_j});
                    }
                }
                vector<pair<int,int>> _used_block;
                auto [ti, tj] = common::exec_operations(i, j, ops, is_block, false, _used_block);
                while(ti != gi) {
                    if(ti < gi) {
                        if(is_block[ti][tj]) {
                            ops.push_back({'A', 'D'});
                        }
                        ops.push_back({'M', 'D'});
                        ++ti;
                    } else if(ti > gi) {
                        if(is_block[ti][tj]) {
                            ops.push_back({'A', 'U'});
                        }
                        ops.push_back({'M', 'U'});
                        --ti;
                    }
                }
                while(tj != gj) {
                    if(tj < gj) {
                        if(is_block[ti][tj]) {
                            ops.push_back({'A', 'R'});
                        }
                        ops.push_back({'M', 'R'});
                        ++tj;
                    } else if(tj > gj) {
                        if(is_block[ti][tj]) {
                            ops.push_back({'A', 'L'});
                        }
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
    vector<pair<int, int>> used_block, unused_block;
    {
        vector<vector<bool>> is_block(n, vector<bool>(n, false));
        auto [gi, gj] = goals[0];
        auto _ = common::exec_operations(gi, gj, res.operations, is_block, true, used_block);
        for(int i = 0; i < n; ++i) {
            for(int j = 0; j < n; ++j) {
                if(!is_block[i][j]) continue;
                if(lower_bound(used_block.begin(), used_block.end(), make_pair(i, j)) != used_block.end()) {
                    ;
                } else {
                    unused_block.push_back({i, j});
                }
            }
        }
    } // get used_block

    res.operations.clear();
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
                    if(ni >= 0 && ni < n) {
                        ops.push_back({'S', dir_i});
                    }
                }
                if(dir_j != 'X') {
                    int nj = j + common::dij(dir_j).second;
                    if(nj >= 0 && nj < n) {
                        ops.push_back({'S', dir_j});
                    }
                }
                vector<pair<int,int>> _used_block;
                auto [ti, tj] = common::exec_operations(i, j, ops, is_block, false, _used_block);

                while(ti != gi) {
                    if(ti < gi) {
                        if(is_block[ti+1][tj]) {
                            ops.push_back({'A', 'D'});
                        }
                        ops.push_back({'M', 'D'});
                        ++ti;
                    } else if(ti > gi) {
                        if(is_block[ti-1][tj]) {
                            ops.push_back({'A', 'U'});
                        }
                        ops.push_back({'M', 'U'});
                        --ti;
                    }
                }
                while(tj != gj) {
                    if(tj < gj) {
                        if(is_block[ti][tj+1]) {
                            ops.push_back({'A', 'R'});
                        }
                        ops.push_back({'M', 'R'});
                        ++tj;
                    } else if(tj > gj) {
                        if(is_block[ti][tj-1]) {
                            ops.push_back({'A', 'L'});
                        }
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

        for(char dir_i: dirs_i) {
            if(dir_i == 'X') continue;
            int ni = i + common::dij(dir_i).first;
            int nj = j;
            if(ni < 0 || ni >= n || is_block[ni][nj] ||
            lower_bound(unused_block.begin(), unused_block.end(), make_pair(ni, nj)) != unused_block.end()) {
                continue;
            }
            bool ok = true;
            for(int jj = 0; jj < n; jj++) {
                for(int k = goal+1; k < m; k++) {
                    if(ni == goals[k].first && jj == goals[k].second) {
                        ok = false;
                        break;
                    }
                }
                if(!ok) break;
            }
            if(!ok) continue;
            int i_start = dir_i == 'U' ? i+1 : i-1;
            int i_delta = dir_i == 'U' ? 1 : -1;
            for(int ii = i_start; 0 <= ii && ii < n; ii += i_delta) {
                for(int k = goal+1; k < m; k++) {
                    if(ii == goals[k].first && nj == goals[k].second) {
                        ok = false;
                        break;
                    }
                }
                if(!ok) break;
            }
            if(!ok) continue;
            best_ops.push_back({'A', dir_i});
            break;
        }

        for(auto op: best_ops) {
            res.operations.push_back(op);
        }

        // print block
        /*
        for(int i = 0; i < n; ++i) {
            for(int j = 0; j < n; ++j) {
                cerr << is_block[i][j] << " ";
            }
            cerr << endl;
        }
        cerr << "aaa" << endl;
        */
        vector<pair<int,int>> _used_block;
        auto [si, sj] = goals[goal-1];
        tie(i, j) = common::exec_operations(si, sj, best_ops, is_block, true, _used_block);
        //cerr << "bbb" << endl;
        assert (i == gi && j == gj);
    }   

    res.calc_score(res.operations);
    return res;
}

#endif
