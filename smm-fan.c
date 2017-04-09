/*
 * smm-fan.c -- Utility to enable / disable SMM BIOS fan control on Dell laptops for X86_64
 *
 * Based on:
 *  - dell-smm-hwmon.c -- Linux driver for accessing the SMM BIOS on Dell laptops
 *  - smm.c            -- Utility to test SMM BIOS calls on Inspiron 8000 laptops from i8kutils
 *
 * Use at your own risk.
 */

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

#define DISABLE_BIOS_FAN_CONTROL 0x30a3
#define ENABLE_BIOS_FAN_CONTROL  0x31a3

const char usage_string[] =
	"state\n\tset SMM BIOS fan control state = { 0, 1 }; (0: disabled, 1: enabled)";

const char illegal_argument_string[] =
	"expected 1 parameter defining state";

const char invalid_access_string[] =
	"only root can do that";

typedef struct {
	unsigned int eax;
	unsigned int ebx __attribute__ ((packed));
	unsigned int ecx __attribute__ ((packed));
	unsigned int edx __attribute__ ((packed));
	unsigned int esi __attribute__ ((packed));
	unsigned int edi __attribute__ ((packed));
} SMMRegisters;

usage(char* argv) {
	printf("usage: %s %s\n\n", argv, usage_string);
}

illegal_argument(char* argv) {
	printf("%s: %s\n\n", argv, illegal_argument_string);
	usage(argv);
	exit(EINVAL);
}

invalid_access(char* argv) {
	printf("%s: %s\n\n", argv, invalid_access_string);
	usage(argv);
	exit(EACCES);
}

static int i8k_smm(SMMRegisters *regs) {
	int rc;
	int eax = regs->eax;

	asm volatile("pushq %%rax\n\t"
		"movl 0(%%rax),%%edx\n\t"
		"pushq %%rdx\n\t"
		"movl 4(%%rax),%%ebx\n\t"
		"movl 8(%%rax),%%ecx\n\t"
		"movl 12(%%rax),%%edx\n\t"
		"movl 16(%%rax),%%esi\n\t"
		"movl 20(%%rax),%%edi\n\t"
		"popq %%rax\n\t"
		"out %%al,$0xb2\n\t"
		"out %%al,$0x84\n\t"
		"xchgq %%rax,(%%rsp)\n\t"
		"movl %%ebx,4(%%rax)\n\t"
		"movl %%ecx,8(%%rax)\n\t"
		"movl %%edx,12(%%rax)\n\t"
		"movl %%esi,16(%%rax)\n\t"
		"movl %%edi,20(%%rax)\n\t"
		"popq %%rdx\n\t"
		"movl %%edx,0(%%rax)\n\t"
		"pushfq\n\t"
		"popq %%rax\n\t"
		"andl $1,%%eax\n"
		: "=a"(rc)
		: "a"(regs)
		: "%ebx", "%ecx", "%edx", "%esi", "%edi", "memory");

	if (rc != 0 || (regs->eax & 0xffff) == 0xffff || regs->eax == eax) {
		rc = -EINVAL;
		printf("error");
	}

	return rc;
}

static int i8k_smm_send_command(unsigned int command) {
	SMMRegisters regs = { .eax = command };
	return i8k_smm(&regs);
}

int main(int argc, char **argv) {
	unsigned int state;
	unsigned int command;

	if (geteuid() != 0) {
		invalid_access(argv[0]);
	}

	if (argc != 2) {
		illegal_argument(argv[0]);
	}

	ioperm(0x84, 1, 1);
	ioperm(0xb2, 1, 1);

	state = atoi(argv[1]);
	if (state == 0) {
		command = ENABLE_BIOS_FAN_CONTROL;
	} else if (state == 1) {
		command = DISABLE_BIOS_FAN_CONTROL;
	} else {
		illegal_argument(argv[0]);
	}
	return i8k_smm_send_command(command);
}

