/*
 * Copyright (c) 2020, NVIDIA Corporation. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <arch.h>
#include <utils_def.h>

/*******************************************************************************
 * Platform definitions used by common code
 ******************************************************************************/

#ifndef __PLATFORM_DEF_H__
#define __PLATFORM_DEF_H__

/*******************************************************************************
 * Platform binary types for linking
 ******************************************************************************/
#define PLATFORM_LINKER_FORMAT		"elf64-littleaarch64"
#define PLATFORM_LINKER_ARCH		aarch64

/*******************************************************************************
 * Tegra DRAM memory base address
 ******************************************************************************/
#define DRAM_BASE			U(0x80000000)
#define DRAM_END			U(0xB0000000)
#define DRAM_SIZE			(DRAM_END - DRAM_BASE)

/*******************************************************************************
 * Run-time address of the TFTF image.
 * It has to match the location where the Trusted Firmware-A loads the BL33
 * image.
 ******************************************************************************/
#define TFTF_BASE			0x80080000

/*******************************************************************************
 * Generic platform constants
 ******************************************************************************/

/* Translation table constants */
#define MAX_XLAT_TABLES			20
#define MAX_MMAP_REGIONS		20

/* stack memory available to each CPU */
#define PLATFORM_STACK_SIZE		0x1400
#define PCPU_DV_MEM_STACK_SIZE		0x100

/* total number of system nodes implemented by the platform */
#define PLATFORM_SYSTEM_COUNT		1

/* total number of clusters implemented by the platform */
#define PLATFORM_CLUSTER_COUNT		4
#define PLATFORM_CORES_PER_CLUSTER	2

/* total number of CPUs implemented by the platform across all clusters */
#define PLATFORM_CORE_COUNT		(PLATFORM_CORES_PER_CLUSTER * \
					PLATFORM_CLUSTER_COUNT)

/* total number of nodes in the affinity hierarchy at all affinity levels */
#define PLATFORM_NUM_AFFS		(PLATFORM_SYSTEM_COUNT + \
					PLATFORM_CLUSTER_COUNT + \
					PLATFORM_CORE_COUNT)

/*
 * maximum number of affinity levels in the system that the platform
 * implements
 */
#define PLATFORM_MAX_AFFLVL		MPIDR_AFFLVL2
#define PLAT_MAX_PWR_LEVEL		PLATFORM_MAX_AFFLVL

/*
 * Defines the maximum number of power states at a power domain level for the
 * platform.
 */
#define PLAT_MAX_PWR_STATES_PER_LVL	2

/*
 * Defines the offset of the last Shared Peripheral Interrupt supported by the
 * TF-A Tests on this platform. SPI numbers are mapped onto GIC interrupt IDs,
 * starting from interrupt ID 32. This offset ID corresponds to the last SPI
 * number, to which 32 must be added to get the corresponding last GIC IRQ ID.
 */
#define PLAT_MAX_SPI_OFFSET_ID		415

/* Local state bit width for each level in the state-ID field of power state */
#define PLAT_LOCAL_PSTATE_WIDTH		4

/*
 * We want to run without support for non-volatile memory and hence using a
 * portion of DRAM as workaround.
 * The TFTF binary itself is loaded at 0xA0600000 so we have plenty of free
 * memory at the beginning of the DRAM. Let's use 256MB from the start.
 */
#define TFTF_NVM_OFFSET			0x0FF80000
#define TFTF_NVM_SIZE			0x10000000

/*
 * Times (in ms) used by test code for completion of different events.
 * Suspend entry time for debug build is high due to the time taken
 * by the VERBOSE/INFO prints. The value considers the worst case scenario
 * where all CPUs are going and coming out of suspend continuously.
 */
#define PLAT_SUSPEND_ENTRY_TIME		500
#define PLAT_SUSPEND_ENTRY_EXIT_TIME	1000

/*******************************************************************************
 * Non-Secure Software Generated Interupts IDs
 ******************************************************************************/
#define IRQ_NS_SGI_0			0
#define IRQ_NS_SGI_1			1
#define IRQ_NS_SGI_2			2
#define IRQ_NS_SGI_3			3
#define IRQ_NS_SGI_4			4
#define IRQ_NS_SGI_5			5
#define IRQ_NS_SGI_6			6
#define IRQ_NS_SGI_7			7

/*******************************************************************************
 * Platform specific page table and MMU setup constants
 ******************************************************************************/
#define PLAT_PHY_ADDR_SPACE_SIZE	(ULL(1) << 40)
#define PLAT_VIRT_ADDR_SPACE_SIZE	(ULL(1) << 40)

/*******************************************************************************
 * Used to align variables on the biggest cache line size in the platform.
 * This is known only to the platform as it might have a combination of
 * integrated and external caches.
 ******************************************************************************/
#define CACHE_WRITEBACK_SHIFT		6
#define CACHE_WRITEBACK_GRANULE		(1 << CACHE_WRITEBACK_SHIFT)

/*******************************************************************************
 * Platform console related constants
 ******************************************************************************/
#define TEGRA194_CONSOLE_BAUDRATE	U(115200)
#define TEGRA194_CONSOLE_CLKRATE	U(408000000)

/*******************************************************************************
 * Platform MMIO devices
 ******************************************************************************/
#define TEGRA194_GICD_BASE		U(0x03881000)
#define TEGRA194_GICC_BASE		U(0x03882000)
#define TEGRA194_SPE_BASE		U(0x0C168000)
#define TEGRA194_UARTC_BASE		U(0x0C280000)
#define TEGRA194_RTC_BASE		U(0x0C2A0000)
#define TEGRA194_TMRUS_BASE		U(0x0C2E0000)
#define SYS_CNT_BASE1			TEGRA194_TMRUS_BASE

#endif /* __PLATFORM_DEF_H__ */
