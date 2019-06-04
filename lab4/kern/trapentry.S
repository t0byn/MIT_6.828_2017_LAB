/* See COPYRIGHT for copyright information. */

#include <inc/mmu.h>
#include <inc/memlayout.h>
#include <inc/trap.h>

#include <kern/picirq.h>


###################################################################
# exceptions/interrupts
###################################################################

/* TRAPHANDLER defines a globally-visible function for handling a trap.
 * It pushes a trap number onto the stack, then jumps to _alltraps.
 * Use TRAPHANDLER for traps where the CPU automatically pushes an error code.
 *
 * You shouldn't call a TRAPHANDLER function from C, but you may
 * need to _declare_ one in C (for instance, to get a function pointer
 * during IDT setup).  You can declare the function with
 *   void NAME();
 * where NAME is the argument passed to TRAPHANDLER.
 */
#define TRAPHANDLER(name, num)						\
	.globl name;		/* define global symbol for 'name' */	\
	.type name, @function;	/* symbol type is function */		\
	.align 2;		/* align function definition */		\
	name:			/* function starts here */		\
	pushl $(num);							\
	jmp _alltraps

/* Use TRAPHANDLER_NOEC for traps where the CPU doesn't push an error code.
 * It pushes a 0 in place of the error code, so the trap frame has the same
 * format in either case.
 */
#define TRAPHANDLER_NOEC(name, num)					\
	.globl name;							\
	.type name, @function;						\
	.align 2;							\
	name:								\
	pushl $0;							\
	pushl $(num);							\
	jmp _alltraps

.text

/*
 * Lab 3: Your code here for generating entry points for the different traps.
 */
# My code:
TRAPHANDLER_NOEC(intr_handler_0, 0);     # 0   T_DIVIDE
TRAPHANDLER_NOEC(intr_handler_1, 1);     # 1   T_DEBUG
TRAPHANDLER_NOEC(intr_handler_2, 2);     # 2   T_NMI
TRAPHANDLER_NOEC(intr_handler_3, 3);     # 3   T_BRKPT
TRAPHANDLER_NOEC(intr_handler_4, 4);     # 4   T_OFLOW
TRAPHANDLER_NOEC(intr_handler_5, 5);     # 5   T_BOUND
TRAPHANDLER_NOEC(intr_handler_6, 6);     # 6   T_ILLOP
TRAPHANDLER_NOEC(intr_handler_7, 7);     # 7   T_DEVICE
TRAPHANDLER(intr_handler_8, 8);          # 8   T_DBLFLT
TRAPHANDLER(intr_handler_10, 10);        # 10  T_TSS
TRAPHANDLER(intr_handler_11, 11);        # 11  T_SEGNP
TRAPHANDLER(intr_handler_12, 12);        # 12  T_STACK
TRAPHANDLER(intr_handler_13, 13);        # 13  T_GPFLT
TRAPHANDLER(intr_handler_14, 14);        # 14  T_PGFLT
TRAPHANDLER_NOEC(intr_handler_16, 16);   # 16  T_FPERR
TRAPHANDLER(intr_handler_17, 17);        # 17  T_ALIGN
TRAPHANDLER_NOEC(intr_handler_18, 18);   # 18  T_MCHK
TRAPHANDLER_NOEC(intr_handler_19, 19);   # 19  T_SIMDERR

TRAPHANDLER_NOEC(intr_handler_48, 48);   # 48  T_SYSCALL


/*
 * Lab 3: Your code here for _alltraps
 */
# My code:
_alltraps:
  # build trap frame
  pushl %ds;
  pushl %es;
  pushal;

  # set up data segments
  #  *ss, cs already changed
  #  *kernel never use fs and gs
  movw $GD_KD, %ax;
  movw %ax, %ds;
  movw %ax, %es;

  # call trap(tf), where tp = %esp
  pushl %esp;
  call trap;

  # looks like trap doesn't return

