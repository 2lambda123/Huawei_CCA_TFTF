/*
 * Copyright (c) 2018-2020, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <arch_helpers.h>
#include <cactus_test_cmds.h>
#include <debug.h>
#include <ffa_endpoints.h>
#include <platform.h>
#include <smccc.h>
#include <ffa_helpers.h>
#include <ffa_svc.h>
#include <test_helpers.h>

#define ECHO_VAL1 U(0xa0a0a0a0)
#define ECHO_VAL2 U(0xb0b0b0b0)
#define ECHO_VAL3 U(0xc0c0c0c0)

#define DIRECT_MSG_TEST_PATTERN1	(0xaaaa0000)
#define DIRECT_MSG_TEST_PATTERN2	(0xbbbb0000)
#define DIRECT_MSG_TEST_PATTERN3	(0xcccc0000)

static const struct ffa_uuid expected_sp_uuids[] = {
		{PRIMARY_UUID}, {SECONDARY_UUID}, {TERTIARY_UUID}
	};

static test_result_t send_receive_direct_msg(unsigned int sp_id,
					     unsigned int test_pattern)
{
	smc_ret_values ret_values;

	/* Send a message to SP through direct messaging */
	ret_values = ffa_msg_send_direct_req(HYP_ID, sp_id, test_pattern);

	/*
	 * Return responses may be FFA_MSG_SEND_DIRECT_RESP or FFA_INTERRUPT,
	 * but only expect the former. Expect SMC32 convention from SP.
	 */
	if (ret_values.ret0 != FFA_MSG_SEND_DIRECT_RESP_SMC32) {
		tftf_testcase_printf("ffa_msg_send_direct_req returned %lx\n",
				     (u_register_t)ret_values.ret0);
		return TEST_RESULT_FAIL;
	}

	/*
	 * Message loop in SP returns initial message with the running VM id
	 * into the lower 16 bits of initial message.
	 */
	if (ret_values.ret3 != (test_pattern | sp_id)) {
		return TEST_RESULT_FAIL;
	}

	return TEST_RESULT_SUCCESS;
}

test_result_t test_ffa_direct_messaging(void)
{
	test_result_t result;

	/**********************************************************************
	 * Check SPMC has ffa_version and expected FFA endpoints are deployed.
	 **********************************************************************/
	CHECK_HAFNIUM_SPMC_TESTING_SETUP(1, 0, expected_sp_uuids);

	/**********************************************************************
	 * Send a message to SP1 through direct messaging
	 **********************************************************************/
	result = send_receive_direct_msg(SP_ID(1), DIRECT_MSG_TEST_PATTERN1);
	if (result != TEST_RESULT_SUCCESS) {
		return result;
	}

	/**********************************************************************
	 * Send a message to SP2 through direct messaging
	 **********************************************************************/
	result = send_receive_direct_msg(SP_ID(2), DIRECT_MSG_TEST_PATTERN2);
	if (result != TEST_RESULT_SUCCESS) {
		return result;
	}

	/**********************************************************************
	 * Send a message to SP1 through direct messaging
	 **********************************************************************/
	result = send_receive_direct_msg(SP_ID(1), DIRECT_MSG_TEST_PATTERN3);

	return result;
}

/**
 * The 'send_cactus_req_echo_cmd' sends a CACTUS_REQ_ECHO_CMD to a cactus SP.
 * Handling this command, cactus should then send CACTUS_ECHO_CMD to
 * the specified SP according to 'echo_dest'. If the CACTUS_ECHO_CMD is resolved
 * successfully, cactus will reply to tftf with CACTUS_SUCCESS, or CACTUS_ERROR
 * otherwise.
 * For the CACTUS_SUCCESS response, the test returns TEST_RESULT_SUCCESS.
 */
static test_result_t send_cactus_req_echo_cmd(ffa_vm_id_t sender,
					      ffa_vm_id_t dest,
					      ffa_vm_id_t echo_dest,
					      uint64_t value)
{
	smc_ret_values ret;

	ret = CACTUS_REQ_ECHO_SEND_CMD(sender, dest, echo_dest, value);

	if (ret.ret0 != FFA_MSG_SEND_DIRECT_RESP_SMC32) {
		ERROR("Failed to send message. error: %lx\n",
		      ret.ret2);
		return TEST_RESULT_FAIL;
	}

	if (CACTUS_GET_RESPONSE(ret) == CACTUS_ERROR) {
		return TEST_RESULT_FAIL;
	}

	return TEST_RESULT_SUCCESS;
}

test_result_t test_ffa_sp_to_sp_direct_messaging(void)
{
	test_result_t result;

	CHECK_HAFNIUM_SPMC_TESTING_SETUP(1, 0, expected_sp_uuids);

	result = send_cactus_req_echo_cmd(HYP_ID, SP_ID(1), SP_ID(2),
					  ECHO_VAL1);
	if (result != TEST_RESULT_SUCCESS) {
		return result;
	}

	/*
	 * The following the tests are intended to test the handling of a
	 * direct message request with a VM's ID as a the sender.
	 */
	result = send_cactus_req_echo_cmd(HYP_ID + 1, SP_ID(2), SP_ID(3),
					  ECHO_VAL2);
	if (result != TEST_RESULT_SUCCESS) {
		return result;
	}

	result = send_cactus_req_echo_cmd(HYP_ID + 2, SP_ID(3), SP_ID(1),
					  ECHO_VAL3);

	return result;
}

test_result_t test_ffa_sp_to_sp_deadlock(void)
{
	smc_ret_values ret;

	/**********************************************************************
	 * Check SPMC has ffa_version and expected FFA endpoints are deployed.
	 **********************************************************************/
	CHECK_HAFNIUM_SPMC_TESTING_SETUP(1, 0, expected_sp_uuids);

	ret = CACTUS_REQ_DEADLOCK_SEND_CMD(HYP_ID, SP_ID(1), SP_ID(2),
					   SP_ID(3));

	if (ret.ret0 != FFA_MSG_SEND_DIRECT_RESP_SMC32) {
		ERROR("Failed to send message. error: %lx\n",
		      ret.ret2);
		return TEST_RESULT_FAIL;
	}

	if (CACTUS_GET_RESPONSE(ret) == CACTUS_ERROR) {
		ERROR("cactus SP response is CACTUS_ERROR!\n");
		return TEST_RESULT_FAIL;
	}

	return TEST_RESULT_SUCCESS;
}
