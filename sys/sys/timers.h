#ifndef _SYS_TIMERS_H_
#define _SYS_TIMERS_H_

#include <sys/time.h>

/*
 * timerdev - Timer device
 */

struct clockframe;
struct timerdev;

struct timerdev {
	void (*td_start)(struct timerdev *, sbintime_t, sbintime_t);
	void (*td_stop)(struct timerdev *);
};

void		timerdev_register(struct timerdev *);
void		timerdev_handler(struct clockframe *);

/*
 * timerev - Timer event
 */

struct timerev {
	void (*te_handler)(struct timerev *, struct clockframe *);
	u_long te_nexttick;
};

/*
 * kern_timer - Global timer state
 */

struct kern_timer {
	struct timerdev *timerdev;
	sbintime_t prev;
	sbintime_t now;
	sbintime_t next;
	sbintime_t sbt_1hz;
};

extern struct kern_timer kern_timer;

void kern_timer_init(void);

#endif /* _SYS_TIMERS_H_ */
