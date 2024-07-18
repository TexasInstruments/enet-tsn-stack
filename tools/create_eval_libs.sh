#!/bin/bash
set -e

rm -rf ../eval_src/lib
rm -rf ../eval_src/obj
rm -rf ../eval_lib/*
make -C ../eval_src/ -f makefile.tsn_l2_freertos.am243x.r5f.ti-arm-clang PROFILE=release -s
make -C ../eval_src/ -f makefile.tsn_l2_freertos.am263x.r5f.ti-arm-clang PROFILE=release -s
make -C ../eval_src/ -f makefile.tsn_l2_freertos.am64x.r5f.ti-arm-clang PROFILE=release -s
make -C ../eval_src/ -f makefile.tsn_lldp_freertos.am243x.r5f.ti-arm-clang PROFILE=release -s
make -C ../eval_src/ -f makefile.tsn_lldp_freertos.am263px.r5f.ti-arm-clang PROFILE=release -s
make -C ../eval_src/ -f makefile.tsn_lldp_freertos.am263x.r5f.ti-arm-clang PROFILE=release -s
make -C ../eval_src/ -f makefile.tsn_lldp_freertos.am273x.r5f.ti-arm-clang PROFILE=release -s
make -C ../eval_src/ -f makefile.tsn_lldp_freertos.am64x.r5f.ti-arm-clang PROFILE=release -s

make -C ../eval_src/ -f makefile.tsn_l2_freertos.am62ax.r5f.ti-arm-clang  PROFILE=release -s
make -C ../eval_src/ -f makefile.tsn_l2_freertos.am62px.wkup-r5f.ti-arm-clang PROFILE=release -s
make -C ../eval_src/ -f makefile.tsn_l2_freertos.am62dx.r5f.ti-arm-clang PROFILE=release -s
make -C ../eval_src/ -f makefile.tsn_lldp_freertos.am62ax.r5f.ti-arm-clang PROFILE=release -s
make -C ../eval_src/ -f makefile.tsn_lldp_freertos.am62px.wkup-r5f.ti-arm-clang PROFILE=release -s
make -C ../eval_src/ -f makefile.tsn_lldp_freertos.am62dx.r5f.ti-arm-clang PROFILE=release -s

mv ../eval_src/lib/tsn_l2-freertos.am243x.r5f.ti-arm-clang.release.lib ../eval_src/lib/tsn_l2-freertos.am243x.r5f.ti-arm-clang.lib
mv ../eval_src/lib/tsn_l2-freertos.am263x.r5f.ti-arm-clang.release.lib ../eval_src/lib/tsn_l2-freertos.am263x.r5f.ti-arm-clang.lib
mv ../eval_src/lib/tsn_l2-freertos.am64x.r5f.ti-arm-clang.release.lib ../eval_src/lib/tsn_l2-freertos.am64x.r5f.ti-arm-clang.lib
mv ../eval_src/lib/tsn_l2-freertos.am62ax.r5f.ti-arm-clang.release.lib ../eval_src/lib/tsn_l2-freertos.am62ax.r5f.ti-arm-clang.lib
mv ../eval_src/lib/tsn_l2-freertos.am62px.wkup-r5f.ti-arm-clang.release.lib ../eval_src/lib/tsn_l2-freertos.am62px.wkup-r5f.ti-arm-clang.lib
mv ../eval_src/lib/tsn_l2-freertos.am62dx.r5f.ti-arm-clang.release.lib ../eval_src/lib/tsn_l2-freertos.am62dx.r5f.ti-arm-clang.lib
mv ../eval_src/lib/tsn_lldp-freertos.am243x.r5f.ti-arm-clang.release.lib ../eval_src/lib/tsn_lldp-freertos.am243x.r5f.ti-arm-clang.lib
mv ../eval_src/lib/tsn_lldp-freertos.am263px.r5f.ti-arm-clang.release.lib ../eval_src/lib/tsn_lldp-freertos.am263px.r5f.ti-arm-clang.lib
mv ../eval_src/lib/tsn_lldp-freertos.am263x.r5f.ti-arm-clang.release.lib ../eval_src/lib/tsn_lldp-freertos.am263x.r5f.ti-arm-clang.lib
mv ../eval_src/lib/tsn_lldp-freertos.am273x.r5f.ti-arm-clang.release.lib ../eval_src/lib/tsn_lldp-freertos.am273x.r5f.ti-arm-clang.lib
mv ../eval_src/lib/tsn_lldp-freertos.am64x.r5f.ti-arm-clang.release.lib ../eval_src/lib/tsn_lldp-freertos.am64x.r5f.ti-arm-clang.lib
mv ../eval_src/lib/tsn_lldp-freertos.am62ax.r5f.ti-arm-clang.release.lib ../eval_src/lib/tsn_lldp-freertos.am62ax.r5f.ti-arm-clang.lib
mv ../eval_src/lib/tsn_lldp-freertos.am62px.wkup-r5f.ti-arm-clang.release.lib ../eval_src/lib/tsn_lldp-freertos.am62px.wkup-r5f.ti-arm-clang.lib
mv ../eval_src/lib/tsn_lldp-freertos.am62dx.r5f.ti-arm-clang.release.lib ../eval_src/lib/tsn_lldp-freertos.am62dx.r5f.ti-arm-clang.lib

cp -f -r ../eval_src/lib/* ../eval_lib/

echo "TSN EVALUATION LIBS ARE GENERATED SUCCESSFULLY"
