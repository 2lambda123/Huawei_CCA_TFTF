/*
 * Copyright (c) 2021, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef SPM_COMMON_H
#define SPM_COMMON_H

#include <ffa_helpers.h>
#include <stdint.h>
#include <string.h>

/* Hypervisor ID at physical FFA instance */
#define HYP_ID          (0)

/* ID for the first Secure Partition. */
#define SPM_VM_ID_FIRST                 SP_ID(1)

/* INTID for the managed exit virtual interrupt. */
#define MANAGED_EXIT_INTERRUPT_ID	U(4)

/** IRQ/FIQ pin used for signaling a virtual interrupt. */
enum interrupt_pin {
	INTERRUPT_TYPE_IRQ,
	INTERRUPT_TYPE_FIQ,
};

/*
 * The bit 15 of the FF-A ID indicates whether the partition is executing
 * in the normal world, in case it is a Virtual Machine (VM); or in the
 * secure world, in case it is a Secure Partition (SP).
 *
 * If bit 15 is set partition is an SP; if bit 15 is clear partition is
 * a VM.
 */
#define SP_ID_MASK	U(1 << 15)
#define SP_ID(x)	((x) | SP_ID_MASK)
#define IS_SP_ID(x)	((x & SP_ID_MASK) != 0U)

struct ffa_features_test {
	const char *test_name;
	unsigned int feature;
	unsigned int expected_ret;
};

struct mailbox_buffers {
	void *recv;
	void *send;
};

#define CONFIGURE_MAILBOX(mb_name, buffers_size) 				\
	do {									\
	/* Declare RX/TX buffers at virtual FF-A instance */			\
	static struct {								\
			uint8_t rx[buffers_size];				\
			uint8_t tx[buffers_size];				\
	} __aligned(PAGE_SIZE) mb_buffers;					\
	mb_name.recv = (void *)mb_buffers.rx;					\
	mb_name.send = (void *)mb_buffers.tx;					\
	} while (false)

#define CONFIGURE_AND_MAP_MAILBOX(mb_name, buffers_size, smc_ret)		\
	do {									\
	CONFIGURE_MAILBOX(mb_name, buffers_size);				\
	smc_ret = ffa_rxtx_map(							\
				(uintptr_t)mb_name.send,			\
				(uintptr_t)mb_name.recv,			\
				buffers_size / PAGE_SIZE			\
			);							\
	} while (false)

/**
 * Helpers to evaluate returns of FF-A calls.
 */
bool is_ffa_call_error(smc_ret_values val);
bool is_ffa_direct_response(smc_ret_values ret);
bool is_expected_ffa_return(smc_ret_values ret, uint32_t func_id);

/*
 * Vector length:
 * SIMD: 128 bits = 16 bytes
 */
#define SIMD_VECTOR_LEN_BYTES		16
#define SIMD_NUM_VECTORS		32
typedef uint8_t simd_vector_t[SIMD_VECTOR_LEN_BYTES];

/*
 * Fills SIMD registers with the content of the container v.
 * Number of vectors is assumed to be SIMD_NUM_VECTORS.
 */
void fill_simd_vector_regs(const simd_vector_t v[SIMD_NUM_VECTORS]);

/*
 * Reads contents of SIMD registers into the provided container v.
 * Number of vectors is assumed to be SIMD_NUM_VECTORS.
 */
void read_simd_vector_regs(simd_vector_t v[SIMD_NUM_VECTORS]);

bool check_spmc_execution_level(void);

unsigned int get_ffa_feature_test_target(const struct ffa_features_test **test_target);

/**
 * Helper to conduct a memory retrieve. This is to be called by the receiver
 * of a memory share operation.
 */
bool memory_retrieve(struct mailbox_buffers *mb,
		     struct ffa_memory_region **retrieved, uint64_t handle,
		     ffa_vm_id_t sender, ffa_vm_id_t receiver,
		     uint32_t mem_func);

/**
 * Helper to conduct a memory relinquish. The caller is usually the receiver,
 * after it being done with the memory shared, identified by the 'handle'.
 */
bool memory_relinquish(struct ffa_mem_relinquish *m, uint64_t handle,
		       ffa_vm_id_t id);

ffa_memory_handle_t memory_send(
	struct ffa_memory_region *memory_region, uint32_t mem_func,
	uint32_t fragment_length, uint32_t total_length);

ffa_memory_handle_t memory_init_and_send(
	struct ffa_memory_region *memory_region, size_t memory_region_max_size,
	ffa_vm_id_t sender, ffa_vm_id_t receiver,
	const struct ffa_memory_region_constituent* constituents,
	uint32_t constituents_count, uint32_t mem_func);

#endif /* SPM_COMMON_H */