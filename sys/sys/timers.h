/*	$OpenBSD$	*/

/*
 * Copyright (c) 2015 Masao Uebayashi <uebayasi@tombiinc.com>
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

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

void timerdev_register(struct timerdev *);
void timerdev_handler(struct clockframe *);

extern struct timerev timerev_prof;
extern struct timerev timerev_stat;
extern struct timerev timerev_hard;

/*
 * timerev - Timer event
 */

struct timerev {
	void (*te_handler)(struct timerev *, struct clockframe *,
	    sbintime_t *);
	u_long te_nexttick;
};

/*
 * kern_timer - Global timer state
 */

struct kern_timer {
	struct timerdev *timerdev;
	sbintime_t sbt_1hz;
	sbintime_t prev;
	sbintime_t now;
	sbintime_t next;
	sbintime_t nextdiffmin;
	sbintime_t nextdiffmax;
};

extern struct kern_timer kern_timer;

void kern_timer_init(void);

#endif /* _SYS_TIMERS_H_ */
