#include <limits.h>

#include "plugin.hpp"

// Verilator must run and build the verilated RTL before this
// source file is compiled. The Makefile handles this.
#include "../build/rtl-verilated/Vcore.h"

struct Eurorack_pmod : Module {
	enum ParamId {
		PARAMS_LEN
	};
	enum InputId {
		IN0_INPUT,
		IN1_INPUT,
		IN2_INPUT,
		IN3_INPUT,
		INPUTS_LEN
	};
	enum OutputId {
		OUT0_OUTPUT,
		OUT1_OUTPUT,
		OUT2_OUTPUT,
		OUT3_OUTPUT,
		OUTPUTS_LEN
	};
	enum LightId {
		ENUMS(LED_IN0_LIGHT, 2),
		ENUMS(LED_IN1_LIGHT, 2),
		ENUMS(LED_IN2_LIGHT, 2),
		ENUMS(LED_IN3_LIGHT, 2),
		ENUMS(LED_OUT0_LIGHT, 2),
		ENUMS(LED_OUT1_LIGHT, 2),
		ENUMS(LED_OUT2_LIGHT, 2),
		ENUMS(LED_OUT3_LIGHT, 2),
		LIGHTS_LEN
	};

    Vcore *tb;

	Eurorack_pmod() {
		config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);
		configInput(IN0_INPUT, "");
		configInput(IN1_INPUT, "");
		configInput(IN2_INPUT, "");
		configInput(IN3_INPUT, "");
		configOutput(OUT1_OUTPUT, "");
		configOutput(OUT2_OUTPUT, "");
		configOutput(OUT3_OUTPUT, "");
		configOutput(OUT0_OUTPUT, "");

        tb = new Vcore();
	}

    /* The eurorack-pmod gateware expects 16-bit signed values representing
     * millivolts in fixed-point 14.2 layout for samples coming in/out. */
    int16_t volt_to_fp16(float v, uint8_t fractional_bits = 2,
                         int16_t clamp_hi = SHRT_MAX, int16_t clamp_lo = SHRT_MIN) {
        v *= 1e3 * (float)(1 << fractional_bits);
        if (v >= clamp_hi) return clamp_hi;
        if (v <= clamp_lo) return clamp_lo;
        return uint16_t(v);
    }

    float fp16_to_volt(int16_t b, uint8_t fractional_bits = 2) {
        float v = (float)b;
        v /= 1e3 * (float)(1 << fractional_bits);
        return v;
    }

    float volt_to_led_green(float f) {
        float brightness = f / 5.0;
        brightness = ((brightness > 1.0) ? 1.0 : brightness);
        brightness = ((brightness < 0.0) ? 0.0 : brightness);
        return brightness;
    }

    float volt_to_led_red(float f) {
        float brightness = -f / 5.0;
        brightness = ((brightness > 1.0) ? 1.0 : brightness);
        brightness = ((brightness < 0.0) ? 0.0 : brightness);
        return brightness;
    }

	void process(const ProcessArgs& args) override {
        tb->sample_clk = 0;
        tb->eval();

        int in_ix = 0;
        for (auto* in : {&tb->sample_in0, &tb->sample_in1,
                         &tb->sample_in2, &tb->sample_in3}) {
            *in = volt_to_fp16(inputs[IN0_INPUT+in_ix].getVoltage());
            lights[LED_IN0_LIGHT + (in_ix*2) + 0].setBrightness(
                    volt_to_led_green(inputs[IN0_INPUT+in_ix].getVoltage()));
            lights[LED_IN0_LIGHT + (in_ix*2) + 1].setBrightness(
                    volt_to_led_red(inputs[IN0_INPUT+in_ix].getVoltage()));
            ++in_ix;
        }

        int out_ix = 0;
        for (auto* out : {&tb->sample_out0, &tb->sample_out1,
                          &tb->sample_out2, &tb->sample_out3}) {
            outputs[OUT0_OUTPUT+out_ix].setVoltage(fp16_to_volt(*out));
            lights[LED_OUT0_LIGHT + (out_ix*2) + 0].setBrightness(
                    volt_to_led_green(outputs[OUT0_OUTPUT+out_ix].getVoltage()));
            lights[LED_OUT0_LIGHT + (out_ix*2) + 1].setBrightness(
                    volt_to_led_red(outputs[OUT0_OUTPUT+out_ix].getVoltage()));
            ++out_ix;
        }

        tb->sample_clk = 1;
        tb->eval();
	}
};

RectangleLight<GreenRedLight> * createRectLight(Eurorack_pmod* module, float pos_x, float pos_y, int light) {
    auto l = createLight<RectangleLight<GreenRedLight>>(mm2px(Vec(pos_x, pos_y)), module, light);
    l->box.size = mm2px(Vec(8.0, 1.0));
    return l;
}

struct Eurorack_pmodWidget : ModuleWidget {
	Eurorack_pmodWidget(Eurorack_pmod* module) {
		setModule(module);
		setPanel(createPanel(asset::plugin(pluginInstance, "res/eurorack-pmod-panel.svg")));

		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH/4, 0)));
		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH/4, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(9.7, 15.0)), module, Eurorack_pmod::IN0_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(9.7, 28.8)), module, Eurorack_pmod::IN1_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(9.7, 42.5)), module, Eurorack_pmod::IN2_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(9.7, 56.3)), module, Eurorack_pmod::IN3_INPUT));

		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(9.7, 70.0)), module, Eurorack_pmod::OUT0_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(9.7, 83.8)), module, Eurorack_pmod::OUT1_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(9.7, 97.4)), module, Eurorack_pmod::OUT2_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(9.7, 111.3)), module, Eurorack_pmod::OUT3_OUTPUT));

		addChild(createRectLight(module, 5.7, 20.9+13.8*0, Eurorack_pmod::LED_IN0_LIGHT));
		addChild(createRectLight(module, 5.7, 20.9+13.8*1, Eurorack_pmod::LED_IN1_LIGHT));
		addChild(createRectLight(module, 5.7, 20.9+13.8*2, Eurorack_pmod::LED_IN2_LIGHT));
		addChild(createRectLight(module, 5.7, 20.9+13.8*3, Eurorack_pmod::LED_IN3_LIGHT));
		addChild(createRectLight(module, 5.7, 20.9+13.8*4, Eurorack_pmod::LED_OUT0_LIGHT));
		addChild(createRectLight(module, 5.7, 20.9+13.8*5, Eurorack_pmod::LED_OUT1_LIGHT));
		addChild(createRectLight(module, 5.7, 20.9+13.8*6, Eurorack_pmod::LED_OUT2_LIGHT));
		addChild(createRectLight(module, 5.7, 20.9+13.8*7, Eurorack_pmod::LED_OUT3_LIGHT));
	}
};


Model* modelEurorack_pmod = createModel<Eurorack_pmod, Eurorack_pmodWidget>("eurorack-pmod");
