/// @file
///	@ingroup 	minexamples
///	@copyright	Copyright 2018 The Min-DevKit Authors. All rights reserved.
///	@license	Use of this source code is governed by the MIT License found in the License.md file.

#include "c74_min_unittest.h"	// required unit test header
#include "pt.tract_tilde.cpp"	// need the source of our object so that we can access it

// Unit tests are written using the Catch framework as described at
// https://github.com/philsquared/Catch/blob/master/docs/tutorial.md

SCENARIO("object produces correct output") {
	ext_main(nullptr);    // every unit test must call ext_main() once to configure the class

	GIVEN("An instance of pt.tract~") {

		test_wrapper<pt_tract> an_instance;
		pt_tract&              my_object = an_instance;

		// TODO: Some tests
	}
}
