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

#endif /* _SYS_TIMERS_H_ */
