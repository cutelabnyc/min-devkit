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

	inlet<>  in1 {this, "(signal) Trigger/Gate"};
	inlet<>  in_attack {this, "(signal) Attack (msec)"};
	inlet<>  in_hold {this, "(signal) Hold (msec)"};
	inlet<>  in_decay {this, "(signal) Decay (msec)"};
	inlet<>  in_curve {this, "(signal) Curve (-1 to 1)"};
	outlet<> out1 {this, "(signal) Envelope", "signal"};

	enum env_stage {
		stage_attack = 1,
		stage_hold = 2,
		stage_decay = 3,
		stage_end = 4
	};

	message<> dspsetup {this, "dspsetup",
		MIN_FUNCTION {
			m_msec_per_sample = 1000.0 / samplerate();
		   return {};
	   }
	};

	sample operator()(sample trigger, sample attack = 1.0, sample hold = 1.0, sample decay = 1.0, sample curve = 0.5) {

		sample output = 0.0;
		m_attack = fmax(0.001, attack);
		m_hold = fmax(0.001, hold);
		m_decay = fmax(0.001, decay);

		// Generate fades for attack:
		// 1 -> 1 -> 0.5 -> 0 -> 0
		sample fadeAttack = fmax(0, fmin(1, ((2 * curve) - 0.5)));
		// 0 -> 0 -> 0.5 -> 1 -> 1
		sample fadeAttackInv = fmax(0, fmin(1, ((-2 * curve) + 1.5)));

		// Generate fades for decay:
		sample fadeDecay, fadeDecayInv;
		if (curve >= 0.25 && curve <= 0.75) {fadeDecay = fadeAttackInv;}
		else if (curve < 0.25) {fadeDecay = (4 * curve);}
		else {fadeDecay = (4 * curve) - 3;}
		fadeDecayInv = -1 * fadeDecay + 1;

		// calculate the possible slopes
		sample preramp = (m_stage == stage_decay ? 1 - m_phase: m_phase);
		sample slopeExp = pow(preramp, M_E); // e^(in1)
		sample slopeLog = fmax(0, (log(preramp)) + 1); // natural log of in1

		if (trigger > 0 && this->m_last_trigger <= 0) {
			m_active = true;
			m_phase = 0;
			m_stage = stage_attack;
		}

		m_last_trigger = trigger;

		if (m_active) {
			if (m_stage == stage_attack) {
				output = (slopeExp * fadeAttackInv) + (slopeLog * fadeAttack);
				m_phase += (m_msec_per_sample / m_attack);
			}

			else if (m_stage == stage_hold) {
				output = 1.0;
				m_phase += (m_msec_per_sample / m_hold);
			}

			else if (m_stage == stage_decay) {
				output = (slopeExp * fadeDecay) + (slopeLog * fadeDecayInv);
				m_phase += (m_msec_per_sample / m_decay);
			}

			else {
				output = 0;
				m_active = false;
			}

			if (m_phase >= 1.0) {
				m_stage = (env_stage) ((int) m_stage + 1);
				m_phase = 0.0;
			}
		}

		return output;
	}

private:
	bool m_active = false;
	sample m_last_trigger = 0;
	sample m_msec_per_sample = 1.0;
	sample m_phase = 0.0;
	sample m_attack, m_hold, m_decay;
	env_stage m_stage = stage_attack;
};

MIN_EXTERNAL(cute_env);
