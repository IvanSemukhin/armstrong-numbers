#include <iostream>
#include <vector>
#include <map>
#include <chrono>
#include <thread>
#include <mutex>
#include "powers_table.h"

using namespace std;

extern const uint64_t pows[][SIZE];
const int N = 10;
const int THR_NUM = 4;
const uint64_t MAX = 0x7FFFFFFFFFFFFFFF;

mutex g_lock;

vector<uint64_t>& gen_next_candidates (const vector<uint64_t>& source_arr, const bool optimize = false);
inline uint64_t get_pow_sum (const uint64_t& num);
inline bool is_armstrong (const uint64_t& num);
inline int get_num_len (const uint64_t& num);
void thread_is_armstrong(const vector< vector<uint64_t>* >& storage, const vector<int>& depth_indx, map <uint64_t, int>& armstrongs);


int main()
{
    auto start_time = std::chrono::steady_clock::now();

    const int DEPTH = get_num_len(MAX);
    cout << "START GENERATE TO DEPTH: " << DEPTH << endl;
    vector<uint64_t> zero_arr = {0};
    vector<uint64_t> init_arr = {1, 2, 3, 4, 5, 6 ,7 ,8, 9};
    vector< vector<uint64_t>* > candidates_storage;
    candidates_storage.push_back(&zero_arr);
    candidates_storage.push_back(&init_arr);
    for (int i = 1; i < DEPTH; i++) {
        if (i == DEPTH-1)
            candidates_storage.push_back(&gen_next_candidates(*candidates_storage[i], true));
        else
            candidates_storage.push_back(&gen_next_candidates(*candidates_storage[i]));

        for (const auto& element : *candidates_storage[i]) {
            uint64_t new_candidate = element * N;
            if (new_candidate > MAX)
                continue;
            candidates_storage[i+1]->push_back(new_candidate);
        }
    }
    auto end_time = std::chrono::steady_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
    cout << "GENERATED TIME: " << elapsed.count() << " ms" << endl;

    vector < vector<int> > indx_threads(THR_NUM);
    for (int i = 1; i <= DEPTH; i++)
        indx_threads[i % THR_NUM].push_back(i);

    map <uint64_t, int> armstrongs;
    thread threads[THR_NUM];
    for (int i = 0; i < THR_NUM;i++)
        threads[i] = thread(thread_is_armstrong, cref(candidates_storage), cref(indx_threads[i]), ref(armstrongs));
    for (int i = 0; i < THR_NUM;i++)
        threads[i].join();

    cout << "armstrong numbers:" << endl;
    for (const auto& pair : armstrongs)
        if(pair.first <= MAX)
            cout << pair.first << endl;

    end_time = std::chrono::steady_clock::now();
    elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
    cout << "TOTAL TIME: " << elapsed.count() << " ms" << endl;

    return 0;
}

vector<uint64_t>& gen_next_candidates (const vector<uint64_t>& source_arr, const bool optimize) {
    auto res_next_arr = new vector<uint64_t>;
    for (const auto& element : source_arr) {
        int last = element % N;
        if(!last)
            continue;
        for (int i = last; i < N; i++) {
            uint64_t new_candidate = element * N + i;
            if (optimize) {
                if (new_candidate > MAX)
                    continue;
                else
                    res_next_arr->push_back(new_candidate);
            }
            else
                res_next_arr->push_back(new_candidate);
        }
    }
    return *res_next_arr;
}

inline uint64_t get_pow_sum (const uint64_t& num) {
    uint64_t copy_num = num;
    int length = 0;
    int mono[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    while (copy_num) {
        uint64_t remainder = copy_num % N;
        mono[length] = remainder;
        length++;
        copy_num /= N;
    }
    uint64_t res = 0;
    for(int i = 0; i < length; i++)
        res += pows[mono[i]][length];
    return res;
}

inline bool is_armstrong (const uint64_t& num) {
    if (!num)
	return false;
    uint64_t copy_num = num;
    int length = 0;
    int ranks[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    while (copy_num) {
        uint64_t remainder = copy_num % N;
        ranks[remainder]++;
        length++;
        copy_num /= N;
    }

    uint64_t res = 0;
    for (int i = N-1; i >= 0; i--) {
        if (ranks[i])
            res += ranks[i] * pows[i][length];
        if (res > num)
            return false;
    }
    return res == num;
}

inline int get_num_len (const uint64_t& num) {
    uint64_t copy_num = num;
    int length = 1;
    while (copy_num /= N)
        length++;
    return length;
}

void thread_is_armstrong(const vector< vector<uint64_t>* >& storage, const vector<int>& depth_indx, map <uint64_t, int>& armstrongs) {
    for (const auto& i : depth_indx) {
        for (const auto& elem : *storage[i]) {
            uint64_t pow_sum = get_pow_sum(elem);
            if (is_armstrong(pow_sum)) {
                g_lock.lock();
                armstrongs[pow_sum]++;
                g_lock.unlock();
            }
        }
    }
}
