#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <assert.h>
#include <errno.h>
#include <string.h>

#include "include/shared/cos_types.h"
#include <captbl.h>
#include <pgtbl.h>
#include <cap_ops.h>
//#include <liveness_tbl.h>
#include <thread.h>
#include <component.h>
#include <inv.h>

u8_t boot_comp_captbl[PAGE_SIZE] PAGE_ALIGNED;
u8_t boot_comp_pgd[PAGE_SIZE]    PAGE_ALIGNED;
u8_t boot_comp_pte_vm[PAGE_SIZE] PAGE_ALIGNED;
u8_t boot_comp_pte_pm[PAGE_SIZE] PAGE_ALIGNED;

unsigned long sys_maxmem      = 1<<10; /* 4M of physical memory (2^10 pages) */
unsigned long sys_llbooter_sz = 10;    /* how many pages is the llbooter? */

void
kern_boot_comp(void)
{
	struct captbl *ct;
	pgtbl_t pt;
	int ret;

	ct = captbl_create(boot_comp_captbl);
	assert(ct);
	pt = pgtbl_create(boot_comp_pgd);
	pgtbl_init_pte(boot_comp_pte_vm);
	pgtbl_init_pte(boot_comp_pte_pm);

	/* 
	 * Initial captbl setup:  
	 * 0 = sret, 
	 * 1 = this captbl, 
	 * 2 = our pgtbl root,
	 * 3 = empty
	 * 4-5 = our component,
	 * 6-7 = empty
	 * 8 = vm pte for booter
	 * 9 = vm pte for physical memory
	 * 
	 * Initial pgtbl setup:
	 * 
	 */
	assert(!captbl_activate_boot(ct, 1));
	assert(!sret_activate(ct, 1, 0));
	assert(!pgtbl_activate(ct, 1, 2, pt, 0));

	assert(!pgtbl_activate(ct, 1, 8, (pgtbl_t)boot_comp_pte_vm, 1));
	assert(!pgtbl_activate(ct, 1, 9, (pgtbl_t)boot_comp_pte_pm, 1));
	ret = comp_activate(ct, 1, 4, 1, 2, 0, 0x37337, NULL);
	assert(!ret);

	/* construct the page tables */

	/* ... */
}

void 
kern_main(void)
{
	cap_init();
	ltbl_init();
	comp_init();
	thd_init();
	inv_init();
	kern_boot_comp();
}

int main(void) { kern_main(); return 0; }