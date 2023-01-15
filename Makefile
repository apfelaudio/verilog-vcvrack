# If RACK_DIR is not defined when calling the Makefile, default to two directories above
RACK_DIR ?= ../..

VERILATOR_ROOT=/opt/oss-cad-suite/share/verilator

# FLAGS will be passed to both the C and C++ compiler
FLAGS += -I$(VERILATOR_ROOT)/include -faligned-new
CFLAGS +=
CXXFLAGS +=

# Careful about linking to shared libraries, since you can't assume much about the user's environment and library search path.
# Static libraries are fine, but they should be added to this plugin's build system.
LDFLAGS += -Lrtl-verilated -l:Vcore__ALL.a

# verilator --cc rtl/core.sv rtl-verilated -CFLAGS -fPIC
# make -C rtl-verilated

# Add .cpp files to the build
SOURCES += $(wildcard src/*.cpp) $(VERILATOR_ROOT)/include/verilated.cpp

# Add files to the ZIP package when running `make dist`
# The compiled plugin and "plugin.json" are automatically added.
DISTRIBUTABLES += res
DISTRIBUTABLES += $(wildcard LICENSE*)
DISTRIBUTABLES += $(wildcard presets)

# Include the Rack plugin Makefile framework
include $(RACK_DIR)/plugin.mk
