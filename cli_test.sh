#!/bin/bash

if [ "$(./build/cpu32 run 0 data/bin/program00.bin)" = $'8421\nahoj!\ncpu status: HALTED' ]; then
    echo "program00.bin passed."
else
    echo "program00.bin failed."
fi

if [ "$(./build/cpu32 run 16 data/bin/program01.bin)" = $'2137cpu status: INVALID_STACK_OPERATION' ]; then
    echo "program01.bin passed."
else
    echo "program01.bin failed."
fi
