#
# Copyright (c) 2020, NVIDIA Corporation. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#

TESTS_SOURCES	+=	$(addprefix tftf/tests/plat/nvidia/common/,		\
	test_sip.c								\
)

TESTS_SOURCES	+=	$(addprefix tftf/tests/plat/nvidia/tegra194/,		\
	test_ras_corrected.c							\
	test_ras_uncorrectable.c						\
	serror_handler.S							\
)
