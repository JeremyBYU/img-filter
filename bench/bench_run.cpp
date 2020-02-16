#include <iostream>
#include <sstream>
#include <vector>


#include <benchmark/benchmark.h>
#include "img_filters.hpp"


void BM_TestFilter(benchmark::State& state)
{
    
    for (auto _ : state) {
        img_filters::TestFilter("test");
    }
}

BENCHMARK(BM_TestFilter)->UseRealTime();

// Run the benchmark
BENCHMARK_MAIN();