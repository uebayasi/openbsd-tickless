/*
 * Copyright (c) 2016 Masao Uebayashi <uebayasi@tombiinc.com>
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
#include <sys/stdint.h>

#include <machine/cpu.h>
#include <machine/cpufunc.h>

void tsc_delay_init(void);
void tsc_delay(int);
uint64_t tsc2usec(uint64_t n);

int tsc_delay_initialized;

void
tsc_delay_init(void)
{
#ifdef DIAGNOSTIC
	struct cpu_info *ci = curcpu();

	KASSERT((ci->ci_flags & CPUF_CONST_TSC) != 0);
	KASSERT(ci->ci_tsc_freq != 0);
#endif

	tsc_delay_initialized = 1;
}

void
tsc_delay(int usec)
{
	struct cpu_info *ci = curcpu();
	int64_t n;
	uint64_t now, prev;

	KASSERT(tsc_delay_initialized == 1);

	n = ci->ci_tsc_freq / 1000000 * usec;
	prev = rdtsc();
	while (n > 0) {
		CPU_BUSY_CYCLE();
		now = rdtsc();
		if (now < prev)
			n -= UINT64_MAX - (prev - now);
		else
			n -= now - prev;
		prev = now;
	}
}

uint64_t
tsc2usec(uint64_t n)
{
	struct cpu_info *ci = curcpu();

	return n / ci->ci_tsc_freq * 1000000;
}
