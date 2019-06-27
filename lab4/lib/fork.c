// implement fork from user space

#include <inc/string.h>
#include <inc/lib.h>

// PTE_COW marks copy-on-write page table entries.
// It is one of the bits explicitly allocated to user processes (PTE_AVAIL).
#define PTE_COW		0x800

//
// Custom page fault handler - if faulting page is copy-on-write,
// map in our own private writable copy.
//
static void
pgfault(struct UTrapframe *utf)
{
	void *addr = (void *) utf->utf_fault_va;
	uint32_t err = utf->utf_err;
	int r;

	// Check that the faulting access was (1) a write, and (2) to a
	// copy-on-write page.  If not, panic.
	// Hint:
	//   Use the read-only page table mappings at uvpt
	//   (see <inc/memlayout.h>).

	// LAB 4: Your code here.
	// My code:
	// uintptr_t fva = (uintptr_t) addr;
	// uintptr_t *pde = (uintptr_t *) (UVPT + (UVPT >> 22) << 12 + fva >> 22);
	// uintptr_t *pte = (uintptr_t *) (UVPT + (fva >> 22) << 12 + (fva >> 12) & 0x003ff);
	pte_t pte = uvpt[((uintptr_t) addr) >> PGSHIFT];

	if (!(err & FEC_WR)) {
		panic("pgfault: not a write access");
	}
	if (!(pte & PTE_P)) {
		panic("pgfault: page not present");
	}
	if (!(pte & PTE_COW)) {
		panic("pgfault: not a copy-on-write page");
	}

	// Allocate a new page, map it at a temporary location (PFTEMP),
	// copy the data from the old page to the new page, then move the new
	// page to the old page's address.
	// Hint:
	//   You should make three system calls.

	// LAB 4: Your code here.
	// My code:
	int e = 0;
	addr = (void *) ROUNDDOWN(addr, PGSIZE);
	e = sys_page_alloc(0, (void *) PFTEMP, PTE_P | PTE_U | PTE_W);
	if (e < 0) {
		panic("sys_page_alloc: %e", e);
	}

	memcpy((void *)PFTEMP, addr, PGSIZE);

	e = sys_page_map(0, (void *) PFTEMP, 0, addr, PTE_P | PTE_U | PTE_W);
	if (e < 0) {
		panic("sys_page_map: %e", e);
	}
	e = sys_page_unmap(0, (void *) PFTEMP);
	if (e < 0) {
		panic("sys_page_unmap: %e", e);
	}
}

//
// Map our virtual page pn (address pn*PGSIZE) into the target envid
// at the same virtual address.  If the page is writable or copy-on-write,
// the new mapping must be created copy-on-write, and then our mapping must be
// marked copy-on-write as well.  (Exercise: Why do we need to mark ours
// copy-on-write again if it was already copy-on-write at the beginning of
// this function?)
//
// Returns: 0 on success, < 0 on error.
// It is also OK to panic on error.
//
static int
duppage(envid_t envid, unsigned pn)
{
	int r;

	// LAB 4: Your code here.
	// My code:
	void *addr = (void *) (pn * PGSIZE);
	int perm = uvpt[((uintptr_t) addr) >> PGSHIFT] & 0xfff;
	if (perm & (PTE_W | PTE_COW)) {
		// duppages sets both PTEs so that the page is not writable,
		// and to contain PTE_COW in the "avail" field to distinguish cow pages
		// from read-only pages
		perm &= PTE_SYSCALL;
		perm &= ~PTE_W;
		perm |= PTE_COW;

		// we should map the page cow into the address space of the child first,
		// and then remap the page cow in its own address space
		// (Why? Consider when we are mapping a page that happens to be the stack)
		r = sys_page_map(0, addr, envid, addr, perm);
		if (r < 0)
			panic("sys_page_map: %e", r);
		// Why do we need to mark ours copy-on-write again 
		// if it was already copy-on-write at the beginning of this function?
		// I still don't understand......
		r = sys_page_map(0, addr, 0, addr, perm);
		if (r < 0)
			panic("sys_page_map: %e", r);
	} else {
		perm &= PTE_SYSCALL;
		r = sys_page_map(0, addr, envid, addr, perm);
		if (r < 0)
			panic("sys_page_map: %e", r);
	}

	return 0;
}

//
// User-level fork with copy-on-write.
// Set up our page fault handler appropriately.
// Create a child.
// Copy our address space and page fault handler setup to the child.
// Then mark the child as runnable and return.
//
// Returns: child's envid to the parent, 0 to the child, < 0 on error.
// It is also OK to panic on error.
//
// Hint:
//   Use uvpd, uvpt, and duppage.
//   Remember to fix "thisenv" in the child process.
//   Neither user exception stack should ever be marked copy-on-write,
//   so you must allocate a new page for the child's user exception stack.
//
envid_t
fork(void)
{
	// LAB 4: Your code here.
	// My code:
	envid_t child_id;
	int e = 0;

	// 1. The parent installs pgfault() as the C-level page fault handler
	set_pgfault_handler(pgfault);

	// 2. The parent calls sys_exofork() to create a child enviroment
	child_id = sys_exofork();
	if (child_id == 0) {
		// fix "thisenv" in the child process
		thisenv = &envs[ENVX(sys_getenvid())];
		return child_id;
	} else if (child_id < 0) {
		panic("sys_exofork: %e", child_id);
	}

	// 3. Copy parent's address space to the child
	uintptr_t va = 0;
	for ( ; va < UTOP - PGSIZE; va += PGSIZE) {
		if (!(uvpd[PDX(va)] & PTE_P) || !(uvpt[va >> PGSHIFT] & PTE_P) ||
				!(uvpt[va >> PGSHIFT] & PTE_U))
			continue;
		duppage(child_id, va / PGSIZE);
	}
	// allocate a page in the child for exception stack
	e = sys_page_alloc(child_id, (void *) va, PTE_P | PTE_U | PTE_W);
	if (e < 0)
		panic("sys_page_alloc: %e", e);

	// 4. The parent sets the user page fault entrypoint 
	// for the child to look like its own.
	extern void _pgfault_upcall(void);
	e = sys_env_set_pgfault_upcall(child_id, _pgfault_upcall);
	if (e < 0)
		panic("sys_env_set_pgfault_upcall: %e", e);

	// 5. mark the child runnable
	e = sys_env_set_status(child_id, ENV_RUNNABLE);
	if (e < 0)
		panic("sys_env_set_status: %e", e);

	return child_id;
}

// Challenge!
int
sfork(void)
{
	panic("sfork not implemented");
	return -E_INVAL;
}
