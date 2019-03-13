/* 
 * A 16 bit graphics line routine, by Johan Klockars.
 *
 * This file is an example of how to write an
 * fVDI device driver routine in C.
 *
 * You are encouraged to use this file as a starting point
 * for other accelerated features, or even for supporting
 * other graphics modes. This file is therefore put in the
 * public domain. It's not copyrighted or under any sort
 * of license.
 */

#include "fvdi.h"
#include "../bitplane/bitplane.h"


extern long CDECL clip_line(Virtual *vwk, long *x1, long *y1, long *x2, long *y2);

/*
 * Make it as easy as possible for the C compiler.
 * The current code is written to produce reasonable results with Lattice C.
 * (long integers, optimize: [x xx] time)
 * - One function for each operation -> more free registers
 * - 'int' is the default type
 * - some compilers aren't very smart when it comes to *, / and %
 * - some compilers can't deal well with *var++ constructs
 */

#ifdef BOTH
static void s_line_replace(short *addr, short *addr_fast, int count,
                      int d, int incrE, int incrNE, int one_step, int both_step,
                      short foreground, short background)
{
#ifdef BOTH
	*addr_fast = foreground;
#endif
	*addr = foreground;
	(void) background;

	for(--count; count >= 0; count--) {
		if (d < 0) {
			d += incrE;
#ifdef BOTH
			addr_fast += one_step;
#endif
			addr += one_step;
		} else {
			d += incrNE;
#ifdef BOTH
			addr_fast += both_step;
#endif
			addr += both_step;
		}
#ifdef BOTH
		*addr_fast = foreground;
#endif
		*addr = foreground;
	}
}

static void s_line_replace_p(short *addr, short *addr_fast, long pattern, int count,
                        int d, int incrE, int incrNE, int one_step, int both_step,
                        short foreground, short background)
{
	unsigned int mask = 0x8000;

	if (pattern & mask) {
#ifdef BOTH
		*addr_fast = foreground;
#endif
		*addr = foreground;
	} else {
#ifdef BOTH
		*addr_fast = background;
#endif
		*addr = background;
	}
		
	for(--count; count >= 0; count--) {
		if (d < 0) {
			d += incrE;
#ifdef BOTH
			addr_fast += one_step;
#endif
			addr += one_step;
		} else {
			d += incrNE;
#ifdef BOTH
			addr_fast += both_step;
#endif
			addr += both_step;
		}

		if (!(mask >>= 1))
			mask = 0x8000;

		if (pattern & mask) {
#ifdef BOTH
			*addr_fast = foreground;
#endif
			*addr = foreground;
		} else {
#ifdef BOTH
			*addr_fast = background;
#endif
			*addr = background;
		}
	}
}

static void s_line_transparent(short *addr, short *addr_fast, int count,
                          int d, int incrE, int incrNE, int one_step, int both_step,
                          short foreground, short background)
{
#ifdef BOTH
	*addr_fast = foreground;
#endif
	*addr = foreground;
	(void) background;

	for(--count; count >= 0; count--) {
		if (d < 0) {
			d += incrE;
#ifdef BOTH
			addr_fast += one_step;
#endif
			addr += one_step;
		} else {
			d += incrNE;
#ifdef BOTH
			addr_fast += both_step;
#endif
			addr += both_step;
		}
#ifdef BOTH
		*addr_fast = foreground;
#endif
		*addr = foreground;
	}
}

static void s_line_transparent_p(short *addr, short *addr_fast, long pattern, int count,
                            int d, int incrE, int incrNE, int one_step, int both_step,
                            short foreground, short background)
{
	unsigned int mask = 0x8000;

	(void) foreground;
	(void) background;
	if (pattern & mask) {
#ifdef BOTH
		*addr_fast = foreground;
#endif
		*addr = foreground;
	}
		
	for(--count; count >= 0; count--) {
		if (d < 0) {
			d += incrE;
#ifdef BOTH
			addr_fast += one_step;
#endif
			addr += one_step;
		} else {
			d += incrNE;
#ifdef BOTH
			addr_fast += both_step;
#endif
			addr += both_step;
		}

		if (!(mask >>= 1))
			mask = 0x8000;

		if (pattern & mask) {
#ifdef BOTH
			*addr_fast = foreground;
#endif
			*addr = foreground;
		}
	}
}

