#include <sys/param.h>
#include <sys/systm.h>
#include <sys/kernel.h>
#include <sys/timers.h>

struct kern_timer {
	struct timerdev *timerdev;
} kern_timer;

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
	}

	(*timerev_prof.te_handler)(&timerev_prof, frame);
	(*timerev_stat.te_handler)(&timerev_stat, frame);
	(*timerev_hard.te_handler)(&timerev_hard, frame);
}

void
timerdev_register(struct timerdev *td)
{
	kern_timer.timerdev = td;
}
