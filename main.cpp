#include <iostream>
#include <vector>
#include <map>
#include <chrono>
#include "powers_table.h"

using namespace std;

extern const unsigned long long pows[][SIZE];

const int N = 10;
const int DEPTH = 19;
const uint64_t MAX_LONG_JAVA = 9223372036854775807;

using namespace std;

vector<uint64_t>& gen_next_candidates (const vector<uint64_t>& source_arr, const bool optimize = false);
inline uint64_t get_pow_sum (const uint64_t& num);
inline bool is_armstrong (const uint64_t& num);

int main()
{
    auto start_time = std::chrono::steady_clock::now();

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
            if (new_candidate > MAX_LONG_JAVA)
                continue;
            candidates_storage[i+1]->push_back(new_candidate);
        }
    }
    auto end_time = std::chrono::steady_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
    cout << "GENERATED TIME: " << elapsed.count() << " ms" << endl;

    map <uint64_t, int> armstrongs;
    for (const auto& arr : candidates_storage)
        for (const auto& elem : *arr) {
            uint64_t pow_sum = get_pow_sum(elem);
            if (is_armstrong(pow_sum))
                armstrongs[pow_sum]++;
        }

    for (const auto& pair : armstrongs)
        cout << pair.first << endl;

    cout << "size: " << armstrongs.size() << endl;

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
                if (new_candidate > MAX_LONG_JAVA)
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
    int ranks[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    while (copy_num) {
        uint64_t remainder = copy_num % N;
        ranks[remainder]++;
        length++;
        copy_num /= N;
    }

    uint64_t res = 0;
    for (int i = SIZE-1; i >= 0; i--) {
        if (ranks[i])
            res += ranks[i] * pows[i][length];
        if (res > num)
            return false;
    }
    return res == num;
}
