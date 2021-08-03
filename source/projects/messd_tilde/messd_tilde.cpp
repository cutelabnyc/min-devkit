/// @file
///	@ingroup 	cutelab
///	@copyright	Copyright 2021 CuteLab LLC. All rights reserved.
///	@license	Use of this source code is governed by the MIT License found in the License.md file.

#include "c74_min.h"
#include "../shared/signal_routing_objects.h"

using namespace c74::min;

class messd : public object<messd>, public sample_operator<1, 1> {
private:
    lib::sync m_oscillator;    // note: must be created prior to any attributes that might set parameters below

public:
    MIN_DESCRIPTION	{ "Mess'd Up, as a Max object" };
    MIN_TAGS		{ "time" };
    MIN_AUTHOR		{ "CuteLab" };
    MIN_RELATED		{ "metro" };

    inlet<>  inlet_1 {this, "(signal) Tempo in bpm"};
	outlet<> output {this, "(signal) Output", "signal"};

	sample operator()(sample in1) {
        sample tempo;
        if (inlet_1.has_signal_connection()) {
            tempo = in1;
        } else {
            tempo = 120.0;
        }

        m_oscillator.frequency(tempo / 60.0, samplerate());
        return m_oscillator();
	}
};

MIN_EXTERNAL(messd);
