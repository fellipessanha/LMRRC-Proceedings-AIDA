#pragma once

// compilation tests for concepts (these are not UNIT tests)

#include "BaseConcepts.ctest.hpp"
#include "Population.hpp"

#ifndef NDEBUG
namespace optframe
{

struct optframe_test_debug_testpareto_ispopulation_disable_runtime
{
   // This is Legacy Population, so should FAIL the next X2ESolution test
   // TestPopOrPareto<pair<Solution<int>,Evaluation<>>, Population<Solution<int>,Evaluation<>> > testPop;
};


} // namespace optframe

#endif // NDEBUG