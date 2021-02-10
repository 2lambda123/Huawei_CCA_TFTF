#
# Copyright (c) 2018-2021, Arm Limited. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#

TESTS_SOURCES	+=							\
	$(addprefix tftf/tests/runtime_services/secure_service/,	\
		ffa_helpers.c						\
		spm_common.c						\
		test_ffa_direct_messaging.c				\
		test_ffa_features.c					\
		test_ffa_memory_sharing.c				\
		test_ffa_rxtx_map.c					\
		test_ffa_version.c					\
	)
