/// @file
///	@ingroup 	minexamples
///	@copyright	Copyright 2018 The Min-DevKit Authors. All rights reserved.
///	@license	Use of this source code is governed by the MIT License found in the License.md file.

#include "c74_min.h"
#include "../shared/signal_routing_objects.h"
#include "../../pink-trombone/src/Glottis.hpp"
#include "../../pink-trombone/src/WhiteNoise.hpp"
#include "../../pink-trombone/src/Biquad.hpp"

using namespace c74::min;

class pt_glottis : public object<pt_glottis>, public sample_operator<0, 2> {
private:
	Glottis *_glottis;

	// Gonna want to move these out of here eventually, but they're okay to leave in for now
	WhiteNoise *_noise;
	Biquad *_aspirateBiquad;
public:
	MIN_DESCRIPTION {"Physical model of a glottis (voicebox)."};
	MIN_TAGS {"audio, physical modeling, pink trombone"};
	MIN_AUTHOR {"CuteLab NYC"};
	MIN_RELATED {"pt.tract~"};

	inlet<>  in1 {this, "(signal) Frequency"};
	outlet<> out1 {this, "(signal) Modeled Sound", "signal"};
	outlet<> out2 {this, "(signal) Modeled Noise", "signal"};

	attribute<number, threadsafe::yes> frequency { this, "frequency", 140.0,
		description {"Target frequency of the modeled glottis"},
		setter { MIN_FUNCTION {
			number newFrequency = fmax(1.0, args[0]);
			if (_glottis)
				_glottis->setTargetFrequency(newFrequency);
			return { newFrequency };
		}}
	};

	attribute<number, threadsafe::yes> tenseness { this, "tenseness", 0.6,
		description {"Tenseness of the modeled glottis"},
		setter { MIN_FUNCTION {
			if (_glottis)
				_glottis->setTargetTenseness(args[0]);
			return args;
		}}
	};

	pt_glottis(const atoms& args = {}):
		_glottis{nullptr},
		_noise{nullptr}
	{
		// For now, just assume a 48k sample rate
		double sr = 48000;
		_glottis = new Glottis(sr);
		_noise = new WhiteNoise(sr * 2.0);
		_aspirateBiquad = new Biquad(sr);
	
		_aspirateBiquad->setGain(1.0);
		_aspirateBiquad->setQ(0.5);
		_aspirateBiquad->setFrequency(500);

		_glottis->setTargetFrequency(frequency);
		_glottis->setTargetTenseness(tenseness);
	}

	~pt_glottis() {
		free(_glottis);
		free(_noise);
		free(_aspirateBiquad);
	}

	samples<2> operator()() {

		// Noise
		sample purenoise = _noise->runStep();
		sample asp = _aspirateBiquad->runStep(purenoise);
		
		// Glottis
		sample lambda1 = 0;
		
		sample glot = _glottis->runStep(lambda1, asp);
		sample gnoise = _glottis->getNoiseModulator();

		_glottis->finishBlock();

		return {{glot, gnoise}};
	}
};

MIN_EXTERNAL(pt_glottis);
