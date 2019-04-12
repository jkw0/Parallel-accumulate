#include <vector>
#include <array>
#include <thread>
#include <iostream>
#include <numeric>

using namespace std;

template<typename T>
T accumulate_parallel(vector<T> v, T init)
{
    if (v.empty()) return init;
    const unsigned int hardware_threads
         = thread::hardware_concurrency() != 0 ? thread::hardware_concurrency() : 2;

    vector<T> results(hardware_threads);
    vector<thread> threads;
    const unsigned int group = v.size() / hardware_threads;

    for (int i = 0; i < hardware_threads; i++)
        threads.emplace_back(thread([=, &results](T& result, int idx) {
        result = accumulate(v.begin()+idx, v.begin()+idx+group, (T)0);
    }, ref(results.at(i)), group*i));

    for (auto&& t : threads) t.join(); 
    
    T sum = accumulate(results.begin(), results.end(), init);
    
    if (int k = v.size() % hardware_threads) {
        for (int i = k; i > 0; i--)
            sum += v.at(v.size()-i);
    }

    return sum;
}


int main()
{
    vector<double> vec{1.1, 2.2, 3.3, 4.4, 5.5};
    
    cout << accumulate_parallel(vec, 5.0) << endl;
}