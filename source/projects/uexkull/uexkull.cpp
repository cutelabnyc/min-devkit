/// @file
///	@ingroup 	minexamples
///	@copyright	Copyright 2018 The Min-DevKit Authors. All rights reserved.
///	@license	Use of this source code is governed by the MIT License found in the License.md file.

#include "c74_min.h"
#include <stdint.h>
extern "C" {
#include "cuteop.h"
}

using namespace c74::min;

/**
* _UX_diffractionSeries:
*
* This is the bread and butter of Uexkull, the heart and soul.
*/
void _UX_diffractionSeries(double *vector, uint16_t numElements)
{
    for (int i = 0; i < numElements; i++)
    {
        if (i != 0)
        {
            vector[i] = vector[i - 1] + (vector[i - 1] * 0.5f);
        }
    }
}

class uexkull : public object<uexkull>, public sample_operator<0, 1> {
private:
    bank_t bank;
    t_series series;
    float _fundamental = 0.0f;
    const int16_t numOsc = 10;

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
        series_init(&series, numOsc, _UX_diffractionSeries);
        series_process(&series, _fundamental);
        bank_init(&bank, numOsc, samplerate(), 0.0f, SIN);
        bank_setFrequencies(&bank, series_getSeries(&series), numOsc);
    };

    ~uexkull(){
        bank_destroy(&bank);
        series_destroy(&series);
    }

    inlet<>  in1{ this, "(float) fundamental" };
    outlet<> out1{ this, "(signal) summed osc bank", "signal" };

    argument<number> frequency_arg{
        this, "fundamental", "fundamental frequency in hertz.", MIN_ARGUMENT_FUNCTION {
            _fundamental = arg;
        }
    };

    message<> m_number{
        this, "float", "Set the fundamental frequency in Hz.",
            MIN_FUNCTION {
                _fundamental = args[0].a_w.w_float;
                series_process(&series, _fundamental);
                bank_setFrequencies(&bank, series_getSeries(&series), numOsc);
                return {};
            }
    };

//    attribute<number> fundamental{
//        this, "fundamental", 1.0, description {"Fundamental frequency in Hz"},
//        setter {
//            MIN_FUNCTION {
//                _fundamental = args[0];
//                series_process(&series, _fundamental);
//                bank_setFrequencies(&bank, (float *)series_getSeries(&series), NUM_OSC);
//                return args;
//            }
//        }
//    };

    sample operator()() {
        return (sample)bank_process(&bank);
    }
};

MIN_EXTERNAL(uexkull);
