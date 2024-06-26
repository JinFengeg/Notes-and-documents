cmd_arch/arm/boot/dts/stm32f429-disco.dtb := mkdir -p arch/arm/boot/dts/ ; gcc -E -Wp,-MD,arch/arm/boot/dts/.stm32f429-disco.dtb.d.pre.tmp -nostdinc -I./scripts/dtc/include-prefixes -undef -D__DTS__ -x assembler-with-cpp -o arch/arm/boot/dts/.stm32f429-disco.dtb.dts.tmp arch/arm/boot/dts/stm32f429-disco.dts ; ./scripts/dtc/dtc -O dtb -o arch/arm/boot/dts/stm32f429-disco.dtb -b 0 -iarch/arm/boot/dts/ -i./scripts/dtc/include-prefixes -Wno-unit_address_vs_reg -Wno-unit_address_format -Wno-avoid_unnecessary_addr_size -Wno-alias_paths -Wno-graph_child_address -Wno-simple_bus_reg -Wno-unique_unit_address -Wno-pci_device_reg  -d arch/arm/boot/dts/.stm32f429-disco.dtb.d.dtc.tmp arch/arm/boot/dts/.stm32f429-disco.dtb.dts.tmp ; cat arch/arm/boot/dts/.stm32f429-disco.dtb.d.pre.tmp arch/arm/boot/dts/.stm32f429-disco.dtb.d.dtc.tmp > arch/arm/boot/dts/.stm32f429-disco.dtb.d

source_arch/arm/boot/dts/stm32f429-disco.dtb := arch/arm/boot/dts/stm32f429-disco.dts

deps_arch/arm/boot/dts/stm32f429-disco.dtb := \
  arch/arm/boot/dts/stm32f429.dtsi \
  arch/arm/boot/dts/armv7-m.dtsi \
  scripts/dtc/include-prefixes/dt-bindings/clock/stm32fx-clock.h \
  scripts/dtc/include-prefixes/dt-bindings/mfd/stm32f4-rcc.h \
  arch/arm/boot/dts/stm32f429-pinctrl.dtsi \
  arch/arm/boot/dts/stm32f4-pinctrl.dtsi \
  scripts/dtc/include-prefixes/dt-bindings/pinctrl/stm32-pinfunc.h \
  scripts/dtc/include-prefixes/dt-bindings/input/input.h \
  scripts/dtc/include-prefixes/dt-bindings/input/linux-event-codes.h \

arch/arm/boot/dts/stm32f429-disco.dtb: $(deps_arch/arm/boot/dts/stm32f429-disco.dtb)

$(deps_arch/arm/boot/dts/stm32f429-disco.dtb):
