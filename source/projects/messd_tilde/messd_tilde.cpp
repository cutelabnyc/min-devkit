/// @file
///	@ingroup 	cutelab
///	@copyright	Copyright 2021 CuteLab LLC. All rights reserved.
///	@license	Use of this source code is governed by the MIT License found in the License.md file.

#include "c74_min.h"
#include "../shared/signal_routing_objects.h"

using namespace c74::min;

class messd : public object<messd>, public sample_operator<1, 4> {
private:
    lib::sync m_oscillator;    // note: must be created prior to any attributes that might set parameters below

public:
    MIN_DESCRIPTION	{ "Mess'd Up, as a Max object" };
    MIN_TAGS		{ "time" };
    MIN_AUTHOR		{ "CuteLab" };
    MIN_RELATED		{ "metro, phasor~" };

    inlet<>  inlet_1 {this, "(signal) Clock (optional)"};
	outlet<> out1 {this, "(signal) Tempo-scaled Clock", "signal"};
    outlet<> out2 {this, "(signal) Downbeats", "signal"};
    outlet<> out3 {this, "(signal) Multiplied Clock", "signal"};
    outlet<> out4 {this, "(signal) Phased Clock", "signal"};

    attribute<bool> downbeat_latch { this, "Latch to Downbeat", false };
    attribute<bool> modulating { this, "Modulating Active", false };
    attribute<int, threadsafe::no, limit::clamp> scale { this, "Interval Scale", 1, range { 1, 9 } };
    attribute<int, threadsafe::no, limit::clamp> beats { this, "Beats per Measure", 1, range { 1, 9 } };
    attribute<int, threadsafe::no, limit::clamp> basis { this, "Power of Two Basis", 0, range { -5, 5} };
    attribute<int, threadsafe::no, limit::clamp> trunc { this, "Measure Truncation", 0, range { 0, 8 } };
    attribute<double, threadsafe::no, limit::wrap> phase { this, "Phase", 0.0, range { 0.0, 1.0 } };
    attribute<double, threadsafe::no, limit::clamp> tempo { this, "Tempo", 120, range { 20.0, 400.0 } };

    message<> invert {this, "invert", "Invert",
        MIN_FUNCTION {
            cout << "invert" << endl;
            return args;
        }
    };

    message<> store {this, "store", "Store",
        MIN_FUNCTION {
            cout << "store" << endl;
            return args;
        }
    };

    message<> recall {this, "recall", "Recall",
        MIN_FUNCTION {
            cout << "recall" << endl;
            return args;
        }
    };

    message<> reset {this, "reset", "Reset",
        MIN_FUNCTION {
            cout << "reset" << endl;
            return args;
        }
    };

	samples<4> operator()(sample in1) {
        m_oscillator.frequency(tempo / 60.0, samplerate());
        double op = m_oscillator();
        return { op, op, op, op };
	}
};

MIN_EXTERNAL(messd);
