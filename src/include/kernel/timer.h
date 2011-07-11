#ifndef _TIMER_H_
#define _TIMER_H_

extern volatile unsigned int timer_ticks;

void do_timer(void);
void timer_install(int hz);

#endif /* _TIMER_H_ */
