#include "plugin.hpp"

// Verilator must run and build the verilated RTL before this
// source file is compiletd
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
		OUT1_OUTPUT,
		OUT2_OUTPUT,
		OUT3_OUTPUT,
		OUT0_OUTPUT,
		OUTPUTS_LEN
	};
	enum LightId {
		LED_IN0_LIGHT,
		LED_IN1_LIGHT,
		LED_IN2_LIGHT,
		LED_IN3_LIGHT,
		LED_OUT0_LIGHT,
		LED_OUT1_LIGHT,
		LED_OUT2_LIGHT,
		LED_OUT3_LIGHT,
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

    int16_t volt_to_fpga(float v) {
        v *= 4e3;
        if (v >  32e3) v = 32e3;
        if (v < -32e3) v = -32e3;
        return uint16_t(v);
    }

    float fpga_to_volt(int16_t b) {
        float v = (float)b;
        v /= 4e3;
        return v;
    }

	void process(const ProcessArgs& args) override {
        tb->sample_clk = 0;
        tb->eval();
        tb->sample_in0 = volt_to_fpga(inputs[IN0_INPUT].getVoltage());
        tb->sample_in1 = volt_to_fpga(inputs[IN1_INPUT].getVoltage());
        tb->sample_in2 = volt_to_fpga(inputs[IN2_INPUT].getVoltage());
        tb->sample_in3 = volt_to_fpga(inputs[IN3_INPUT].getVoltage());
        outputs[OUT0_OUTPUT].setVoltage(fpga_to_volt(tb->sample_out0));
        outputs[OUT1_OUTPUT].setVoltage(fpga_to_volt(tb->sample_out1));
        outputs[OUT2_OUTPUT].setVoltage(fpga_to_volt(tb->sample_out2));
        outputs[OUT3_OUTPUT].setVoltage(fpga_to_volt(tb->sample_out3));
        tb->sample_clk = 1;
        tb->eval();
	}
};


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

		addChild(createLight<MediumLight<RedLight>>(mm2px(Vec(9.7, 21.45)), module, Eurorack_pmod::LED_IN0_LIGHT));
		addChild(createLight<MediumLight<RedLight>>(mm2px(Vec(9.7, 35.27)), module, Eurorack_pmod::LED_IN1_LIGHT));
		addChild(createLight<MediumLight<RedLight>>(mm2px(Vec(9.7, 48.65)), module, Eurorack_pmod::LED_IN2_LIGHT));
		addChild(createLight<MediumLight<RedLight>>(mm2px(Vec(9.7, 62.50)), module, Eurorack_pmod::LED_IN3_LIGHT));
		addChild(createLight<MediumLight<RedLight>>(mm2px(Vec(9.7, 76.53)), module, Eurorack_pmod::LED_OUT0_LIGHT));
		addChild(createLight<MediumLight<RedLight>>(mm2px(Vec(9.7, 90.35)), module, Eurorack_pmod::LED_OUT1_LIGHT));
		addChild(createLight<MediumLight<RedLight>>(mm2px(Vec(9.7, 103.77)), module, Eurorack_pmod::LED_OUT2_LIGHT));
		addChild(createLight<MediumLight<RedLight>>(mm2px(Vec(9.7, 117.65)), module, Eurorack_pmod::LED_OUT3_LIGHT));
	}
};


Model* modelEurorack_pmod = createModel<Eurorack_pmod, Eurorack_pmodWidget>("eurorack-pmod");
