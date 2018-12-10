/*
 * Copyright (c) 2018, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <debug.h>
#include <smccc.h>
#include <spci_helpers.h>
#include <spci_svc.h>
#include <tftf_lib.h>

/* Returns a SPCI error code. On success, it also returns a 16 bit handle. */
int spci_service_handle_open(uint16_t client_id, uint16_t *handle,
			     uint32_t uuid1, uint32_t uuid2,
			     uint32_t uuid3, uint32_t uuid4)
{
	int32_t ret;

	smc_args get_handle_smc_args = {
		SPCI_SERVICE_HANDLE_OPEN,
		uuid1, uuid2, uuid3, uuid4,
		0, 0, /* Reserved - MBZ */
		client_id
	};

	smc_ret_values smc_ret = tftf_smc(&get_handle_smc_args);

	ret = smc_ret.ret0;
	if (ret != SPCI_SUCCESS)
		return ret;

	uint32_t x1 = smc_ret.ret1;

	if ((x1 & 0x0000FFFF) != 0) {
		tftf_testcase_printf("SPCI_SERVICE_HANDLE_OPEN returned x1 = 0x%08x\n", x1);
		return SPCI_TFTF_ERROR;
	}

	/* The handle is returned in the top 16 bits */
	*handle = (x1 >> 16) & 0xFFFF;

	return SPCI_SUCCESS;
}

/* Invokes SPCI_SERVICE_HANDLE_CLOSE. Returns a SPCI error code. */
int spci_service_handle_close(uint16_t client_id, uint16_t handle)
{
	smc_args close_handle_smc_args = {
		SPCI_SERVICE_HANDLE_CLOSE,
		client_id | (handle << 16)
	};

	smc_ret_values smc_ret = tftf_smc(&close_handle_smc_args);

	return (int32_t)(uint32_t)smc_ret.ret0;
}
