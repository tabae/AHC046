#ifndef __STATE_HPP__
#define __STATE_HPP__

#include "common.hpp"
#include "ryuka.hpp"
#include <numeric>
#include <algorithm>
#include <cassert>
#include <queue>

using namespace std;
using namespace common;

extern RandGenerator ryuka;

struct State {
    static constexpr long long inf = 1LL<<60;
    long long score;
    int block_num;
    bool fix_block;
    vector<pair<char, char>> operations;
    State() : score(-inf) {
        block_num = 0;
        fix_block = false;
    };
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
    int num_blocks = ryuka.rand(30);
    State res = input_state;
    if(res.fix_block) num_blocks =  res.block_num;
    res.block_num = num_blocks;
    vector<pair<int, int>> used_block, unused_block;
    res.operations.clear();
    auto [i, j] = goals[0];
    vector is_block(n, vector<bool>(n, false));
    for(int goal = 1; goal < m; ++goal) {        
        auto [gi, gj] = goals[goal];
        vector<vector<bool>> seen(n, vector<bool>(n, false));
        vector<vector<tuple<int,int,char,char>>> from(n, vector<tuple<int,int,char,char>>(n));
        queue<pair<int,int>> que;
        seen[i][j] = true;
        que.push({i, j});
        string commands = "MS";
        string dirs = "UDLR";
        while(!que.empty()) {
            auto [ci, cj] = que.front();
            que.pop();
            for(char com: commands) {
                for(char dir: dirs) {
                    auto [di, dj] = common::dij(dir);
                    int ni = ci, nj = cj;
                    if(com == 'M') {
                        ni = ci + di;
                        nj = cj + dj;
                    }
                    if(com == 'S') {
                        int nni = ci;
                        int nnj = cj;
                        while(true) {
                            nni = nni + di;
                            nnj = nnj + dj;
                            if(nni < 0 || nni >= n || nnj < 0 || nnj >= n) {
                                break;
                            }
                            if(is_block[nni][nnj]) {
                                break;
                            }
                            ni = nni;
                            nj = nnj;
                        }
                    }
                    if(ni < 0 || ni >= n || nj < 0 || nj >= n) continue;
                    if(seen[ni][nj]) continue;
                    if(is_block[ni][nj]) continue;
                    seen[ni][nj] = true;
                    que.push({ni, nj});
                    from[ni][nj] = {ci, cj, com, dir};
                }
            }
        }
        if (!seen[gi][gj]) {
            res.score = -1;
            return res;
        }

        // restore operations
        vector<pair<char,char>> best_ops;
        int ci = gi, cj = gj;
        while(ci != i || cj != j) {
            auto [ni, nj, com, dir] = from[ci][cj];
            best_ops.push_back({com, dir});
            ci = ni;
            cj = nj;
        }
        reverse(best_ops.begin(), best_ops.end());

        i = gi;
        j = gj;

        if(goal <= num_blocks) {
            int block_count = 0;
            string dirs = "LRUD";
            shuffle(dirs.begin(), dirs.end(), ryuka.engine);
            for(char dir: dirs) {
                int ni = i + common::dij(dir).first;
                int nj = j + common::dij(dir).second;
                if(ni >= 0 && ni < n && nj >= 0 && nj < n) {
                    bool ok = true;
                    for(int k = goal+1; k < m; ++k) {
                        if(ni == goals[k].first && nj == goals[k].second) {
                            ok = false;
                            break;
                        }
                    }
                    if(!ok) continue;
                    best_ops.push_back({'A', dir});
                    block_count++;
                    break;
                }  
            }
        }
        for(auto op: best_ops) {
            res.operations.push_back(op);
        }
        vector<pair<int,int>> _used_block;
        auto [si, sj] = goals[goal-1];
        tie(i, j) = common::exec_operations(si, sj, best_ops, is_block, true, _used_block);
        assert (i == gi && j == gj);
    }   

    res.calc_score(res.operations);
    return res;
}

#endif
