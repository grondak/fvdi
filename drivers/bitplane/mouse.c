/*
 * Bitplane mouse routine
 *
 * Copyright 2006, Johan Klockars 
 * Copyright 2002 The EmuTOS development team
 * Copyright 1999 by Caldera, Inc. and Authors:
 * Copyright 1982 by Digital Research Inc.
 *
 * This software is licensed under the GNU General Public License.
 * Please, see LICENSE.TXT for further information.
 */

#define NO_W  1
#define LOCAL_PTR 1

#if PLANES == 1
static long CDECL c_mouse_draw_1(Workstation * wk, long x, long y, Mouse * mouse)
#elif PLANES == 2
static long CDECL c_mouse_draw_2(Workstation * wk, long x, long y, Mouse * mouse)
#elif PLANES == 4
static long CDECL c_mouse_draw_4(Workstation * wk, long x, long y, Mouse * mouse)
#else
static long CDECL c_mouse_draw_8(Workstation * wk, long x, long y, Mouse * mouse)
#endif
{
#if !LOCAL_PTR
	UWORD *dst, *save_w, *mask_start;
#endif
	long state;
	static long old_colours = 0;
	static short colours = 0xaaaa;
	long *color_p;
	
	static volatile long save_state = 0;

	static unsigned short mouse_data[16 * 2] = {
		0xffff, 0x0000, 0x7ffe, 0x3ffc, 0x3ffc, 0x1ff8, 0x1ff8, 0x0ff0,
		0x0ff0, 0x07e0, 0x07e0, 0x03c0, 0x03c0, 0x0180, 0x0180, 0x0000
	};
	static unsigned short saved[16 * 8 * 2];

	if ((long) mouse > 7)
	{									/* New mouse shape */
		if (fix_shape)
			return 0;
		color_p = (long *) &wk->mouse.colour;
		if (*color_p != old_colours)
		{
			colours = set_mouse_colours(wk);
			old_colours = *color_p;
		}

		set_mouse_shape(mouse, mouse_data);
		return 0;
	}


	state = save_state;
	if (state && !no_restore && (((long) mouse == 0) || ((long) mouse == 2)))
	{									/* Move or Hide */
#if LOCAL_PTR
		UWORD *dst, *save_w;
#endif
		short h, wrap;
		wrap = wk->screen.wrap - PLANES * 2;
		dst = (UWORD *) ((long) wk->screen.mfdb.address + (state & 0x00ffffffL));
		h = ((unsigned long) state >> 24) & 0x0f;
		save_w = saved;

		if (state & 0x80000000L)
		{								/* Long? */
			do
			{
				switch (PLANES)
				{
				case 8:
					*dst++ = *save_w++;
					*(dst + PLANES - 1) = *save_w++;
					*dst++ = *save_w++;
					*(dst + PLANES - 1) = *save_w++;
					*dst++ = *save_w++;
					*(dst + PLANES - 1) = *save_w++;
					*dst++ = *save_w++;
					*(dst + PLANES - 1) = *save_w++;
					/* fall through */
				case 4:
					*dst++ = *save_w++;
					*(dst + PLANES - 1) = *save_w++;
					*dst++ = *save_w++;
					*(dst + PLANES - 1) = *save_w++;
					/* fall through */
				case 2:
					*dst++ = *save_w++;
					*(dst + PLANES - 1) = *save_w++;
					/* fall through */
				case 1:
					*dst++ = *save_w++;
					*(dst + PLANES - 1) = *save_w++;
					break;
				}
				dst = (UWORD *) ((long) dst + wrap);
			} while (--h >= 0);
		} else
		{								/* Word */
			do
			{
				switch (PLANES)
				{
				case 8:
					*dst++ = *save_w++;
					*dst++ = *save_w++;
					*dst++ = *save_w++;
					*dst++ = *save_w++;
					/* fall through */
				case 4:
					*dst++ = *save_w++;
					*dst++ = *save_w++;
					/* fall through */
				case 2:
					*dst++ = *save_w++;
					/* fall through */
				case 1:
					*dst++ = *save_w++;
					break;
				}
				dst = (UWORD *) ((long) dst + wrap);
			} while (--h >= 0);
		}

		save_state = 0;
	}

	if (((long) mouse == 0) || ((long) mouse == 3))
	{									/* Move or Show */
#if LOCAL_PTR
		UWORD *dst, *save_w, *mask_start;
#endif
		UWORD cdb_fgbg;
		short h, plane, wrap, shft, op;

#if !NO_W
		int w, xs, ys;
#endif

		x -= wk->mouse.hotspot.x;
		y -= wk->mouse.hotspot.y;
#if !NO_W
		w = 16;
#endif
		h = 16;

		op = 0;							/* Assume long operations */

		mask_start = mouse_data;
		if (y < wk->screen.coordinates.min_y)
		{
			int ys;

			ys = wk->screen.coordinates.min_y - y;
			h -= ys;
			y = wk->screen.coordinates.min_y;
			mask_start += ys << 1;
		}
		if (y + h - 1 > wk->screen.coordinates.max_y)
			h = wk->screen.coordinates.max_y - y + 1;

		shft = x & 0xf;
		if (!shft)
			op = 2;

		if (x < wk->screen.coordinates.min_x)
		{
#if !NO_W
			int xs;

			xs = wk->screen.coordinates.min_x - x;
			w -= xs;
#endif
			x = wk->screen.coordinates.min_x;
			op = 1;						/* Left clip */
		} else
		{
#if !NO_W
			if (x + w - 1 > wk->screen.coordinates.max_x)
			{
				w = wk->screen.coordinates.max_x - x + 1;
#else
			if (x + 16 - 1 > wk->screen.coordinates.max_x)
			{
#endif
				op = 2;					/* Right clip */
			}
		}

		wrap = (wk->screen.wrap >> 1) - PLANES;
		dst = (UWORD *) ((long) wk->screen.mfdb.address +
						 (short) y * (long) wk->screen.wrap + (short) (x >> 4) * (long) PLANES * 2);

		save_w = saved;

		if (h)
		{
			state = 0;
			state |= (long) (op == 0) << 31;
			state |= (long) ((h - 1) & 0x0f) << 24;
			state |= (long) dst - (long) wk->screen.mfdb.address;
		} else
		{
			state = 0;
			return 0;
		}

		switch (op)
		{
		case 0:						/* Long word */
			{
				ULONG bits, fg, bg;

				cdb_fgbg = colours;
				shft = 16 - shft;

				h--;
				do
				{
					bg = *mask_start++;
					bg <<= shft;
					fg = *mask_start++;
					fg <<= shft;

					plane = PLANES - 1;
					do
					{
						bits = *(ULONG *) dst & 0xffff0000;
						bits += *(ULONG *) (dst + PLANES) >> 16;
						*(ULONG *) save_w = bits;
						save_w += 2;

						cdb_fgbg = (cdb_fgbg >> 1) | (cdb_fgbg << 15);
						if (cdb_fgbg & 0x8000)
							bits |= bg;
						else
							bits &= ~bg;

						cdb_fgbg = (cdb_fgbg >> 1) | (cdb_fgbg << 15);
						if (cdb_fgbg & 0x8000)
							bits |= fg;
						else
							bits &= ~fg;

						*(dst + PLANES) = (UWORD) bits;
						bits = (bits << 16) | (bits >> 16);
						*dst++ = (UWORD) bits;

					} while (--plane >= 0);
					dst += wrap;
				} while (--h >= 0);
			}
			break;

		case 1:						/* Right word only */
			{
				UWORD bits, fg, bg;

				cdb_fgbg = colours;
				shft = 16 - shft;

				h--;
				do
				{
					bg = *mask_start++;
					bg <<= shft;
					fg = *mask_start++;
					fg <<= shft;

					plane = PLANES - 1;
					do
					{
						bits = *dst;
						*save_w++ = bits;

						cdb_fgbg = (cdb_fgbg >> 1) | (cdb_fgbg << 15);
						if (cdb_fgbg & 0x8000)
							bits |= bg;
						else
							bits &= ~bg;

						cdb_fgbg = (cdb_fgbg >> 1) | (cdb_fgbg << 15);
						if (cdb_fgbg & 0x8000)
							bits |= fg;
						else
							bits &= ~fg;

						*dst++ = bits;

					} while (--plane >= 0);
					dst += wrap;
				} while (--h >= 0);
			}
			break;

		default:						/* Left word only */
			{
				UWORD bits, fg, bg;

				cdb_fgbg = colours;

				h--;
				do
				{
					bg = *mask_start++;
					bg >>= shft;
					fg = *mask_start++;
					fg >>= shft;

					plane = PLANES - 1;
					do
					{
						bits = *dst;
						*save_w++ = bits;

						cdb_fgbg = (cdb_fgbg >> 1) | (cdb_fgbg << 15);
						if (cdb_fgbg & 0x8000)
							bits |= bg;
						else
							bits &= ~bg;

						cdb_fgbg = (cdb_fgbg >> 1) | (cdb_fgbg << 15);
						if (cdb_fgbg & 0x8000)
							bits |= fg;
						else
							bits &= ~fg;

						*dst++ = bits;

					} while (--plane >= 0);
					dst += wrap;
				} while (--h >= 0);
			}
			break;
		}

		save_state = state;
	}

	return 0;
}
