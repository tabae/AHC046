#ifndef __ANNEALER_HPP__
#define __ANNEALER_HPP__

#include "ryuka.hpp"
#include "toki.hpp"

extern Timer toki;
extern RandGenerator ryuka;

template <class STATE>
struct IterationControl {
    long long iteration_counter;
    long long swap_counter;
    double average_time;
    double start_time;
    bool liner_cooling;
    bool save_best_answer;
    bool use_rollback;
    int gettime_interval;
    IterationControl()
        : iteration_counter(0),
          swap_counter(0),
          liner_cooling(false),
          save_best_answer(true),
          use_rollback(false),
          gettime_interval(1) {}
    STATE climb(double time_limit, STATE initial_state);
    STATE anneal(double time_limit, double temp_start, double temp_end,
                 STATE initial_state);
};

template <class STATE>
STATE IterationControl<STATE>::climb(double time_limit, STATE initial_state) {
    start_time = toki.gettime();
    average_time = 0;
    STATE best_state = initial_state;
    double time_stamp = start_time;
    cerr << "Start climbing...\n";
    while (time_stamp - start_time + average_time < time_limit) {
        if (use_rollback) {
            long long best_score = best_state.score;
            best_state.nextState();
            if (best_state.score > best_score) {
                best_score = best_state.score;
                swap_counter++;
            } else {
                best_state.rollback();
            }
        } else {
            STATE current_state = STATE::generateState(best_state);
            if (current_state.score > best_state.score) {
                swap(best_state, current_state);
                swap_counter++;
            }
        }
        iteration_counter++;
        if (iteration_counter % gettime_interval == 0) {
            time_stamp = toki.gettime();
            average_time = (time_stamp - start_time) / iteration_counter;
        }
    }
    cerr << "Iterated " << iteration_counter << " times and swapped "
         << swap_counter << " times.\n";
    return best_state;
}

template <class STATE>
STATE IterationControl<STATE>::anneal(double time_limit, double temp_start,
                                      double temp_end, STATE initial_state) {
    start_time = toki.gettime();
    average_time = 0;
    STATE current_state = initial_state;
    STATE answer_state = initial_state;
    double elapsed_time = 0;
    const double inv_time_limit = 1.0 / time_limit;
    cerr << "Start annealing...\n";
    while (elapsed_time + average_time < time_limit) {
        double temp_current;
        if (liner_cooling) {
            const double normalized_time = elapsed_time * inv_time_limit;
            temp_current =
                temp_start + (temp_end - temp_start) * normalized_time;
        } else {
            const double normalized_time = elapsed_time * inv_time_limit;
            temp_current = pow(temp_start, 1.0 - normalized_time) *
                           pow(temp_end, normalized_time);
        }

        if (use_rollback) {
            long long prev_score = current_state.score;
            current_state.nextState();
            long long delta = current_state.score - prev_score;
            if(save_best_answer && current_state.score > answer_state.score) {
                answer_state = current_state;
            }
            if (delta > 0 || ryuka.pjudge(exp(1.0 * delta / temp_current))) {
                swap_counter++;
            } else {
                current_state.rollback();
            }
        } else {
            STATE new_state = STATE::generateState(current_state);
            long long delta = new_state.score - current_state.score;
            if(save_best_answer && new_state.score > answer_state.score) {
                answer_state = new_state;
            }
            if (delta > 0 || ryuka.pjudge(exp(1.0 * delta / temp_current))) {
                swap(new_state, current_state);
                swap_counter++;
            }
        }

        iteration_counter++;
        if (iteration_counter % gettime_interval == 0) {
            elapsed_time = toki.gettime() - start_time;
            average_time = elapsed_time / iteration_counter;
        }
    }
    cerr << "Iterated " << iteration_counter << " times and swapped "
         << swap_counter << " times.\n";
    return save_best_answer ? answer_state : current_state;
}

#endif
