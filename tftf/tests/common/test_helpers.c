/*
 * Copyright (c) 2020-2023, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <stdlib.h>

#include <arch_helpers.h>
#include <cactus_test_cmds.h>
#include <plat_topology.h>
#include <platform.h>
#include <power_management.h>
#include <test_helpers.h>
#include <tftf_lib.h>

static struct mailbox_buffers test_mb = {.send = NULL, .recv = NULL};

int is_sys_suspend_state_ready(void)
{
	int aff_info;
	unsigned int target_node;
	u_register_t target_mpid;
	u_register_t current_mpid = read_mpidr_el1() & MPID_MASK;

	for_each_cpu(target_node) {
		target_mpid = tftf_get_mpidr_from_node(target_node);

		/* Skip current CPU, as it is powered on */
		if (target_mpid == current_mpid)
			continue;

		aff_info = tftf_psci_affinity_info(target_mpid, MPIDR_AFFLVL0);
		if (aff_info != PSCI_STATE_OFF)
			return 0;
	}

	return 1;
}

void psci_system_reset(void)
{
	smc_args args = { SMC_PSCI_SYSTEM_RESET };
	smc_ret_values ret;

	ret = tftf_smc(&args);

	/* The PSCI SYSTEM_RESET call is not supposed to return */
	tftf_testcase_printf("System didn't reboot properly (%d)\n",
			(unsigned int)ret.ret0);
}

int psci_mem_protect(int val)
{
	smc_args args = { SMC_PSCI_MEM_PROTECT};
	smc_ret_values ret;

	args.arg1 = val;
	ret = tftf_smc(&args);

	return ret.ret0;
}

int psci_mem_protect_check(uintptr_t addr, size_t size)
{
	smc_args args = { SMC_PSCI_MEM_PROTECT_CHECK };
	smc_ret_values ret;

	args.arg1 = addr;
	args.arg2 = size;
	ret = tftf_smc(&args);
	return ret.ret0;
}

/*
 * This function returns an address that can be used as
 * sentinel for mem_protect functions. The logic behind
 * it is that it has to search one region that doesn't intersect
 * with the memory used by TFTF.
 */
unsigned char *psci_mem_prot_get_sentinel(void)
{
	const mem_region_t *ranges, *rp, *lim;
	int nranges;
	IMPORT_SYM(uintptr_t, __TFTF_BASE__, tftf_base);
	IMPORT_SYM(uintptr_t, __TFTF_END__, tftf_end);
	uintptr_t p = 0;

	ranges = plat_get_prot_regions(&nranges);
	if (!ranges)
		return NULL;

	lim = &ranges[nranges];
	for (rp = ranges ; rp < lim; rp++) {
		p = rp->addr;
		if (p < tftf_base || p > tftf_end)
			break;
		p = p + (rp->size - 1);
		if (p < tftf_base || p > tftf_end)
			break;
	}

	return (rp == lim) ? NULL : (unsigned char *) p;
}

/*
 * This function maps the memory region before the
 * test and unmap it after the test is run
 */
test_result_t map_test_unmap(const map_args_unmap_t *args,
			     test_function_arg_t test)
{
	int mmap_ret;
	test_result_t test_ret;

	mmap_ret = mmap_add_dynamic_region(args->addr, args->addr,
					   args->size, args->attr);

	if (mmap_ret != 0) {
		tftf_testcase_printf("Couldn't map memory (ret = %d)\n",
				     mmap_ret);
		return TEST_RESULT_FAIL;
	}

	test_ret = (*test)(args->arg);

	mmap_ret = mmap_remove_dynamic_region(args->addr, args->size);
	if (mmap_ret != 0) {
		tftf_testcase_printf("Couldn't unmap memory (ret = %d)\n",
				     mmap_ret);
		return TEST_RESULT_FAIL;
	}

	return test_ret;
}

bool reset_tftf_mailbox(void)
{
	if (is_ffa_call_error(ffa_rxtx_unmap())) {
		return false;
	}

	test_mb.send = NULL;
	test_mb.recv = NULL;

	return true;
}

