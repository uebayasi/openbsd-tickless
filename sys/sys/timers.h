#ifndef _SYS_TIMERS_H_
#define _SYS_TIMERS_H_

/*
 * timerdev
 */

struct clockframe;
struct timerdev;

struct timerdev {
	void (*td_start)(struct timerdev *, u_long, u_long);
	void (*td_stop)(struct timerdev *);
};

void		timerdev_register(struct timerdev *);
void		timerdev_handler(struct clockframe *);

#endif /* _SYS_TIMERS_H_ */
