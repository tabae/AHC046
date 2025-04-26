#ifndef __COMMON_HPP__
#define __COMMON_HPP__

#include <iostream>
#include <vector>
#include <cmath>
#include <map>
#include <algorithm>
using namespace std;
using ll = long long;

namespace common {
    int n, m;
    vector<pair<int,int>> goals;
    void read();
    pair<int,int> dij(char dir);
    pair<int,int> exec_operations(int, int, const vector<pair<char, char>>&, vector<vector<bool>>&, bool, vector<pair<int,int>>&);
    inline ll calc_score(const vector<pair<char, char>>& operations);
};

void common::read() {
    cin >> n >> m;
    for(int i = 0; i < m; ++i) {
        int x, y;
        cin >> x >> y;
        goals.push_back({x, y});
    }
}

pair<int,int> common::dij(char dir) {
    if(dir == 'L') {
        return {0, -1};
    } else if(dir == 'R') {
        return {0, 1};
    } else if(dir == 'U') {
        return {-1, 0};
    } else if(dir == 'D') {
        return {1, 0};
    } else {
        cerr << "Invalid direction: " << dir << endl;
        exit(1);
    }
}

pair<int,int> common::exec_operations(int i, int j, const vector<pair<char, char>>& operations, 
                                      vector<vector<bool>>& is_block, bool change_is_block,
                                      vector<pair<int,int>>& used_block) {
    map<pair<int, int>, int> is_block_org;
    for(auto [act, dir]: operations) {
        //cerr << "Executing operation: " << act << " " << dir << " (" << i << ", " << j << ")" << endl;
        if(act == 'M') {
            auto [di, dj] = common::dij(dir);
            int ni = i + di;
            int nj = j + dj;
            if(ni < 0 || ni >= n || nj < 0 || nj >= n) {
                cerr << "Out of bounds: (" << ni << ", " << nj << ")" << endl;
                cerr << "(i, j) = " << i << ", " << j << endl;
                exit(1);
            }
            if(is_block[ni][nj]) {
                cerr << "Blocked cell: (" << ni << ", " << nj << ")" << endl;
                exit(1);
            }
            i = ni;
            j = nj;
        } else if(act == 'S') {
            auto [di, dj] = common::dij(dir);
            while(true) {
                int ni = i + di;
                int nj = j + dj;
                if(ni < 0 || ni >= n || nj < 0 || nj >= n) {
                    break;
                }
                if(is_block[ni][nj]) {
                    used_block.push_back({ni, nj});
                    break;
                }
                i = ni;
                j = nj;
            }
        } else if(act == 'A') {
            auto [di, dj] = common::dij(dir);
            int ni = i + di;
            int nj = j + dj;
            if(ni < 0 || ni >= n || nj < 0 || nj >= n) {
                cerr << "Out of bounds: (" << ni << ", " << nj << ")" << endl;
                exit(1);
            }
            if(!change_is_block && !is_block_org.count({ni, nj})) {
                is_block_org[{ni, nj}] = is_block[ni][nj];
            }
            is_block[ni][nj] = !is_block[ni][nj];
        }
    }
    if(!change_is_block) {
        for(auto [pos, val]: is_block_org) {
            is_block[pos.first][pos.second] = val;
        }
    }
    sort(used_block.begin(), used_block.end());
    used_block.erase(unique(used_block.begin(), used_block.end()), used_block.end());
    return {i, j};
}


inline ll common::calc_score(const vector<pair<char, char>>& operations) {
    //cerr << "Operations size = " << operations.size() << endl;
    ll score = 0;
    vector<vector<bool>> is_block(n, vector<bool>(n, false));
    auto [i, j] = goals[0];
    int goal_index = 1;
    int visited = 1;
    for(auto [act, dir]: operations) {
        vector<pair<int,int>> _used_block;
        tie(i, j) = exec_operations(i, j, {{act, dir}}, is_block, true, _used_block);
        if(goals[goal_index].first == i && goals[goal_index].second == j) {
            visited++;
            goal_index++;
        }
    }
    if(visited == m) {
        score = m + 2*n*m - operations.size();
    } else {
        score = visited;
    }
    cerr << "visited = " << visited << endl;
    return score;
}

#endif