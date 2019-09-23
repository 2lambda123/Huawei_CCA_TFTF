#
# Copyright (c) 2018, Arm Limited. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#

TESTS_SOURCES	+=							\
	$(addprefix tftf/tests/runtime_services/standard_service/psci/api_tests/, \
		psci_stat/test_psci_stat.c				\
		system_off/test_system_off.c 				\
	)

ifeq (${USE_NVM},0)
$(error Manual tests require USE_NVM=1 to persist test results across reboots)
endif

ifeq (${NEW_TEST_SESSION},1)
$(error Manual tests require NEW_TEST_SESSION=0 to persist test results across reboots)
endif
