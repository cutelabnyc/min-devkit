/// @file
///	@ingroup 	minexamples
///	@copyright	Copyright 2018 The Min-DevKit Authors. All rights reserved.
///	@license	Use of this source code is governed by the MIT License found in the License.md file.

#include "c74_min.h"
#include <math.h>
#include "../shared/signal_routing_objects.h"
#include "../../pink-trombone/src/Tract.hpp"
#include "../../pink-trombone/src/WhiteNoise.hpp"
#include "../../pink-trombone/src/Biquad.hpp"

using namespace c74::min;

class pt_tract : public object<pt_tract>, public sample_operator<2, 1> {
private:
	t_tractProps tractProps;
	Tract *_tract;

	// Might want to move these out of the object as well
	WhiteNoise *_noise;
	Biquad *_fricativeBiquad;

	// TODO: Find a better way to manage these
	float tongueX = 0.5;
	float tongueY = 0.5;
	float constrictionX = 0.0;
	float constrictionY = 0.0;
	float fricativeIntensity = 0.0;
	bool constrictionActive = false;
public:
	MIN_DESCRIPTION {"Physical model of a tract."};
	MIN_TAGS {"audio, physical modeling, pink trombone"};
	MIN_AUTHOR {"CuteLab NYC"};
	MIN_RELATED {"pt.glottis~"};

	inlet<>  in1 {this, "(signal) Glottis sound output"};
	inlet<>  in2 {this, "(signal) Glottis noise modulation"};
	outlet<> out1 {this, "(signal) Modeled Sound", "signal"};

	pt_tract(const atoms& args = {}):
		_tract{nullptr}
	{
		// TODO: Figure out some way to make these tract properties dynamic
		initializeTractProps(&tractProps, NUM_CONSTRICTIONS);

		// For now, just assume a 48k sample rate
		double sr = 48000;
		_noise = new WhiteNoise(sr * 2.0);
		_fricativeBiquad = new Biquad(sr);
		_tract = new Tract(sr, 1, &tractProps);

		_fricativeBiquad->setGain(1.0);
		_fricativeBiquad->setQ(0.5);
		_fricativeBiquad->setFrequency(1000);
	}

	~pt_tract() {
		free(_tract);
		free(_noise);
		free(_fricativeBiquad);
	}

	void pt_tract::processConstrictions() {
		sample_t tongueIndex = tongueX * ((sample_t) (_tract->tongueIndexUpperBound() - _tract->tongueIndexLowerBound())) + _tract->tongueIndexLowerBound();
		sample_t innerTongueControlRadius = 2.05;
		sample_t outerTongueControlRadius = 3.5;
		sample_t tongueDiameter = tongueY * (outerTongueControlRadius - innerTongueControlRadius) + innerTongueControlRadius;
		sample_t constrictionMin = -2.0;
		sample_t constrictionMax = 2.0;
		
		sample_t constrictionIndex = this->constrictionX * (sample_t) _tract->getTractIndexCount();
		sample_t constrictionDiameter = this->constrictionY * (constrictionMax - constrictionMin) + constrictionMin;
		if (this->constrictionActive == false) {
			constrictionDiameter = constrictionMax;
		} else {
			this->fricativeIntensity += 0.1; // TODO ex recto
			this->fricativeIntensity = fmin(1.0, this->fricativeIntensity);
		}
		
		_tract->setRestDiameter(tongueIndex, tongueDiameter);
		_tract->setConstriction(constrictionIndex, constrictionDiameter, this->fricativeIntensity);
	}

	sample operator()( sample in1, sample in2) {

		sample vocalOutput = 0.0;
		sample fri = _fricativeBiquad->runStep(_noise->runStep());

		// TOOD: Figure out how to manage the whole lambda situation,
		//  given the way that the tract is upsampled
		sample lambda1 = 0.0;
		sample lambda2 = 0.5;
		_tract->runStep(in1, fri, lambda1, in2);
		vocalOutput += _tract->lipOutput + _tract->noseOutput;
		_tract->runStep(in1, fri, lambda2, in2);
		vocalOutput += _tract->lipOutput + _tract->noseOutput;

		this->processConstrictions();

		_tract->finishBlock();
		return vocalOutput;
	}
};

MIN_EXTERNAL(pt_tract);
