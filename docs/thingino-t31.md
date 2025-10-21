Thingino Ingenic T31 (thingino-t31) machine

Minimal QEMU machine to emulate Ingenic T31 SoC (T31X baseline).

Status
- CPU: MIPS32r2 single core
- RAM: default 128 MiB (T31X). Override with -m N
- UART: UART1 phys 0x10031000 (KSEG1 0xB0031000), ns16550, regshift=2, 24 MHz
- NOR: CFI pflash at phys 0x1FC00000 (reset vector KSEG1 0xBFC00000)
- Interrupts: dummy IRQ (polled UART OK). INTC not modeled yet
- Note: SDK U-Boot usually boots via ROM/SPL to SRAM/DDR; direct pflash boot may hang without a stub

Build
1) cd ../thingino-qemu
2) meson setup build-t31 -Dtarget_list=mipsel-softmmu
3) ninja -C build-t31

Run (pflash 16 MiB)
1) Create 16 MiB image filled with 0xFF (example): use any tool to create a 16MiB file filled with 0xFF
2) Write your image at offset 0: dd if=upstream/u-boot.bin of=/tmp/t31_pflash.bin conv=notrunc status=none
3) Run: ../thingino-qemu/build-t31/qemu-system-mipsel -M thingino-t31 -nographic -serial mon:stdio -drive if=pflash,format=raw,file=/tmp/t31_pflash.bin -m 128 -d guest_errors

UART
- Console on UART1 (ttyS1) at 0x10031000, regshift=2, 24 MHz
- Use -serial mon:stdio for combined monitor+console

Roadmap / TODO
- Interrupt Controller and timer wiring
- Minimal SPL / boot ROM path to DDR (SDK T31X)
- Model SFC SPI NOR controller (8/16 MiB)
- Essential clocks/resets

Notes
- T31L/T31N: 64 MiB; T31X/T31A: 128 MiB (default here is 128 MiB)
- Reset vector 0xBFC00000 maps to phys 0x1FC00000 where pflash is placed
