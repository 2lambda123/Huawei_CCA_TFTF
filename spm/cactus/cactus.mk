#
# Copyright (c) 2018, Arm Limited. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#

CACTUS_INCLUDES :=					\
	-Iinclude					\
	-Iinclude/common				\
	-Iinclude/common/${ARCH}			\
	-Iinclude/drivers				\
	-Iinclude/drivers/arm				\
	-Iinclude/lib					\
	-Iinclude/lib/${ARCH}				\
	-Iinclude/lib/stdlib				\
	-Iinclude/lib/stdlib/sys			\
	-Iinclude/lib/utils				\
	-Iinclude/lib/xlat_tables			\
	-Iinclude/runtime_services			\
	-Iinclude/runtime_services/secure_el0_payloads	\
	-Ispm/cactus					\
	-Ispm/common					\

CACTUS_SOURCES	:=					\
	$(addprefix spm/cactus/,			\
		aarch64/cactus_entrypoint.S		\
		cactus_main.c				\
		cactus_service_loop.c			\
		cactus_tests_memory_attributes.c	\
		cactus_tests_misc.c			\
		cactus_tests_system_setup.c		\
	)						\
	$(addprefix spm/common/,			\
		aarch64/sp_arch_helpers.S		\
		sp_helpers.c				\
	)						\

STDLIB_SOURCES	:=	$(addprefix lib/stdlib/,	\
	assert.c					\
	mem.c						\
	putchar.c					\
	printf.c					\
	rand.c						\
	strlen.c					\
	subr_prf.c					\
)

# TODO: Remove dependency on TFTF files.
CACTUS_SOURCES	+=					\
	tftf/framework/debug.c				\
	tftf/framework/${ARCH}/asm_debug.S

CACTUS_SOURCES	+= 	drivers/arm/pl011/${ARCH}/pl011_console.S	\
			lib/${ARCH}/cache_helpers.S			\
			lib/${ARCH}/misc_helpers.S			\
			plat/common/${ARCH}/platform_helpers.S		\
			${STDLIB_SOURCES}

CACTUS_LINKERFILE	:=	spm/cactus/cactus.ld.S

CACTUS_DEFINES	:=

$(eval $(call add_define,CACTUS_DEFINES,DEBUG))
$(eval $(call add_define,CACTUS_DEFINES,ENABLE_ASSERTIONS))
$(eval $(call add_define,CACTUS_DEFINES,LOG_LEVEL))
$(eval $(call add_define,CACTUS_DEFINES,PLAT_${PLAT}))
ifeq (${ARCH},aarch32)
        $(eval $(call add_define,CACTUS_DEFINES,AARCH32))
else
        $(eval $(call add_define,CACTUS_DEFINES,AARCH64))
endif

cactus: ${AUTOGEN_DIR}/tests_list.h
