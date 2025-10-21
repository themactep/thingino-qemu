# Thingino Ingenic T31 (thingino-t31) machine

Minimal QEMU machine to emulate the Ingenic T31 SoC family for early U-Boot bring-up.

## Status
- CPU: MIPS32r2 single core
- RAM: defaults to 128 MiB (T31X). Override with .
- UART: UART1 at phys 0x10031000 (KSEG1 0xB0031000), ns16550, regshift=2, 24 MHz clock
- Boot ROM / NOR: maps CFI pflash at phys 0x1FC00000 (KSEG1 0xBFC00000)
- Interrupts: UART wired to a dummy IRQ (polled console works; no INTC yet)
- Known issues: very early boot will likely hang with a generic U-Boot binary; a vendor SPL/bootrom normally loads U-Boot proper to SRAM/DDR. We’re iterating toward a minimal SPL flow or a ROM stub.

## Build
Directory already configured.

Just run your build command (e.g. ninja) and Meson will regenerate as necessary.
Run "meson setup --reconfigure" to force Meson to regenerate.

If build failures persist, run "meson setup --wipe" to rebuild from scratch
using the same options as passed when configuring the build.

ERROR: Unknown options: "target_list"
ninja: Entering directory `build-t31'
[1/19] Generating subprojects/dtc/version_gen.h with a custom command
[2/19] Generating qemu-version.h with a custom command (wrapped by meson to capture output)
[3/16] Compiling C object qemu-vmsr-helper.p/tools_i386_qemu-vmsr-helper.c.o
[4/16] Compiling C object qemu-pr-helper.p/scsi_qemu-pr-helper.c.o
[5/16] Compiling C object storage-daemon/qemu-storage-daemon.p/qemu-storage-daemon.c.o
[6/16] Compiling C object qemu-io.p/qemu-io.c.o
[7/16] Linking target qemu-vmsr-helper
[8/16] Linking target qemu-pr-helper
[9/16] Compiling C object qemu-nbd.p/qemu-nbd.c.o
[10/16] Compiling C object qga/qemu-ga.p/main.c.o
[11/16] Linking target storage-daemon/qemu-storage-daemon
[12/16] Linking target qga/qemu-ga
[13/16] Linking target qemu-io
[14/16] Linking target qemu-nbd
[15/16] Compiling C object qemu-img.p/qemu-img.c.o
[16/16] Linking target qemu-img

Check the machine is registered:
thingino-t31         Thingino Ingenic T31 (UART@0x10031000, pflash@0x1FC00000)

## Run (NOR pflash @ 16 MiB)
Create a 16 MiB pflash image filled with 0xFF and place your ROM/U-Boot image at offset 0. Note: for SDK U-Boot, a small ROM stub or SPL may be needed; plain U-Boot may not be link-compatible to execute directly at 0xBFC00000.



### UART
- UART1 console (ttyS1): phys 0x10031000, regshift=2, 24 MHz clock
- Use  for a combined monitor+console on stdio.

## Roadmap / TODO
- Interrupt Controller and timer wiring (eliminate dummy IRQ, periodic timer)
- Minimal SPL / boot ROM loader path to DDR, aligned with SDK T31X
- Model SFC SPI NOR controller so we can boot from SPI NOR like real hardware (8/16 MiB)
- Clocks / resets gating for essential peripherals
- Optional: SD/MMC (MSC), Ethernet/USB as needed

## Notes
- Real hardware: T31L/T31N have 64 MiB; T31X/T31A have 128 MiB. This machine defaults to T31X (128 MiB).
- The reset vector is KSEG1 0xBFC00000, which maps to phys 0x1FC00000 where pflash is placed.
