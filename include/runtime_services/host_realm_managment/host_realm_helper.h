/*
 * Copyright (c) 2022-2023, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#ifndef HOST_REALM_HELPER_H
#define HOST_REALM_HELPER_H

#include <stdlib.h>
#include <host_realm_rmi.h>
#include <tftf_lib.h>

bool host_create_realm_payload(u_register_t realm_payload_adr,
		u_register_t plat_mem_pool_adr,
		u_register_t plat_mem_pool_size,
		u_register_t realm_pages_size,
		u_register_t feature_flag);
bool host_create_shared_mem(
		u_register_t ns_shared_mem_adr,
		u_register_t ns_shared_mem_size);
bool host_destroy_realm(void);
bool host_enter_realm_execute(uint8_t cmd, struct realm **realm_ptr);
test_result_t host_cmp_result(void);

#endif /* HOST_REALM_HELPER_H */
