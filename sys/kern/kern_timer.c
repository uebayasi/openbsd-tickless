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

#include <sys/param.h>
#include <sys/systm.h>
#include <sys/kernel.h>
#include <sys/timers.h>
#include <sys/stdint.h>

struct kern_timer kern_timer;

struct timerev *timer_events[] = {
	&timerev_prof, &timerev_stat, &timerev_hard
};

void
kern_timer_init(void)
{
	int i;

	kern_timer.now = sbinuptime();
	kern_timer.events = timer_events;
	kern_timer.nevents = nitems(timer_events);

	for (i = 0; i < kern_timer.nevents; i++) {
		struct timerev *te = kern_timer.events[i];
		(*te->te_init)(te);
	}
}

void
timerdev_handler(struct clockframe *frame)
{
	struct cpu_info *ci = curcpu();
	sbintime_t nextdiff;
	int i;

	KASSERT(stathz == 0);

	/*
         * Update ticks first, so that all timer handlers can equally
         * see the same, updated value.
	 */
	if (CPU_IS_PRIMARY(ci)) {
		ticks++;
		kern_timer.now = sbinuptime();
	}

	/*
	 * Handle events.
	 */
	nextdiff = INT64_MAX;
	for (i = 0; i < kern_timer.nevents; i++) {
		struct timerev *te = kern_timer.events[i];
		sbintime_t tmpnextdiff = INT64_MAX;

		(*te->te_handler)(te, frame, &tmpnextdiff);

		if (tmpnextdiff < nextdiff)
			nextdiff = tmpnextdiff;
	}

	/*
	 * Schedule the next tick.
	 */
	if (nextdiff != INT64_MAX)
		(*kern_timer.timerdev->td_start)(kern_timer.timerdev, nextdiff,
		    0);
}

void
timerdev_register(struct timerdev *td)
{
	kern_timer.timerdev = td;
}