static void s_line_xor(short *addr, short *addr_fast, int count,
                  int d, int incrE, int incrNE, int one_step, int both_step,
                  short foreground, short background)
{
	int v;

	(void) foreground;
	(void) background;
#ifdef BOTH
	v = ~*addr_fast;
#else
	v = ~*addr;
#endif
#ifdef BOTH
	*addr_fast = v;
#endif
	*addr = v;

	for(--count; count >= 0; count--) {
		if (d < 0) {
			d += incrE;
#ifdef BOTH
			addr_fast += one_step;
#endif
			addr += one_step;
		} else {
			d += incrNE;
#ifdef BOTH
			addr_fast += both_step;
#endif
			addr += both_step;
		}
#ifdef BOTH
		v = ~*addr_fast;
#else
		v = ~*addr;
#endif
#ifdef BOTH
		*addr_fast = v;
#endif
		*addr = v;
	}
}

static void s_line_xor_p(short *addr, short *addr_fast, long pattern, int count,
                    int d, int incrE, int incrNE, int one_step, int both_step,
                    short foreground, short background)
{
	int v;
	unsigned int mask = 0x8000;

	(void) foreground;
	(void) background;
	if (pattern & mask) {
#ifdef BOTH
		v = ~*addr_fast;
#else
		v = ~*addr;
#endif
#ifdef BOTH
		*addr_fast = v;
#endif
		*addr = v;
	}
		
	for(--count; count >= 0; count--) {
		if (d < 0) {
			d += incrE;
#ifdef BOTH
			addr_fast += one_step;
#endif
			addr += one_step;
		} else {
			d += incrNE;
#ifdef BOTH
			addr_fast += both_step;
#endif
			addr += both_step;
		}

		if (!(mask >>= 1))
			mask = 0x8000;

		if (pattern & mask) {
#ifdef BOTH
			v = ~*addr_fast;
#else
			v = ~*addr;
#endif
#ifdef BOTH
			*addr_fast = v;
#endif
			*addr = v;
		}
	}
}

static void s_line_revtransp(short *addr, short *addr_fast, int count,
                        int d, int incrE, int incrNE, int one_step, int both_step,
                        short foreground, short background)
{
#ifdef BOTH
	*addr_fast = foreground;
#endif
	*addr = foreground;
	(void) addr_fast;
	(void) background;

	for(--count; count >= 0; count--) {
		if (d < 0) {
			d += incrE;
#ifdef BOTH
			addr_fast += one_step;
#endif
			addr += one_step;
		} else {
			d += incrNE;
#ifdef BOTH
			addr_fast += both_step;
#endif
			addr += both_step;
		}
#ifdef BOTH
		*addr_fast = foreground;
#endif
		*addr = foreground;
	}
}

static void s_line_revtransp_p(short *addr, short *addr_fast, long pattern, int count,
                          int d, int incrE, int incrNE, int one_step, int both_step,
                          short foreground, short background)
{
	unsigned int mask = 0x8000;

	(void) background;
	if (!(pattern & mask)) {
#ifdef BOTH
		*addr_fast = foreground;
#endif
		*addr = foreground;
	}
		
	for(--count; count >= 0; count--) {
		if (d < 0) {
			d += incrE;
#ifdef BOTH
			addr_fast += one_step;
#endif
			addr += one_step;
		} else {
			d += incrNE;
#ifdef BOTH
			addr_fast += both_step;
#endif
			addr += both_step;
		}

		if (!(mask >>= 1))
			mask = 0x8000;

		if (!(pattern & mask)) {
#ifdef BOTH
			*addr_fast = foreground;
#endif
			*addr = foreground;
		}
	}
}


#define BOTH_WAS_ON
#endif
#undef BOTH

/*
 * The functions below are exact copies of those above.
 * The '#undef BOTH' makes sure that this works as it should
 * when no shadow buffer is available
 */

