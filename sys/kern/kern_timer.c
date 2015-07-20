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
}

void
timerdev_handler(struct clockframe *frame)
{
	struct cpu_info *ci = curcpu();
	extern struct timerev timerev_prof;
	extern struct timerev timerev_stat;
	extern struct timerev timerev_hard;

	KASSERT(stathz == 0);

	/*
         * Update ticks first, so that all timer handlers can equally
         * see the same, updated value.
	 */
	if (CPU_IS_PRIMARY(ci)) {
		ticks++;
		kern_timer.now = sbinuptime();
		if (kern_timer.prev < kern_timer.now) {
			kern_timer.prev = kern_timer.now;
			kern_timer.next += kern_timer.sbt_1hz;
		}
	}

	(*timerev_prof.te_handler)(&timerev_prof, frame);
	(*timerev_stat.te_handler)(&timerev_stat, frame);
	(*timerev_hard.te_handler)(&timerev_hard, frame);

	/*
	 * Schedule the next tick.
	 */
	(*kern_timer.timerdev->td_start)(kern_timer.timerdev,
	    kern_timer.next, 0);

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
