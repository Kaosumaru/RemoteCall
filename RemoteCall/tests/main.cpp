#include <iostream>
#include <stdexcept>
#include "mtl/function_traits.hpp"
#include "mtl/make_pipe.hpp"

#ifndef _MSC_VER
#define lest_FEATURE_COLOURISE 1
#endif

//#include "lest.hpp"

using namespace std;





int main (int argc, char * argv[])
{
	auto x = mtl::make_pipe([](int a) { return a + 1; }, [](int a) { return a * 2; });
	auto t = x(5);

	return 0;
}
