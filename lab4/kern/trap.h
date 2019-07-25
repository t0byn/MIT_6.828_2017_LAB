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

// IRQs handler
void intr_handler_32();
void intr_handler_33();
void intr_handler_34();
void intr_handler_35();
void intr_handler_36();
void intr_handler_37();
void intr_handler_38();
void intr_handler_39();
void intr_handler_40();
void intr_handler_41();
void intr_handler_42();
void intr_handler_43();
void intr_handler_44();
void intr_handler_45();
void intr_handler_46();
void intr_handler_47();

#endif /* JOS_KERN_TRAP_H */
