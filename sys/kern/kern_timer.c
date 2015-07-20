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

struct kern_timer kern_timer;

void
kern_timer_init(void)
{
	kern_timer.sbt_1hz = SBT_1S / hz;
	kern_timer.prev = kern_timer.now = kern_timer.next = sbinuptime();
	kern_timer.next += kern_timer.sbt_1hz;
	kern_timer.nextdiffmin = kern_timer.sbt_1hz * 9 / 10;
	kern_timer.nextdiffmax = kern_timer.sbt_1hz * 11 / 10;
}

extern struct timerev timerev_prof;
extern struct timerev timerev_stat;
extern struct timerev timerev_hard;

void
timerdev_handler(struct clockframe *frame)
{
	struct cpu_info *ci = curcpu();
	sbintime_t nextdiff;

	KASSERT(stathz == 0);

	/*
         * Update ticks first, so that all timer handlers can equally
         * see the same, updated value.
	 */
	if (CPU_IS_PRIMARY(ci)) {
		ticks++;
		kern_timer.now = sbinuptime();
		nextdiff = kern_timer.next - kern_timer.now;

		/*
		 * If timer is getting inaccurate, forcibly reset it.
		 */
		if (nextdiff < kern_timer.nextdiffmin ||
		    nextdiff > kern_timer.nextdiffmax) {
			if (nextdiff < kern_timer.nextdiffmin)
				printf("x");
			if (nextdiff > kern_timer.nextdiffmax)
				printf("X");
			kern_timer.prev = kern_timer.next = kern_timer.now;
			kern_timer.next += kern_timer.sbt_1hz;
			nextdiff = kern_timer.sbt_1hz;
		}
	}

	/*
	 * Handle events.
	 */
	(*timerev_prof.te_handler)(&timerev_prof, frame);
	(*timerev_stat.te_handler)(&timerev_stat, frame);
	(*timerev_hard.te_handler)(&timerev_hard, frame);

	/*
	 * Schedule the next tick.
	 */
	(*kern_timer.timerdev->td_start)(kern_timer.timerdev, nextdiff, 0);

	/*
	 * Update counters for the next iteration.
	 */
	if (CPU_IS_PRIMARY(ci)) {
		kern_timer.prev = kern_timer.next;
		kern_timer.next += kern_timer.sbt_1hz;
	}
}

void
timerdev_register(struct timerdev *td)
{
	kern_timer.timerdev = td;
}