static void line_replace(short *addr, short *addr_fast, int count,
                    int d, int incrE, int incrNE, int one_step, int both_step,
                    short foreground, short background)
{
#ifdef BOTH
	*addr_fast = foreground;
#endif
	*addr = foreground;
	(void) addr_fast;
	(void) background;

	for(--count; count >= 0; count--) {
		if (d < 0) {
			d += incrE;
#ifdef BOTH
			addr_fast += one_step;
#endif
			addr += one_step;
		} else {
			d += incrNE;
#ifdef BOTH
			addr_fast += both_step;
#endif
			addr += both_step;
		}
#ifdef BOTH
		*addr_fast = foreground;
#endif
		*addr = foreground;
	}
}

static void line_replace_p(short *addr, short *addr_fast, long pattern, int count,
                      int d, int incrE, int incrNE, int one_step, int both_step,
                      short foreground, short background)
{
	unsigned int mask = 0x8000;

	(void) addr_fast;
	if (pattern & mask) {
#ifdef BOTH
		*addr_fast = foreground;
#endif
		*addr = foreground;
	} else {
#ifdef BOTH
		*addr_fast = background;
#endif
		*addr = background;
	}
		
	for(--count; count >= 0; count--) {
		if (d < 0) {
			d += incrE;
#ifdef BOTH
			addr_fast += one_step;
#endif
			addr += one_step;
		} else {
			d += incrNE;
#ifdef BOTH
			addr_fast += both_step;
#endif
			addr += both_step;
		}

		if (!(mask >>= 1))
			mask = 0x8000;

		if (pattern & mask) {
#ifdef BOTH
			*addr_fast = foreground;
#endif
			*addr = foreground;
		} else {
#ifdef BOTH
			*addr_fast = background;
#endif
			*addr = background;
		}
	}
}

static void line_transparent(short *addr, short *addr_fast, int count,
                        int d, int incrE, int incrNE, int one_step, int both_step,
                        short foreground, short background)
{
#ifdef BOTH
	*addr_fast = foreground;
#endif
	*addr = foreground;
	(void) addr_fast;
	(void) background;

	for(--count; count >= 0; count--) {
		if (d < 0) {
			d += incrE;
#ifdef BOTH
			addr_fast += one_step;
#endif
			addr += one_step;
		} else {
			d += incrNE;
#ifdef BOTH
			addr_fast += both_step;
#endif
			addr += both_step;
		}
#ifdef BOTH
		*addr_fast = foreground;
#endif
		*addr = foreground;
	}
}

static void line_transparent_p(short *addr, short *addr_fast, long pattern, int count,
                          int d, int incrE, int incrNE, int one_step, int both_step,
                          short foreground, short background)
{
	unsigned int mask = 0x8000;

	(void) addr_fast;
	(void) background;
	if (pattern & mask) {
#ifdef BOTH
		*addr_fast = foreground;
#endif
		*addr = foreground;
	}
		
	for(--count; count >= 0; count--) {
		if (d < 0) {
			d += incrE;
#ifdef BOTH
			addr_fast += one_step;
#endif
			addr += one_step;
		} else {
			d += incrNE;
#ifdef BOTH
			addr_fast += both_step;
#endif
			addr += both_step;
		}

		if (!(mask >>= 1))
			mask = 0x8000;

		if (pattern & mask) {
#ifdef BOTH
			*addr_fast = foreground;
#endif
			*addr = foreground;
		}
	}
}

static void line_xor(short *addr, short *addr_fast, int count,
                int d, int incrE, int incrNE, int one_step, int both_step,
                short foreground, short background)
{
	int v;

	(void) addr_fast;
	(void) foreground;
	(void) background;
#ifdef BOTH
	v = ~*addr_fast;
#else
	v = ~*addr;
#endif
#ifdef BOTH
	*addr_fast = v;
#endif
	*addr = v;

	for(--count; count >= 0; count--) {
		if (d < 0) {
			d += incrE;
#ifdef BOTH
			addr_fast += one_step;
#endif
			addr += one_step;
		} else {
			d += incrNE;
#ifdef BOTH
			addr_fast += both_step;
#endif
			addr += both_step;
		}
#ifdef BOTH
		v = ~*addr_fast;
#else
		v = ~*addr;
#endif
#ifdef BOTH
		*addr_fast = v;
#endif
		*addr = v;
	}
}

