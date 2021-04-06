/// @file
///	@ingroup 	minexamples
///	@copyright	Copyright 2018 The Min-DevKit Authors. All rights reserved.
///	@license	Use of this source code is governed by the MIT License found in the License.md file.

#include "c74_min.h"
extern "C" {
#include "cuteop.h"
}

#define NUM_OSC 10

using namespace c74::min;


class uexkull : public object<uexkull>, public sample_operator<0, 1> {
private:
    bank_t bank;
    float fundamental = 0.0f;

public:
    MIN_DESCRIPTION{
        "A modular oscillator bank that emphasizes the effects of interference patterns."
        "Use this to create rich beating patterns, sum and difference tones, and "
        "sonic diffraction grates when used in spatialized multichannel."
        "Named after biosemiotician <a href='https://en.wikipedia.org/wiki/Jakob_Johann_von_Uexk%C3%BCll'>Jacob von Uexküll</a>." };
    MIN_TAGS{ "audio, oscillator, bank" };
    MIN_AUTHOR{ "CuteLab" };
    MIN_RELATED{ "oscbank~, cycle~, saw~, poly~" };

    uexkull(){
        BK_init(&bank, NUM_OSC, samplerate(), 0.0f, SIN);
        float array[10] = { 100, 250, 370, 420, 590, 630, 745, 882, 910, 1008 };

        for (int i = 0; i < NUM_OSC; i++) {
            BK_setFrequencies(&bank, array, NUM_OSC);
        }
    };

    ~uexkull(){
        BK_destroy(&bank);
    }

    inlet<>  in1{ this, "(float) fundamental" };
    outlet<> out1{ this, "(signal) summed osc bank", "signal" };

    argument<number> frequency_arg{
        this, "fundamental", "fundamental frequency in hertz.", MIN_ARGUMENT_FUNCTION {
            frequency = arg;
        }
    };

    message<> m_number{
        this, "float", "Set the fundamental frequency in Hz.", MIN_FUNCTION {
            frequency = args;
            return {};
        }
    };

    attribute<number> frequency{
        this, "fundamental", 1.0, description {"Fundamental frequency in Hz"}, setter {
            MIN_FUNCTION {
                    fundamental = args[0];
    // float array[10] = {100, 250, 370, 420, 590, 630, 745, 882, 910, 1008};

    // for (int i = 0; i < NUM_OSC; i++) {
    //     BK_setFrequencies(&bank, array, NUM_OSC);
    // }

        return args;
    }
}
    };

    sample operator()() {
        sample y = 0;
        for (int i = 0; i < NUM_OSC; i++){
            y += (sample)BK_process(&bank);
        }
        return y;
    }
};

// /**
    //  * _UX_diffractionSeries:
    //  *
    //  * This is the bread and butter of Uexkull, the heart and soul.
    //  */
    // void _UX_diffractionSeries(double *vector, uint16_t numElements)
    // {
    //     vector[0] = fundamental;
    //     for (int i = 0; i < numElements; i++)
    //     {
    //         if (i != 0)
    //         {
    //             vector[i] = vector[i - 1] * 2;
    //         }
    //     }
    // }

MIN_EXTERNAL(uexkull);
