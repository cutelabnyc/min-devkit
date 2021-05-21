/// @file
///	@ingroup 	minexamples
///	@copyright	Copyright 2018 The Min-DevKit Authors. All rights reserved.
///	@license	Use of this source code is governed by the MIT License found in the License.md file.

#include "c74_min.h"
#include "../shared/signal_routing_objects.h"
#include "../../pink-trombone/src/Glottis.hpp"
#include "../../pink-trombone/src/WhiteNoise.hpp"
#include "../../pink-trombone/src/Biquad.hpp"

class pt_glottis : public signal_routing_base<pt_glottis>, public sample_operator<0, 2> {
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

	// above we inherited from sample_operator<3,1> which means 3 inputs and 1 output for our calculate method
	// we still need to create the interface for the object though, which includes the assistance strings...

	inlet<>  in1 {this, "(signal) Frequency"};
	outlet<> out1 {this, "(signal) Modeled Sound", "signal"};
	outlet<> out2 {this, "(signal) Modeled Noise", "signal"};

	pt_glottis(const atoms& args = {}) {
		// For now, just assume a 48k sample rate
		double sr = 48000;
		_glottis = new Glottis(sr);
		_noise = new WhiteNoise(sr * 2.0);
		_aspirateBiquad = new Biquad(sr);
	
		_aspirateBiquad->setGain(1.0);
		_aspirateBiquad->setQ(0.5);
		_aspirateBiquad->setFrequency(500);
	}

	~pt_glottis() {
		free(_glottis);
		free(_noise);
		free(_aspirateBiquad);
	}

	/// Call operator: process a single sample

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
