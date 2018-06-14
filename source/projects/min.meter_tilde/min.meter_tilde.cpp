/// @file
///	@ingroup 	minexamples
///	@copyright	Copyright 2018 The Min-DevKit Authors. All rights reserved.
///	@license	Use of this source code is governed by the MIT License found in the License.md file.

#include "c74_min.h"

using namespace c74::min;
using namespace c74::min::ui;

class min_meter : public object<min_meter>, public ui_operator<140, 24>, public sample_operator<1, 0> {
public:
	MIN_DESCRIPTION {"Show audio gain levels"};
	MIN_TAGS {"ui"};
	MIN_AUTHOR {"Cycling '74"};
	MIN_RELATED {"meter~, live.meter~"};

	inlet<>  input {this, "(signal) audio to measure / visualize"};
	outlet<> output {this, "(number) value"};

	min_meter(const atoms& args = {})
	: ui_operator::ui_operator {this, args} {
		m_timer.delay(40);
	}

	message<> m_notify {this, "notify",
		MIN_FUNCTION {
			symbol s      = args[1];
			symbol msg    = args[2];
			void*  sender = args[3];
			void*  data   = args[4];

			// TODO: the following should be baked-in for ui operators
			return {c74::max::jbox_notify((c74::max::t_jbox*)maxobj(), s, msg, sender, data)};
		}};

	attribute<numbers> m_range {this, "range", { {0.0, 1.0}}};
	attribute<numbers> m_offset {this, "offset", { {10.0, 10.0}}};
	attribute<symbol>  m_label {this, "label", ""};
	attribute<symbol>  m_fontname {this, "fontname", "lato-light"};
	attribute<number>  m_fontsize {this, "fontsize", 14.0};
	attribute<color>   m_bgcolor {this, "bgcolor", color::black, title {"Background Color"}};
	attribute<color>   m_elementcolor {this, "elementcolor", color::white};
	attribute<color>   m_knobcolor {this, "knobcolor", color::gray, title {"Knob Color"}};

	message<> paint {this, "paint",
		MIN_FUNCTION {
			using namespace ui;

			target t {args};

			// ALL STYLE-AWARE ATTRS MUST BE UPDATED HERE
			// TODO: BAKE THIS INTO THE WRAPPER OF THE PAINT METHOD!
			c74::max::t_jrgba elemcolor {};
			c74::max::object_attr_getjrgba(*this, symbol("elementcolor"), &elemcolor);

			long              ac {};
			c74::max::t_atom* av {};
			c74::max::object_attr_getvalueof(*this, symbol("elementcolor"), &ac, &av);
			// TODO: need to free av?
			atoms a {av + 0, av + 1, av + 2, av + 3};
			m_elementcolor.set(a);

			c74::max::object_attr_getvalueof(*this, symbol("bgcolor"), &ac, &av);
			// TODO: need to free av?
			a = {av + 0, av + 1, av + 2, av + 3};
			m_bgcolor.set(a);

			auto value = (m_value - m_range[0]) / (m_range[1] - m_range[0]);
			auto pos   = ((t.width() - 3) * value) + 1;    // one pixel for each border and -1 for counting to N-1

			rect<fill> {// background
				t, color {m_bgcolor}};

			rect<> {// frame
				t, color {{0.3, 0.3, 0.3, 1.0}}, line_width {3.0}};

			rect<fill> {// active part of the slider
				t, color {m_elementcolor}, position {1.0, 1.0}, size {pos, -2.0}};

			rect<fill> {// slider knob
				t, color {m_knobcolor}, position {pos, 1.0}, size {4.0, -2.0}};

			text {// text display
				t, color {color::white}, position {m_offset[0], m_offset[1] + m_fontsize * 0.5}, fontface {m_fontname}, fontsize {m_fontsize},
				content {static_cast<symbol&>(m_label)}};

			return {};
		}};

	timer<timer_options::defer_delivery> m_timer {this, MIN_FUNCTION {
													 m_value = MIN_CLAMP(m_unclipped_value, m_range[0], m_range[1]);
													 redraw();
													 output.send(m_unclipped_value);
													 m_timer.delay(40);
													 return {};
												 }};

	void operator()(sample input) {
		m_unclipped_value = input;
	}

private:
	number m_unclipped_value {0.0};
	number m_value;
	string m_text;
};

MIN_EXTERNAL(min_meter);
