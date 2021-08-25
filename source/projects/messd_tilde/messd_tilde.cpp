/// @file
///	@ingroup 	cutelab
///	@copyright	Copyright 2021 CuteLab LLC. All rights reserved.
///	@license	Use of this source code is governed by the MIT License found in the License.md file.

#include "c74_min.h"
#include "../shared/signal_routing_objects.h"

#define MESSD_UP 1

extern "C" {
#include "cutesynth.h"
}
using namespace c74::min;

#define MESSD_UP 1
#define NUM_INPUTS 5
#define NUM_OUTPUTS 4
#define NUM_ARGS (NUM_INPUTS + NUM_OUTPUTS)

class messdup : public object<messdup>, public sample_operator<1, 4> {
private:
    double ins[NUM_INPUTS];
    double outs[NUM_OUTPUTS];

public:
    MIN_DESCRIPTION{ "Mess'd Up, as a Max object" };
    MIN_TAGS{ "time" };
    MIN_AUTHOR{ "CuteLab" };
    MIN_RELATED{ "metro, phasor~" };

    inlet<>  inlet_1{ this, "(int) Pour les messages" };
    
    outlet<> out1{ this, "(signal) Tempo-scaled Clock", "signal" };
    outlet<> out2{ this, "(signal) Downbeats", "signal" };
    outlet<> out3{ this, "(signal) Multiplied Clock", "signal" };
    outlet<> out4{ this, "(signal) Phased Clock", "signal" };
    
    messdup()
    {
        MS_init(&messd);
        ins[CLOCK_KNOB] = 10;
        ins[DOWNBEAT_IN] = 300;
        ins[SUBDIVISION_IN] = 400;
        ins[PHASE_IN] = 0;
    }
    
    ~messdup()
    {
        MS_destroy(&messd);
    }
    
    message<> tempo { this, "tempo", "Tempo",
        MIN_FUNCTION {
            ins[CLOCK_KNOB] = args[0];
            return {};
        }
    };

    message<> beats { this, "beats", "Beats Per Measure",
        MIN_FUNCTION {
            ins[DOWNBEAT_IN] = args[0];
            return {};
        }
    };

    message<> subdivision { this, "subdivision", "Subdivision Per Measure",
        MIN_FUNCTION {
            ins[SUBDIVISION_IN] = args[0];
            return {};
        }
    };

    message<> phase { this, "phase", "Phased rhythm",
        MIN_FUNCTION {
            ins[PHASE_IN] = args[0];
            return {};
        }
    };


    samples<4> operator()(sample in){
        
        MS_process(&messd, ins, outs);
            
        return { outs[0], outs[1], outs[2], outs[3] };
    }
};

MIN_EXTERNAL(messdup);
