/* SPDX-License-Identifier: GPL-2.0-or-later */
#include "qemu/osdep.h"
#include "qapi/error.h"
#include "qemu/units.h"

#include "hw/boards.h"
#include "hw/irq.h"
#include "hw/char/serial-mm.h"
#include "hw/block/flash.h"
#include "system/address-spaces.h"
#include "system/system.h"
#include "qom/object.h"

#include "target/mips/cpu.h"
#include "hw/qdev-clock.h"

#define T31_UART0_PHYS 0x10030000u /* vendor UART0_BASE 0xB0030000 (KSEG1) */
#define T31_RAM_BASE   0x00000000u
#define T31_RESET_ADDR 0x1FC00000u /* maps to KSEG1 0xBFC00000 */
#define T31_BIOS_SIZE  (16 * MiB)

static void t31_init(MachineState *machine)
{
    MemoryRegion *sysmem = get_system_memory();
    Clock *cpuclk;
    MIPSCPU *cpu __attribute__((unused));

    /* CPU clock ~400 MHz (arbitrary; adjust later if desired) */
    cpuclk = clock_new(OBJECT(machine), "cpu-refclk");
    clock_set_hz(cpuclk, 400000000);

    /* Create a MIPS32r2 CPU (default from mc->default_cpu_type) */
    cpu = mips_cpu_create_with_clock(machine->cpu_type, cpuclk, false);

    cpu_mips_irq_init_cpu(cpu);
    cpu_mips_clock_init(cpu);

    /* Map main RAM at 0x0 */
    memory_region_add_subregion(sysmem, T31_RAM_BASE, machine->ram);

    /* UART0 at Ingenic T31 address; use regshift=2 (32-bit spaced regs). */
    MIPSCPU *pcpu = MIPS_CPU(first_cpu);
    CPUMIPSState *env = &pcpu->env;
    qemu_irq uirq = env->irq[4];
    serial_mm_init(sysmem, T31_UART0_PHYS, /*regshift*/2, uirq,
                   115200, serial_hd(0), DEVICE_NATIVE_ENDIAN);

    /* Optional: register a pflash (if provided via -drive if=pflash) at reset addr */
    {
        DriveInfo *dinfo = drive_get(IF_PFLASH, 0, 0);
        if (dinfo) {
            PFlashCFI01 *fl = pflash_cfi01_register(T31_RESET_ADDR,
                                                    "thingino_t31.bios",
                                                    T31_BIOS_SIZE,
                                                    blk_by_legacy_dinfo(dinfo),
                                                    64 * KiB,
                                                    4,  /* width */
                                                    0x0000, 0x0000, 0x0000, 0x0000,
                                                    0);
            (void)fl;
        }
    }
}

static void t31_machine_class_init(ObjectClass *oc, const void *data)
{
    MachineClass *mc = MACHINE_CLASS(oc);
    mc->desc = "Thingino Ingenic T31 (UART@0x10030000, pflash@0x1FC00000)";
    mc->init = t31_init;
    mc->default_cpu_type = MIPS_CPU_TYPE_NAME("24Kf");
    mc->default_ram_id = "t31.ram";
    mc->default_ram_size = 128 * MiB;
}

static const TypeInfo t31_machine_types[] = {
    {
        .name       = MACHINE_TYPE_NAME("thingino-t31"),
        .parent     = TYPE_MACHINE,
        .class_init = t31_machine_class_init,
    },
};

DEFINE_TYPES(t31_machine_types)
