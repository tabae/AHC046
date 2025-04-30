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
  State() : score(-inf) { block_candidates.resize(n, vector<bool>(n, false)); };
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
  res.operations = common::solve(res.block_candidates);
  res.calc_score(res.operations);
  return res;
}

State State::generateState(const State &input_state) {
  State res;
  res.block_candidates = input_state.block_candidates;
  int i = ryuka.rand(n);
  int j = ryuka.rand(n);
  res.block_candidates[i][j] = !res.block_candidates[i][j];
  res.operations = common::solve(res.block_candidates);
  res.calc_score(res.operations);
  return res;
}

#endif
