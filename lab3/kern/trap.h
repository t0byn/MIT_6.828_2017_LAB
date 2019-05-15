/* See COPYRIGHT for copyright information. */

#ifndef JOS_KERN_TRAP_H
#define JOS_KERN_TRAP_H
#ifndef JOS_KERNEL
# error "This is a JOS kernel header; user programs should not #include it"
#endif

#include <inc/trap.h>
#include <inc/mmu.h>

/* The kernel's interrupt descriptor table */
extern struct Gatedesc idt[];
extern struct Pseudodesc idt_pd;

void trap_init(void);
void trap_init_percpu(void);
void print_regs(struct PushRegs *regs);
void print_trapframe(struct Trapframe *tf);
void page_fault_handler(struct Trapframe *);
void backtrace(struct Trapframe *);

/* interrupt handler */
void intr_handler_0();
void intr_handler_1();
void intr_handler_2();
void intr_handler_3();
void intr_handler_4();
void intr_handler_5();
void intr_handler_6();
void intr_handler_7();
void intr_handler_8();
void intr_handler_10();
void intr_handler_11();
void intr_handler_12();
void intr_handler_13();
void intr_handler_14();
void intr_handler_16();
void intr_handler_17();
void intr_handler_18();
void intr_handler_19();

// system call interrupt handler
void intr_handler_48();

#endif /* JOS_KERN_TRAP_H */