static void line_xor_p(short *addr, short *addr_fast, long pattern, int count,
                  int d, int incrE, int incrNE, int one_step, int both_step,
                  short foreground, short background)
{
	int v;
	unsigned int mask = 0x8000;

	(void) addr_fast;
	(void) foreground;
	(void) background;
	if (pattern & mask) {
#ifdef BOTH
		v = ~*addr_fast;
#else
		v = ~*addr;
#endif
#ifdef BOTH
		*addr_fast = v;
#endif
		*addr = v;
	}
		
	for(--count; count >= 0; count--) {
		if (d < 0) {
			d += incrE;
#ifdef BOTH
			addr_fast += one_step;
#endif
			addr += one_step;
		} else {
			d += incrNE;
#ifdef BOTH
			addr_fast += both_step;
#endif
			addr += both_step;
		}

		if (!(mask >>= 1))
			mask = 0x8000;

		if (pattern & mask) {
#ifdef BOTH
			v = ~*addr_fast;
#else
			v = ~*addr;
#endif
#ifdef BOTH
			*addr_fast = v;
#endif
			*addr = v;
		}
	}
}

static void line_revtransp(short *addr, short *addr_fast, int count,
                      int d, int incrE, int incrNE, int one_step, int both_step,
                      short foreground, short background)
{
#ifdef BOTH
	*addr_fast = foreground;
#endif
	*addr = foreground;
	(void) addr_fast;
	(void) background;

	for(--count; count >= 0; count--) {
		if (d < 0) {
			d += incrE;
#ifdef BOTH
			addr_fast += one_step;
#endif
			addr += one_step;
		} else {
			d += incrNE;
#ifdef BOTH
			addr_fast += both_step;
#endif
			addr += both_step;
		}
#ifdef BOTH
		*addr_fast = foreground;
#endif
		*addr = foreground;
	}
}

static void line_revtransp_p(short *addr, short *addr_fast, long pattern, int count,
                        int d, int incrE, int incrNE, int one_step, int both_step,
                        short foreground, short background)
{
	unsigned int mask = 0x8000;

	(void) addr_fast;
	(void) background;
	if (!(pattern & mask)) {
#ifdef BOTH
		*addr_fast = foreground;
#endif
		*addr = foreground;
	}
		
	for(--count; count >= 0; count--) {
		if (d < 0) {
			d += incrE;
#ifdef BOTH
			addr_fast += one_step;
#endif
			addr += one_step;
		} else {
			d += incrNE;
#ifdef BOTH
			addr_fast += both_step;
#endif
			addr += both_step;
		}

		if (!(mask >>= 1))
			mask = 0x8000;

		if (!(pattern & mask)) {
#ifdef BOTH
			*addr_fast = foreground;
#endif
			*addr = foreground;
		}
	}
}

#ifdef BOTH_WAS_ON
#define BOTH
#endif