bool get_tftf_mailbox(struct mailbox_buffers *mb)
{
	struct ffa_value ret;

	if (test_mb.recv == NULL || test_mb.send == NULL) {
		CONFIGURE_AND_MAP_MAILBOX(test_mb, PAGE_SIZE, ret);
		if (is_ffa_call_error(ret)) {
			return false;
		}
	}

	*mb = test_mb;

	return true;
}

test_result_t check_spmc_testing_set_up(
	uint32_t ffa_version_major, uint32_t ffa_version_minor,
	const struct ffa_uuid *ffa_uuids, size_t ffa_uuids_size)
{
	struct  mailbox_buffers mb;

	if (ffa_uuids == NULL) {
		ERROR("Invalid parameter ffa_uuids!\n");
		return TEST_RESULT_FAIL;
	}

	SKIP_TEST_IF_FFA_VERSION_LESS_THAN(ffa_version_major,
					   ffa_version_minor);

	/**********************************************************************
	 * If OP-TEE is SPMC skip the current test.
	 **********************************************************************/
	if (check_spmc_execution_level()) {
		VERBOSE("OPTEE as SPMC at S-EL1. Skipping test!\n");
		return TEST_RESULT_SKIPPED;
	}

	GET_TFTF_MAILBOX(mb);

	for (unsigned int i = 0U; i < ffa_uuids_size; i++)
		SKIP_TEST_IF_FFA_ENDPOINT_NOT_DEPLOYED(*mb, ffa_uuids[i]);

	return TEST_RESULT_SUCCESS;
}

test_result_t spm_run_multi_core_test(uintptr_t cpu_on_handler,
				      event_t *cpu_done)
{
	unsigned int lead_mpid = read_mpidr_el1() & MPID_MASK;
	unsigned int core_pos, cpu_node, mpidr;
	int32_t ret;

	VERBOSE("Powering on all cpus.\n");

	for (unsigned int i = 0U; i < PLATFORM_CORE_COUNT; i++) {
		tftf_init_event(&cpu_done[i]);
	}

	 /* Power on each secondary CPU one after the other. */
	for_each_cpu(cpu_node) {
		mpidr = tftf_get_mpidr_from_node(cpu_node);
		if (mpidr == lead_mpid) {
			continue;
		}

		ret = tftf_cpu_on(mpidr, cpu_on_handler, 0U);
		if (ret != 0) {
			ERROR("tftf_cpu_on mpidr 0x%x returns %d\n",
			      mpidr, ret);
		}

		/* Wait for the secondary CPU to be ready. */
		core_pos = platform_get_core_pos(mpidr);
		tftf_wait_for_event(&cpu_done[core_pos]);
	}

	VERBOSE("Done exiting.\n");

	return TEST_RESULT_SUCCESS;
}

bool spm_core_sp_init(ffa_id_t sp_id)
{
	/*
	 * Secure Partitions secondary ECs need one round of ffa_run to reach
	 * the message loop.
	 */
	if (sp_id != SP_ID(1)) {
		uint32_t core_pos = get_current_core_id();
		struct ffa_value ret = ffa_run(sp_id, core_pos);

		if (ffa_func_id(ret) != FFA_MSG_WAIT) {
			ERROR("Failed to run SP%x on core %u\n",
			      sp_id, core_pos);
			return false;
		}
	}

	return true;
}

/*
 * Utility function to wait for all CPUs other than the caller to be
 * OFF.
 */
void wait_for_non_lead_cpus(void)
{
	unsigned int target_mpid, target_node;

	for_each_cpu(target_node) {
		target_mpid = tftf_get_mpidr_from_node(target_node);
		wait_for_core_to_turn_off(target_mpid);
	}
}

void wait_for_core_to_turn_off(unsigned int mpidr)
{
	/* Skip lead CPU, as it is powered on */
	if (mpidr == (read_mpidr_el1() & MPID_MASK))
		return;

	while (tftf_psci_affinity_info(mpidr, MPIDR_AFFLVL0) != PSCI_STATE_OFF) {
		continue;
	}
}

/* Generate 64-bit random number */
unsigned long long rand64(void)
{
	return ((unsigned long long)rand() << 32) | rand();
}
