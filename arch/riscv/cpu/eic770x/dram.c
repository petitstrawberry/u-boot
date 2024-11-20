// SPDX-License-Identifier: GPL-2.0-only
/*
 *
 * Copyright 2024 Beijing ESWIN Computing Technology Co., Ltd.
 *  Author: Xiang Xu <xuxiang@eswincomputing.com>
 */

#include <efi_loader.h>
#include <fdtdec.h>
#include <init.h>
#include <mapmem.h>
#include <asm/global_data.h>
#include <linux/sizes.h>
DECLARE_GLOBAL_DATA_PTR;

DECLARE_GLOBAL_DATA_PTR;

/* 8 GB */
#define DDR_SIZE_MAX    	0x200000000

/* 128 MB offset */
#define RAM_BASE_OFFSET 	0x8000000

int dram_init(void)
{
	int ret = fdtdec_setup_mem_size_base();
	unsigned long base_with_offset = (gd->ram_base + RAM_BASE_OFFSET);
	gd->ram_size = get_ram_size((long *)base_with_offset, DDR_SIZE_MAX);
	return ret;
}

int dram_init_banksize(void)
{
	return fdtdec_setup_memory_banksize();
}

phys_addr_t board_get_usable_ram_top(phys_size_t total_size)
{
#ifdef CONFIG_64BIT
	/*
	 * Ensure that we run from first 4GB so that all
	 * addresses used by U-Boot are 32bit addresses.
	 *
	 * This in-turn ensures that 32bit DMA capable
	 * devices work fine because DMA mapping APIs will
	 * provide 32bit DMA addresses only.
	 */
	if (gd->ram_top > SZ_4G)
		return SZ_4G;
#endif
	return gd->ram_top;
}

#if CONFIG_IS_ENABLED(EFI_LOADER)
/**
 * efi_add_known_memory() - add memory banks to map
 *
 */
void efi_add_known_memory(void)
{
	int i;
	/* Add RAM */
	for (i = 0; i < CONFIG_NR_DRAM_BANKS; i++) {
		u64 ram_end, ram_start, ram_top;

		ram_start = (uintptr_t)gd->ram_base;
		ram_end = ram_start + gd->ram_size;
		ram_top = ram_end;
		efi_add_conventional_memory_map(ram_start, ram_end, ram_top);
	}
}
#endif
