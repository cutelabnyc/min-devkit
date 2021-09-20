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

static void _scale_messdUp(messd_ins_t *ins)
{
    ins->tempo = (ins->tempo > 0 ? ins->tempo : 1);
    ins->beatsPerMeasure = (ins->beatsPerMeasure > 0 ? ins->beatsPerMeasure : 1);
    ins->subdivisionsPerMeasure = (ins->subdivisionsPerMeasure > 0 ? ins->subdivisionsPerMeasure : 1);
    ins->phase = (ins->phase <= 1 && ins->phase >= 0 ? ins->phase : 0);
    ins->pulseWidth = (ins->pulseWidth < 1 && ins->pulseWidth > 0 ? ins->pulseWidth : 0.5);
}

class messdup : public object<messdup>, public sample_operator<1, 4> {
private:
    messd_t messd;
    messd_ins_t messd_ins;
    messd_outs_t messd_outs;
	float lastClockIn = 0;
	int measuredClockWavelength = 0;

public:
    MIN_DESCRIPTION{ "Mess'd Up, as a Max object" };
    MIN_TAGS{ "time" };
    MIN_AUTHOR{ "CuteLab" };
    MIN_RELATED{ "metro, phasor~" };

    inlet<>  inlet_clock{ this, "(signal) External clock", "signal"};
    inlet<>  inlet_mess{ this, "(int) Pour les messages" };

    outlet<> out1{ this, "(signal) Tempo-scaled Clock", "signal" };
    outlet<> out2{ this, "(signal) Downbeats", "signal" };
    outlet<> out3{ this, "(signal) Multiplied Clock", "signal" };
    outlet<> out4{ this, "(signal) Phased Clock", "signal" };
	outlet<> out5{ this, "UI Sync Messages", "list" };

    messdup()
    {
        MS_init(&messd);
    }

    ~messdup()
    {
        MS_destroy(&messd);
    }

    message<> tempo{ this, "tempo", "Tempo",
        MIN_FUNCTION {
            messd_ins.tempo = args[0];
            return {};
        }
    };

    message<> beats{ this, "beats", "Beats Per Measure",
        MIN_FUNCTION {
            messd_ins.beatsPerMeasure = args[0].a_w.w_long;
            return {};
        }
    };

    message<> subdivision{ this, "subdivision", "Subdivision Per Measure",
        MIN_FUNCTION {
            messd_ins.subdivisionsPerMeasure = args[0].a_w.w_long;
            return {};
        }
    };

    message<> phase{ this, "phase", "Phased rhythm",
        MIN_FUNCTION {
            messd_ins.phase = args[0];
            return {};
        }
    };

    message<> wrap{ this, "wrap", "Wrap rhythm",
        MIN_FUNCTION {
            messd_ins.wrap = args[0].a_w.w_long;
            return {};
        }
    };

    message<> invert{ this, "invert", "Invert the subdivision",
        MIN_FUNCTION {
            messd_ins.invert = args[0];
            return {};
        }
    };

    message<> modulate{ this, "modulate", "Metric modulationm",
        MIN_FUNCTION {
            messd_ins.metricModulation = args[0];
            return {};
        }
    };

    message<> pulsewidth{ this, "pulsewidth", "Pulse Width",
        MIN_FUNCTION {
            messd_ins.pulseWidth = args[0];
            return {};
        }
    };
    
    message<> latch{ this, "latch", "Latch to Downbeat",
        MIN_FUNCTION {
            messd_ins.latchToDownbeat = args[0];
            return {};
        }
    };
    
    message<> roundtrip{ this, "roundtrip", "Round Trip v.s. One Way",
        MIN_FUNCTION {
            messd_ins.isRoundTrip = args[0];
            return {};
        }
    };
    
    message<> reset{ this, "reset", "Reset tempo scale",
        MIN_FUNCTION {
            messd_ins.reset = args[0];
            return {};
        }
    };


    samples<4> operator()(sample in){
        messd_ins.delta = 1000.0 / samplerate();
        messd_ins.ext_clock = in;
		messd_ins.ext_clock_connected = inlet_clock.has_signal_connection();

		if (lastClockIn > in) {
			float forcedHint = 1.0 / (fmax(fmin(measuredClockWavelength, samplerate()), samplerate() / 1000.0));
			MS_clock_wavelength_hint(&messd, forcedHint);
			measuredClockWavelength = 0;
		} else {
			measuredClockWavelength++;
		}
		lastClockIn = in;

        _scale_messdUp(&messd_ins);

		int subdivisionsBeforeProcess = messd_ins.subdivisionsPerMeasure;
        MS_process(&messd, &messd_ins, &messd_outs);

		// Send messages
		if (subdivisionsBeforeProcess != messd_ins.subdivisionsPerMeasure) {
			out5.send("subdivisions", messd_ins.subdivisionsPerMeasure);
		}

        return { (double)messd_outs.downbeat, (double)messd_outs.beat, (double)messd_outs.subdivision, (double)messd_outs.phase };
    }
};

MIN_EXTERNAL(messdup);