long CDECL c_line_draw(Virtual *vwk, long x1, long y1, long x2, long y2,
                       long pattern, long colour, long mode)
{
	Workstation *wk;
	short *addr, *addr_fast;
	long colours;
	short foreground, background;
  	int line_add;
	long pos;
	int x_step, y_step;
	int dx, dy;
	int one_step, both_step;
	int d, count;
	int incrE, incrNE;

	if ((long)vwk & 1) {
		return -1;			/* Don't know about anything yet */
	}

	if (!clip_line(vwk, &x1, &y1, &x2, &y2))
		return 1;

	colours = c_get_colour(vwk, colour);
	foreground = colours;
	background = colours >> 16;
	
	wk = vwk->real_address;

	pos = (short)y1 * (long)wk->screen.wrap + x1 * 2;
	addr = wk->screen.mfdb.address;
	line_add = wk->screen.wrap >> 1;


	x_step = 1;
	y_step = line_add;
	
	dx = x2 - x1;
	if (dx < 0) {
		dx = -dx;
		x_step = -x_step;
	}
	dy = y2 - y1;
	if (dy < 0) {
		dy = -dy;
		y_step = -y_step;
	}
	
	if (dx > dy) {
		count = dx;
		one_step = x_step;
		incrE = 2 * dy;
		incrNE = 2 * dy - 2 * dx;
		d = 2 * dy - dx;
	} else {
		count = dy;
		one_step = y_step;
		incrE = 2 * dx;
		incrNE = 2 * dx - 2 * dy;
		d = 2 * dx - dy;
	}
	both_step = x_step + y_step;

#ifdef BOTH
	if ((addr_fast = wk->screen.shadow.address) != 0) {

		addr += pos >> 1;
#ifdef BOTH
		addr_fast += pos >> 1;
#endif
		if ((pattern & 0xffff) == 0xffff) {
			switch (mode) {
			case 1:				/* Replace */
				s_line_replace(addr, addr_fast, count, d, incrE, incrNE, one_step, both_step, foreground, background);
				break;
			case 2:				/* Transparent */
				s_line_transparent(addr, addr_fast, count, d, incrE, incrNE, one_step, both_step, foreground, background);
				break;
			case 3:				/* XOR */
				s_line_xor(addr, addr_fast, count, d, incrE, incrNE, one_step, both_step, foreground, background);
				break;
			case 4:				/* Reverse transparent */
				s_line_revtransp(addr, addr_fast, count, d, incrE, incrNE, one_step, both_step, foreground, background);
				break;
			}
		} else {
			switch (mode) {
			case 1:				/* Replace */
				s_line_replace_p(addr, addr_fast, pattern, count, d, incrE, incrNE, one_step, both_step, foreground, background);
				break;
			case 2:				/* Transparent */
				s_line_transparent_p(addr, addr_fast, pattern, count, d, incrE, incrNE, one_step, both_step, foreground, background);
				break;
			case 3:				/* XOR */
				s_line_xor_p(addr, addr_fast, pattern, count, d, incrE, incrNE, one_step, both_step, foreground, background);
				break;
			case 4:				/* Reverse transparent */
				s_line_revtransp_p(addr, addr_fast, pattern, count, d, incrE, incrNE, one_step, both_step, foreground, background);
				break;
			}
		}
	} else {
#endif
		addr += pos >> 1;
		if ((pattern & 0xffff) == 0xffff) {
			switch (mode) {
			case 1:				/* Replace */
				line_replace(addr, addr_fast, count, d, incrE, incrNE, one_step, both_step, foreground, background);
				break;
			case 2:				/* Transparent */
				line_transparent(addr, addr_fast, count, d, incrE, incrNE, one_step, both_step, foreground, background);
				break;
			case 3:				/* XOR */
				line_xor(addr, addr_fast, count, d, incrE, incrNE, one_step, both_step, foreground, background);
				break;
			case 4:				/* Reverse transparent */
				line_revtransp(addr, addr_fast, count, d, incrE, incrNE, one_step, both_step, foreground, background);
				break;
			}
		} else {
			switch (mode) {
			case 1:				/* Replace */
				line_replace_p(addr, addr_fast, pattern, count, d, incrE, incrNE, one_step, both_step, foreground, background);
				break;
			case 2:				/* Transparent */
				line_transparent_p(addr, addr_fast, pattern, count, d, incrE, incrNE, one_step, both_step, foreground, background);
				break;
			case 3:				/* XOR */
				line_xor_p(addr, addr_fast, pattern, count, d, incrE, incrNE, one_step, both_step, foreground, background);
				break;
			case 4:				/* Reverse transparent */
				line_revtransp_p(addr, addr_fast, pattern, count, d, incrE, incrNE, one_step, both_step, foreground, background);
				break;
			}
		}
#ifdef BOTH
	}
#endif
	return 1;		/* Return as completed */
}
