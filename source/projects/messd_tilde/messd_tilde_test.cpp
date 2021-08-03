/// @file
///	@ingroup 	cutelab
///	@copyright	Copyright 2021 CuteLab LLC. All rights reserved.
///	@license	Use of this source code is governed by the MIT License found in the License.md file.

#include "c74_min_unittest.h"     // required unit test header
#include "messd_tilde.cpp"

// Unit tests are written using the Catch framework as described at
// https://github.com/philsquared/Catch/blob/master/docs/tutorial.md

SCENARIO("nothing") {
    ext_main(nullptr);    // every unit test must call ext_main() once to configure the class

    //	std::cout << "Hello waiter" << std::endl;
    //	auto start = std::chrono::high_resolution_clock::now();
    //	std::this_thread::sleep_for(2s);
    //	auto end = std::chrono::high_resolution_clock::now();
    //	std::chrono::duration<double, std::milli> elapsed = end-start;
    //	std::cout << "Waited " << elapsed.count() << " ms\n";

    GIVEN("An instance of messd") {

        test_wrapper<messd> an_instance;
        messd&              my_object = an_instance;
    }
}
