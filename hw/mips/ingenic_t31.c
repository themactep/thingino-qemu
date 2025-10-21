/* SPDX-License-Identifier: GPL-2.0-or-later */
#include "qemu/osdep.h"
#include "qapi/error.h"
#include "hw/boards.h"
#include "hw/char/serial-mm.h"
#include "hw/sysbus.h"
#include "target/mips/cpu.h"
#include "qemu/units.h"
#include "system/address-spaces.h"

#define T31_UART0_PHYS 0x10030000u /* vendor UART0_BASE 0xB0030000 (KSEG1) */
#define T31_RAM_BASE 0x00000000u
#define T31_RAM_SIZE (256 * MiB)

static void t31_init(MachineState *machine)
{
    MIPSCPU *cpu;
    Error *err = NULL;

    cpu = MIPS_CPU(object_new(TYPE_MIPS_CPU));
    object_property_set_str(OBJECT(cpu), "cpu-model", "24Kf", &err);
    if (err) {
        error_report_err(err);
        exit(1);
    }
    qdev_realize(DEVICE(cpu), NULL, &err);
    if (err) {
        error_report_err(err);
        exit(1);
    }

    memory_region_init_ram(&machine->ram, NULL, "t31.ram",
                           machine->ram_size ? machine->ram_size : T31_RAM_SIZE,
                           &err);
    if (err) {
        error_report_err(err);
        exit(1);
    }
    memory_region_add_subregion(get_system_memory(), T31_RAM_BASE, &machine->ram);

    /* NS16550 compatible UART at T31 UART0 */
    serial_mm_init(get_system_memory(), T31_UART0_PHYS, 0, 115200,
                   serial_hd(0), DEVICE_NATIVE_ENDIAN);
}

static void t31_machine_class_init(ObjectClass *oc, void *data)
{
    MachineClass *mc = MACHINE_CLASS(oc);
    mc->desc = "Thingino Ingenic T31 (UART@0x10030000)";
    mc->init = t31_init;
    mc->default_cpu_type = TYPE_MIPS_CPU;
    mc->default_ram_id = "t31.ram";
    mc->default_ram_size = T31_RAM_SIZE;
}

static const TypeInfo t31_machine_types[] = {
    {
        .name          = MACHINE_TYPE_NAME("thingino-t31"),
        .parent        = TYPE_MACHINE,
        .class_init    = t31_machine_class_init,
    },
};

DEFINE_TYPES(t31_machine_types)

