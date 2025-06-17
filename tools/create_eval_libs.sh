#!/bin/bash
set -e

CG_TOOL_ROOT=${HOME}/ti/ti-cgt-armllvm_3.2.2.LTS
CGT_GCC_AARCH64_PATH=${HOME}/ti/gcc-arm-9.2-2019.12-x86_64-aarch64-none-elf

rm -rf ../eval_src/lib
rm -rf ../eval_src/obj
rm -rf ../license_lib/*
rm -rf ../eval_lib/xmrpd-freertos*
rm -rf ../eval_lib/tsn_l2-freertos*
rm -rf ../eval_lib/tsn_lldp-freertos*

make -C ../eval_src/ -f makefile.tsn_l2_freertos.am243x.r5f.ti-arm-clang PROFILE=release -s
make -C ../eval_src/ -f makefile.tsn_l2_freertos.am263x.r5f.ti-arm-clang PROFILE=release -s
make -C ../eval_src/ -f makefile.tsn_l2_freertos.am64x.r5f.ti-arm-clang PROFILE=release -s
make -C ../eval_src/ -f makefile.tsn_l2_freertos.am275x.r5f.ti-arm-clang PROFILE=release -s
make -C ../eval_src/ -f makefile.tsn_lldp_freertos.am243x.r5f.ti-arm-clang PROFILE=release -s
make -C ../eval_src/ -f makefile.tsn_lldp_freertos.am263px.r5f.ti-arm-clang PROFILE=release -s
make -C ../eval_src/ -f makefile.tsn_lldp_freertos.am263x.r5f.ti-arm-clang PROFILE=release -s
make -C ../eval_src/ -f makefile.tsn_lldp_freertos.am273x.r5f.ti-arm-clang PROFILE=release -s
make -C ../eval_src/ -f makefile.tsn_lldp_freertos.am64x.r5f.ti-arm-clang PROFILE=release -s
make -C ../eval_src/ -f makefile.tsn_lldp_freertos.am275x.r5f.ti-arm-clang PROFILE=release -s

make -C ../eval_src/ -f makefile.tsn_l2_freertos.am62ax.r5f.ti-arm-clang  PROFILE=release -s
make -C ../eval_src/ -f makefile.tsn_l2_freertos.am62px.wkup-r5f.ti-arm-clang PROFILE=release -s
make -C ../eval_src/ -f makefile.tsn_l2_freertos.am62dx.r5f.ti-arm-clang PROFILE=release -s
make -C ../eval_src/ -f makefile.tsn_lldp_freertos.am62ax.r5f.ti-arm-clang PROFILE=release -s
make -C ../eval_src/ -f makefile.tsn_lldp_freertos.am62px.wkup-r5f.ti-arm-clang PROFILE=release -s
make -C ../eval_src/ -f makefile.tsn_lldp_freertos.am62dx.r5f.ti-arm-clang PROFILE=release -s
make -C ../eval_src/ -f makefile.xmrpd_freertos.am62ax.r5f.ti-arm-clang PROFILE=release -s
make -C ../eval_src/ -f makefile.xmrpd_freertos.am62dx.r5f.ti-arm-clang PROFILE=release -s
make -C ../eval_src/ -f makefile.xmrpd_freertos.am275x.r5f.ti-arm-clang PROFILE=release -s

# yangemb
make -C ../eval_src/ -f makefile.yangemb_freertos.am243x.r5f.ti-arm-clang PROFILE=release -s
make -C ../eval_src/ -f makefile.yangemb_freertos.am263x.r5f.ti-arm-clang PROFILE=release -s
make -C ../eval_src/ -f makefile.yangemb_freertos.am261x.r5f.ti-arm-clang PROFILE=release -s
make -C ../eval_src/ -f makefile.yangemb_freertos.am263px.r5f.ti-arm-clang PROFILE=release -s
make -C ../eval_src/ -f makefile.yangemb_freertos.am273x.r5f.ti-arm-clang PROFILE=release -s
make -C ../eval_src/ -f makefile.yangemb_freertos.am275x.r5f.ti-arm-clang PROFILE=release -s
make -C ../eval_src/ -f makefile.yangemb_freertos.am64x.r5f.ti-arm-clang PROFILE=release -s
make -C ../eval_src/ -f makefile.yangemb_freertos.am62ax.r5f.ti-arm-clang PROFILE=release -s
make -C ../eval_src/ -f makefile.yangemb_freertos.am62dx.r5f.ti-arm-clang PROFILE=release -s
make -C ../eval_src/ -f makefile.yangemb_freertos.am62px.wkup-r5f.ti-arm-clang PROFILE=release -s
make -C ../eval_src/ -f makefile.yangemb_freertos.am62x.a53.gcc-aarch64 PROFILE=release -s
make -C ../eval_src/ -f makefile.yangemb_freertos.am62lx.a53.gcc-aarch64 PROFILE=release -s
make -C ../eval_src/ -f makefile.yangemb_freertos.am64x.a53.gcc-aarch64 PROFILE=release -s

${CG_TOOL_ROOT}/bin/tiarmobjcopy --strip-unneeded ../eval_src/lib/yangemb-freertos.am243x.r5f.ti-arm-clang.release.lib
${CG_TOOL_ROOT}/bin/tiarmobjcopy --strip-unneeded ../eval_src/lib/yangemb-freertos.am263x.r5f.ti-arm-clang.release.lib
${CG_TOOL_ROOT}/bin/tiarmobjcopy --strip-unneeded ../eval_src/lib/yangemb-freertos.am261x.r5f.ti-arm-clang.release.lib
${CG_TOOL_ROOT}/bin/tiarmobjcopy --strip-unneeded ../eval_src/lib/yangemb-freertos.am263px.r5f.ti-arm-clang.release.lib
${CG_TOOL_ROOT}/bin/tiarmobjcopy --strip-unneeded ../eval_src/lib/yangemb-freertos.am273x.r5f.ti-arm-clang.release.lib
${CG_TOOL_ROOT}/bin/tiarmobjcopy --strip-unneeded ../eval_src/lib/yangemb-freertos.am275x.r5f.ti-arm-clang.release.lib
${CG_TOOL_ROOT}/bin/tiarmobjcopy --strip-unneeded ../eval_src/lib/yangemb-freertos.am64x.r5f.ti-arm-clang.release.lib
${CG_TOOL_ROOT}/bin/tiarmobjcopy --strip-unneeded ../eval_src/lib/yangemb-freertos.am62ax.r5f.ti-arm-clang.release.lib
${CG_TOOL_ROOT}/bin/tiarmobjcopy --strip-unneeded ../eval_src/lib/yangemb-freertos.am62dx.r5f.ti-arm-clang.release.lib
${CG_TOOL_ROOT}/bin/tiarmobjcopy --strip-unneeded ../eval_src/lib/yangemb-freertos.am62px.wkup-r5f.ti-arm-clang.release.lib
${CGT_GCC_AARCH64_PATH}/bin/aarch64-none-elf-objcopy --strip-unneeded ../eval_src/lib/yangemb-freertos.am62x.a53.gcc-aarch64.release.lib
${CGT_GCC_AARCH64_PATH}/bin/aarch64-none-elf-objcopy --strip-unneeded ../eval_src/lib/yangemb-freertos.am62lx.a53.gcc-aarch64.release.lib
${CGT_GCC_AARCH64_PATH}/bin/aarch64-none-elf-objcopy --strip-unneeded ../eval_src/lib/yangemb-freertos.am64x.a53.gcc-aarch64.release.lib

mv ../eval_src/lib/yangemb-freertos.am243x.r5f.ti-arm-clang.release.lib ../eval_src/lib/yangemb-freertos.am243x.r5f.ti-arm-clang.lib 
mv ../eval_src/lib/yangemb-freertos.am263x.r5f.ti-arm-clang.release.lib ../eval_src/lib/yangemb-freertos.am263x.r5f.ti-arm-clang.lib 
mv ../eval_src/lib/yangemb-freertos.am261x.r5f.ti-arm-clang.release.lib ../eval_src/lib/yangemb-freertos.am261x.r5f.ti-arm-clang.lib 
mv ../eval_src/lib/yangemb-freertos.am263px.r5f.ti-arm-clang.release.lib ../eval_src/lib/yangemb-freertos.am263px.r5f.ti-arm-clang.lib 
mv ../eval_src/lib/yangemb-freertos.am273x.r5f.ti-arm-clang.release.lib ../eval_src/lib/yangemb-freertos.am273x.r5f.ti-arm-clang.lib 
mv ../eval_src/lib/yangemb-freertos.am275x.r5f.ti-arm-clang.release.lib ../eval_src/lib/yangemb-freertos.am275x.r5f.ti-arm-clang.lib 
mv ../eval_src/lib/yangemb-freertos.am64x.r5f.ti-arm-clang.release.lib ../eval_src/lib/yangemb-freertos.am64x.r5f.ti-arm-clang.lib 
mv ../eval_src/lib/yangemb-freertos.am62ax.r5f.ti-arm-clang.release.lib ../eval_src/lib/yangemb-freertos.am62ax.r5f.ti-arm-clang.lib
mv ../eval_src/lib/yangemb-freertos.am62dx.r5f.ti-arm-clang.release.lib ../eval_src/lib/yangemb-freertos.am62dx.r5f.ti-arm-clang.lib
mv ../eval_src/lib/yangemb-freertos.am62px.wkup-r5f.ti-arm-clang.release.lib ../eval_src/lib/yangemb-freertos.am62px.wkup-r5f.ti-arm-clang.lib
mv ../eval_src/lib/yangemb-freertos.am62x.a53.gcc-aarch64.release.lib ../eval_src/lib/yangemb-freertos.am62x.a53.gcc-aarch64.lib
mv ../eval_src/lib/yangemb-freertos.am62lx.a53.gcc-aarch64.release.lib ../eval_src/lib/yangemb-freertos.am62lx.a53.gcc-aarch64.lib
mv ../eval_src/lib/yangemb-freertos.am64x.a53.gcc-aarch64.release.lib ../eval_src/lib/yangemb-freertos.am64x.a53.gcc-aarch64.lib

mv ../eval_src/lib/tsn_l2-freertos.am243x.r5f.ti-arm-clang.release.lib ../eval_src/lib/tsn_l2-freertos.am243x.r5f.ti-arm-clang.lib
mv ../eval_src/lib/tsn_l2-freertos.am263x.r5f.ti-arm-clang.release.lib ../eval_src/lib/tsn_l2-freertos.am263x.r5f.ti-arm-clang.lib
mv ../eval_src/lib/tsn_l2-freertos.am275x.r5f.ti-arm-clang.release.lib ../eval_src/lib/tsn_l2-freertos.am275x.r5f.ti-arm-clang.lib
mv ../eval_src/lib/tsn_l2-freertos.am64x.r5f.ti-arm-clang.release.lib ../eval_src/lib/tsn_l2-freertos.am64x.r5f.ti-arm-clang.lib
mv ../eval_src/lib/tsn_l2-freertos.am62ax.r5f.ti-arm-clang.release.lib ../eval_src/lib/tsn_l2-freertos.am62ax.r5f.ti-arm-clang.lib
mv ../eval_src/lib/tsn_l2-freertos.am62px.wkup-r5f.ti-arm-clang.release.lib ../eval_src/lib/tsn_l2-freertos.am62px.wkup-r5f.ti-arm-clang.lib
mv ../eval_src/lib/tsn_l2-freertos.am62dx.r5f.ti-arm-clang.release.lib ../eval_src/lib/tsn_l2-freertos.am62dx.r5f.ti-arm-clang.lib
mv ../eval_src/lib/tsn_lldp-freertos.am243x.r5f.ti-arm-clang.release.lib ../eval_src/lib/tsn_lldp-freertos.am243x.r5f.ti-arm-clang.lib
mv ../eval_src/lib/tsn_lldp-freertos.am263px.r5f.ti-arm-clang.release.lib ../eval_src/lib/tsn_lldp-freertos.am263px.r5f.ti-arm-clang.lib
mv ../eval_src/lib/tsn_lldp-freertos.am263x.r5f.ti-arm-clang.release.lib ../eval_src/lib/tsn_lldp-freertos.am263x.r5f.ti-arm-clang.lib
mv ../eval_src/lib/tsn_lldp-freertos.am273x.r5f.ti-arm-clang.release.lib ../eval_src/lib/tsn_lldp-freertos.am273x.r5f.ti-arm-clang.lib
mv ../eval_src/lib/tsn_lldp-freertos.am275x.r5f.ti-arm-clang.release.lib ../eval_src/lib/tsn_lldp-freertos.am275x.r5f.ti-arm-clang.lib
mv ../eval_src/lib/tsn_lldp-freertos.am64x.r5f.ti-arm-clang.release.lib ../eval_src/lib/tsn_lldp-freertos.am64x.r5f.ti-arm-clang.lib
mv ../eval_src/lib/tsn_lldp-freertos.am62ax.r5f.ti-arm-clang.release.lib ../eval_src/lib/tsn_lldp-freertos.am62ax.r5f.ti-arm-clang.lib
mv ../eval_src/lib/tsn_lldp-freertos.am62px.wkup-r5f.ti-arm-clang.release.lib ../eval_src/lib/tsn_lldp-freertos.am62px.wkup-r5f.ti-arm-clang.lib
mv ../eval_src/lib/tsn_lldp-freertos.am62dx.r5f.ti-arm-clang.release.lib ../eval_src/lib/tsn_lldp-freertos.am62dx.r5f.ti-arm-clang.lib
mv ../eval_src/lib/xmrpd-freertos.am62ax.r5f.ti-arm-clang.release.lib ../eval_src/lib/xmrpd-freertos.am62ax.r5f.ti-arm-clang.lib
mv ../eval_src/lib/xmrpd-freertos.am62dx.r5f.ti-arm-clang.release.lib ../eval_src/lib/xmrpd-freertos.am62dx.r5f.ti-arm-clang.lib
mv ../eval_src/lib/xmrpd-freertos.am275x.r5f.ti-arm-clang.release.lib ../eval_src/lib/xmrpd-freertos.am275x.r5f.ti-arm-clang.lib

cp -f -r ../eval_src/lib/* ../eval_lib/
mv ../eval_lib/yangemb* ../license_lib/
echo "TSN EVALUATION LIBS ARE GENERATED SUCCESSFULLY"
