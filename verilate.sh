#!/bin/bash
verilator --cc rtl/core.sv -Mdir rtl-verilated -CFLAGS -fPIC
make -C rtl-verilated -f Vcore.mk
