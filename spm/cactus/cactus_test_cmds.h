/*
 * Copyright (c) 2020, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef CACTUS_TEST_CMDS
#define CACTUS_TEST_CMDS

#include <debug.h>
#include <ffa_helpers.h>

/**
 * Success and error return to be sent over a msg response.
 */
#define CACTUS_SUCCESS	 0
#define CACTUS_ERROR	-1

/**
 * Get command from struct smc_ret_values.
 */
#define CACTUS_GET_CMD(smc_ret) smc_ret.ret3

/**
 * Template for commands to be sent to CACTUS partitions over direct
 * messages interfaces.
 */
#define CACTUS_SEND_CMD(source, dest, cmd, val0, val1, val2, val3)	\
	ffa_msg_send_direct_req64_5args(source, dest, cmd, 		\
					val0, val1, val2, val3)

#define PRINT_CMD(smc_ret)						\
	VERBOSE("cmd %lx; args: %lx, %lx, %lx, %lx\n",	 		\
		smc_ret.ret3, smc_ret.ret4, smc_ret.ret5, 		\
		smc_ret.ret6, smc_ret.ret7)

/**
 * Command to notify cactus of a memory management operation. The cmd value
 * should be the memory management smc function id.
 */
#define CACTUS_MEM_SEND_CMD(source, dest, mem_func, handle) 		\
		CACTUS_SEND_CMD(source, dest, mem_func, handle, 0, 0, 0)

#define CACTUS_MEM_SEND_GET_HANDLE(smc_ret) smc_ret.ret4

/**
 * Template for responses to CACTUS commands.
 */
#define CACTUS_RESPONSE(source, dest, response) 			\
		ffa_msg_send_direct_resp(source, dest, response)

#define CACTUS_SUCCESS_RESP(source, dest) 				\
		CACTUS_RESPONSE(source, dest, CACTUS_SUCCESS)

#define CACTUS_ERROR_RESP(source, dest) 				\
		CACTUS_RESPONSE(source, dest, CACTUS_ERROR)

#define CACTUS_GET_RESPONSE(smc_ret) smc_ret.ret3

#endif
