#ifndef _ISR_H_
#define _ISR_H_

#define VALID_ISR (32)
#define INTERRUPT_COUNT (1)

void default_isr(void);

extern void (*isr_table[(VALID_ISR + INTERRUPT_COUNT) << 1])(void);

#endif /* _ISR_H_ */
