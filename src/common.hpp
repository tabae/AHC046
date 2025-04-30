#ifndef __COMMON_HPP__
#define __COMMON_HPP__

#include <algorithm>
#include <cassert>
#include <cmath>
#include <iostream>
#include <map>
#include <queue>
#include <vector>
using namespace std;
using ll = long long;

namespace common {
const string dirs = "LRUD";

int n, m;
vector<pair<int, int>> goals;
vector<vector<bool>> goal_map;
double bfs_time = 0;

void read();
pair<int, int> dij(char dir);
int dir2idx(char dir);
vector<pair<char, char>> solve(vector<vector<bool>> &block_candidates);
inline ll calc_score(const vector<pair<char, char>> &operations,
                     bool check = false);
};  // namespace common

void common::read() {
    cin >> n >> m;
    goal_map.resize(n, vector<bool>(n, false));
    for (int i = 0; i < m; ++i) {
        int x, y;
        cin >> x >> y;
        goals.push_back({x, y});
        goal_map[x][y] = true;
    }
}

pair<int, int> common::dij(char dir) {
    if (dir == 'L') {
        return {0, -1};
    } else if (dir == 'R') {
        return {0, 1};
    } else if (dir == 'U') {
        return {-1, 0};
    } else if (dir == 'D') {
        return {1, 0};
    } else {
        assert(false);
    }
}

int common::dir2idx(char dir) {
    if (dir == 'L') return 0;
    if (dir == 'R') return 1;
    if (dir == 'U') return 2;
    if (dir == 'D') return 3;
    assert(false);
}

