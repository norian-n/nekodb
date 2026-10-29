#pragma once
#include <iostream>
#include <cstring>

const int8_t testInDevelopment {2}; 

inline void contributeTestResultToStatistics(int8_t testResult) {
    if (testResult == testInDevelopment)
        std::cout << "DEVEL"  << std::endl;
    else if (! testResult)
        std::cout << "FAIL" << std::endl;
    else
        std::cout << "PASS" << std::endl;
}