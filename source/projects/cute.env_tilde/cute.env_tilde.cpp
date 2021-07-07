/// @file
///	@ingroup 	cutelab
///	@copyright	Copyright 2018 The Min-DevKit Authors. All rights reserved.
///	@license	Use of this source code is governed by the MIT License found in the License.md file.

#include "c74_min.h"
#include "../shared/signal_routing_objects.h"

class cute_env : public signal_routing_base<cute_env>, public sample_operator<5, 1> {
public:
	MIN_DESCRIPTION {"CuteLab envelope generator"};
	MIN_TAGS {"audio, routing"};
	MIN_AUTHOR {"CuteLab"};

	inlet<>  in1 {this, "(signal) Trigger"};
	inlet<>  in_attack {this, "(signal) Attack (msec)"};
	inlet<>  in_hold {this, "(signal) Hold (msec)"};
	inlet<>  in_decay {this, "(signal) Decay (msec)"};
	inlet<>  in_curve {this, "(signal) Curve (-1 to 1)"};
	outlet<> out1 {this, "(signal) Envelope", "signal"};

	/// Process one sample

	samples<5> operator()(sample trigger, sample attack, sample hold, sample decay, sample curve) {
		return 0.5;
	}
};

MIN_EXTERNAL(cute_env);