vector<pair<char, char>> common::solve(
    vector<vector<bool>> &block_candidates) {
    vector<vector<bool>> block_map(n, vector<bool>(n, false));

    vector skate_stop(4, vector(n, vector<pair<int, int>>(n)));
    // initialize skate_stop
    for (int i = 0; i < n; i++)
        for (int j = 0; j < n; j++) {
            skate_stop[0][i][j] = {i, 0};
            skate_stop[1][i][j] = {i, n - 1};
            skate_stop[2][i][j] = {0, j};
            skate_stop[3][i][j] = {n - 1, j};
        }
    auto update_skate_stop = [&](int block_i, int block_j) -> void {
        for (int j = block_j + 1; j < n; j++) {
            if (block_map[block_i][j]) break;
            skate_stop[0][block_i][j] = {block_i, block_j + 1};
        }
        for (int j = block_j - 1; j >= 0; j--) {
            if (block_map[block_i][j]) break;
            skate_stop[1][block_i][j] = {block_i, block_j - 1};
        }
        for (int i = block_i + 1; i < n; i++) {
            if (block_map[i][block_j]) break;
            skate_stop[2][i][block_j] = {block_i + 1, block_j};
        }
        for (int i = block_i - 1; i >= 0; i--) {
            if (block_map[i][block_j]) break;
            skate_stop[3][i][block_j] = {block_i - 1, block_j};
        }
    };

    vector seen(n, vector<bool>(n, false));
    vector prev(n,
                vector<tuple<int, int, char, char>>(n, {-1, -1, '?', '?'}));

    auto bfs = [&](int si, int sj, int gi, int gj) -> vector<pair<char, char>> {
        auto start = toki.gettime();
        for(int i = 0; i < n; i++) {
            for (int j = 0; j < n; j++) {
                seen[i][j] = false;
            }
        }
        queue<pair<int, int>> que;
        que.push({si, sj});
        seen[si][sj] = true;
        while (!que.empty()) {
            const auto [i, j] = que.front();
            que.pop();
            bool break_flag = false;
            for (char dir : dirs) {
                const auto [di, dj] = dij(dir);
                // move
                {
                    const int ni = i + di;
                    const int nj = j + dj;
                    if (ni < 0 || ni >= n || nj < 0 || nj >= n) continue;
                    if (block_map[ni][nj]) continue;
                    if (seen[ni][nj]) continue;
                    seen[ni][nj] = true;
                    prev[ni][nj] = {i, j, 'M', dir};
                    que.push({ni, nj});
                    break_flag |= (ni == gi && nj == gj);
                }
                // skate
                {
                    const auto [ni, nj] = skate_stop[dir2idx(dir)][i][j];
                    if (ni < 0 || ni >= n || nj < 0 || nj >= n) continue;
                    if (block_map[ni][nj]) continue;
                    if (seen[ni][nj]) continue;
                    seen[ni][nj] = true;
                    prev[ni][nj] = {i, j, 'S', dir};
                    que.push({ni, nj});
                    break_flag |= (ni == gi && nj == gj);
                }
            }
            if (break_flag) break;
        }
        if (!seen[gi][gj]) {
            return {};
        }
        vector<pair<char, char>> ret;
        int i = gi, j = gj;
        while (i != si || j != sj) {
            const auto [pi, pj, act, dir] = prev[i][j];
            ret.push_back({act, dir});
            i = pi;
            j = pj;
        }
        reverse(ret.begin(), ret.end());
        bfs_time += toki.gettime() - start;
        return ret;
    };

    vector<vector<int>> visit_counts(n, vector<int>(n, 0));
    auto insert_block =
        [&](int si, int sj,
            const vector<pair<char, char>> &ops) -> vector<pair<char, char>> {
        {
            for(int i = 0; i < n; i++) {
                for (int j = 0; j < n; j++) {
                    visit_counts[i][j] = 0;
                }
            }
            int i = si, j = sj;
            visit_counts[i][j]++;
            for (const auto [act, dir] : ops) {
                int pi = i, pj = j;
                if(act == 'M') {
                    const auto [di, dj] = dij(dir);
                    i += di;
                    j += dj;
                    visit_counts[i][j]++;
                } else if (act == 'S') {
                    tie(i, j) = skate_stop[dir2idx(dir)][i][j];
                    if(dir == 'L') for(int tj = pj-1; tj >= j; tj--) {
                        visit_counts[i][tj]++;
                    }
                    else if(dir == 'R') for(int tj = pj+1; tj <= j; tj++) {
                        visit_counts[i][tj]++;
                    }
                    else if(dir == 'U') for(int ti = pi-1; ti >= i; ti--) {
                        visit_counts[ti][j]++;
                    }
                    else if(dir == 'D') for(int ti = pi+1; ti <= i; ti++) {
                        visit_counts[ti][j]++;
                    }
                }
            }
        }
        vector<pair<char, char>> ret;
        int i = si, j = sj;
        for (const auto [act, dir] : ops) {
            int pi = i, pj = j;
            ret.push_back({act, dir});
            if (act == 'M') {
                const auto [di, dj] = dij(dir);
                i += di;
                j += dj;
                visit_counts[i][j]--;
            } else if (act == 'S') {
                tie(i, j) = skate_stop[dir2idx(dir)][i][j];
                if(dir == 'L') for(int tj = pj-1; tj >= j; tj--) {
                    visit_counts[i][tj]--;
                }
                else if(dir == 'R') for(int tj = pj+1; tj <= j; tj++) {
                    visit_counts[i][tj]--;
                }
                else if(dir == 'U') for(int ti = pi-1; ti >= i; ti--) {
                    visit_counts[ti][j]--;
                }
                else if(dir == 'D') for(int ti = pi+1; ti <= i; ti++) {
                    visit_counts[ti][j]--;
                }
            }
            for (char dir : dirs) {
                const auto [di, dj] = dij(dir);
                const int ni = i + di;
                const int nj = j + dj;
                if (ni < 0 || ni >= n || nj < 0 || nj >= n) continue;
                if (!block_candidates[ni][nj]) continue;
                if (block_map[ni][nj]) continue;
                if (goal_map[ni][nj]) continue;
                if (visit_counts[ni][nj] > 0) continue;
                block_map[ni][nj] = true;
                update_skate_stop(ni, nj);
                ret.push_back({'A', dir});
            }
        }
        return ret;
    };

    vector<pair<char, char>> ret;
    for (int goal = 1; goal < m; goal++) {
        const auto [si, sj] = goals[goal - 1];
        const auto [gi, gj] = goals[goal];
        vector<pair<char, char>> ops = bfs(si, sj, gi, gj);
        if (ops.empty()) {
            return {};
        }
        auto ops_with_block = insert_block(si, sj, ops);
        ret.insert(ret.end(), ops_with_block.begin(), ops_with_block.end());
    }
    block_candidates = block_map;
    return ret;
}

inline ll common::calc_score(const vector<pair<char, char>> &operations,
                             bool check) {
    if (operations.empty()) {
        return 0;
    }
    if (!check) {
        return m + 2 * n * m - operations.size();
    }
    cerr << "Not Implemented yet" << endl;
    assert(false);
}

#endif