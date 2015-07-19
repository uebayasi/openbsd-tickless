#include <sys/param.h>
#include <sys/systm.h>
#include <sys/timers.h>

struct timerdev *timerdev;

void
timerdev_handler(struct clockframe *frame)
{
	hardclock(frame);
}

void
timerdev_register(struct timerdev *td)
{
	timerdev = td;
}
