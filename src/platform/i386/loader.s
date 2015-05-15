#include <shared/cos_config.h>

.set MULTIBOOT_PAGE_ALIGN,  1<<0
.set MULTIBOOT_MEMINFO,     1<<1
.set MULTIBOOT_AOUT_KLUDGE, 1<<16
.set MULTIBOOT_MAGIC,       0x1BADB002
.set MULTIBOOT_FLAGS,       MULTIBOOT_PAGE_ALIGN | MULTIBOOT_MEMINFO
.set MULTIBOOT_CHECKSUM,    -(MULTIBOOT_MAGIC + MULTIBOOT_FLAGS)

.align 4
.long MULTIBOOT_MAGIC
.long MULTIBOOT_FLAGS
.long MULTIBOOT_CHECKSUM

.set STACKSIZE, 0x4000
.comm stack, STACKSIZE, 32

.globl loader
loader = (_loader-COS_MEM_KERN_VA_SZ)

.globl _loader
_loader:
	mov $(stack + STACKSIZE), %esp
	movl	%eax, %ecx
	movl    %cr4, %eax
	orl     $(1<<4), %eax
	movl    %eax, %cr4
	movl    $(boot_comp_pgd-COS_MEM_KERN_VA_SZ), %eax
	movl    %eax, %cr3
	# Turn on paging.
	movl    %cr0, %eax
	orl     $(1<<31), %eax
	movl    %eax, %cr0
	cli
	pushl %esp
	pushl %ecx
	pushl %ebx
	pushl $0  /* empty return value as we're jmping, not calling */
	mov $kmain, %eax
	jmp *%eax