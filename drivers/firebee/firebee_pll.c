/*
 * saga_pll.c - SAGA PLL data
 * This is part of the SAGA driver for fVDI
 * This file comes from the SAGA Picasso96 driver:
 * https://github.com/ezrec/saga-drivers/blob/master/saga.card/saga_pll.c
 * Glued by Vincent Riviere
 */

/*
 *  Copyright 2016, Jason S. McMullan <jason.mcmullan@gmail.com>
 *  Licensed under the MIT (Expat) license.
 *
 *  PLL data set: Copyright 2016, Christoph Hoehne <ceaich@gmx.de>
 */

#include "fvdi.h"
#include "driver.h"
#include "firebee.h"
#include "video.h"
#include <mint/osbind.h>

static struct saga_pll_data {
    ULONG freq;
    UBYTE data[18];
} fbee_pll[] = {
    { .freq = 20267942,
      .data = { 0x09,0xC0,0x60,0x00,0x01,0x90,0xC8,0x25,0x11,
                0x02,0x40,0xE0,0x00,0x08,0x00,0x02,0x00,0x00 } },
    { .freq = 20288210,
      .data = { 0x08,0x40,0x60,0x00,0x02,0x45,0x1C,0x32,0x19,
                0x02,0x81,0x60,0x00,0x08,0x00,0x02,0x00,0x00 } },
    { .freq = 20335502,
      .data = { 0x08,0x00,0x60,0x00,0x01,0x58,0xAC,0x1E,0x0F,
                0x01,0x80,0xE0,0x00,0x08,0x00,0x02,0x00,0x00 } },
    { .freq = 20378495,
      .data = { 0x08,0x40,0x60,0x00,0x02,0x79,0x3C,0x39,0x1B,
                0x03,0x41,0x60,0x00,0x08,0x00,0x02,0x00,0x00 } },
    { .freq = 20394617,
      .data = { 0x09,0xC0,0x60,0x00,0x01,0xD4,0xE4,0x28,0x14,
                0x02,0x01,0x20,0x00,0x08,0x00,0x02,0x00,0x00 } },
    { .freq = 20430086,
      .data = { 0x08,0x00,0x60,0x00,0x01,0x20,0x90,0x1B,0x0C,
                0x01,0xC0,0xA0,0x00,0x08,0x00,0x02,0x00,0x00 } },
    { .freq = 20453732,
      .data = { 0x08,0x40,0x60,0x00,0x02,0xBD,0x58,0x3C,0x1E,
                0x03,0x01,0xA0,0x00,0x08,0x00,0x02,0x00,0x00 } },
    { .freq = 20470622,
      .data = { 0x09,0xC0,0x60,0x00,0x01,0x9C,0xC8,0x25,0x11,
                0x02,0x40,0xE0,0x00,0x08,0x00,0x02,0x00,0x00 } },
    { .freq = 20493142,
      .data = { 0x08,0x40,0x60,0x00,0x02,0x09,0x04,0x2F,0x16,
                0x02,0xC1,0x20,0x00,0x08,0x00,0x02,0x00,0x00 } },
    { .freq = 20507473,
      .data = { 0x08,0x40,0x60,0x00,0x02,0x85,0x3C,0x39,0x1B,
                0x03,0x41,0x60,0x00,0x08,0x00,0x02,0x00,0x00 } },
    { .freq = 20571962,
      .data = { 0x08,0x00,0x60,0x00,0x01,0x64,0xAC,0x1E,0x0F,
                0x01,0x80,0xE0,0x00,0x08,0x00,0x02,0x00,0x00 } },
    { .freq = 20636450,
      .data = { 0x08,0x40,0x60,0x00,0x02,0x81,0x40,0x39,0x1B,
                0x03,0x41,0x60,0x00,0x08,0x00,0x02,0x00,0x00 } },
    { .freq = 20650781,
      .data = { 0x08,0x40,0x60,0x00,0x02,0x15,0x04,0x2F,0x16,
                0x02,0xC1,0x20,0x00,0x08,0x00,0x02,0x00,0x00 } },
    { .freq = 20673301,
      .data = { 0x09,0xC0,0x60,0x00,0x01,0x98,0xCC,0x25,0x11,
                0x02,0x40,0xE0,0x00,0x08,0x00,0x02,0x00,0x00 } },
    { .freq = 20690191,
      .data = { 0x08,0x40,0x60,0x00,0x02,0xC5,0x5C,0x3C,0x1E,
                0x03,0x01,0xA0,0x00,0x08,0x00,0x02,0x00,0x00 } },
    { .freq = 20713837,
      .data = { 0x08,0x00,0x60,0x00,0x01,0x2C,0x90,0x1B,0x0C,
                0x01,0xC0,0xA0,0x00,0x08,0x00,0x02,0x00,0x00 } },
    { .freq = 20749306,
      .data = { 0x09,0xC0,0x60,0x00,0x01,0xDC,0xE8,0x28,0x14,
                0x02,0x01,0x20,0x00,0x08,0x00,0x02,0x00,0x00 } },
    { .freq = 20765428,
      .data = { 0x08,0x40,0x60,0x00,0x02,0x8D,0x40,0x39,0x1B,
                0x03,0x41,0x60,0x00,0x08,0x00,0x02,0x00,0x00 } },
    { .freq = 20808421,
      .data = { 0x08,0x00,0x60,0x00,0x01,0x60,0xB0,0x1E,0x0F,
                0x01,0x80,0xE0,0x00,0x08,0x00,0x02,0x00,0x00 } },
    { .freq = 20855713,
      .data = { 0x08,0x40,0x60,0x00,0x02,0x55,0x24,0x32,0x19,
                0x02,0x81,0x60,0x00,0x08,0x00,0x02,0x00,0x00 } },
    { .freq = 20875981,
      .data = { 0x09,0xC0,0x60,0x00,0x01,0xA4,0xCC,0x25,0x11,
                0x02,0x40,0xE0,0x00,0x08,0x00,0x02,0x00,0x00 } },
    { .freq = 20894406,
      .data = { 0x08,0x40,0x60,0x00,0x02,0x89,0x44,0x39,0x1B,
                0x03,0x41,0x60,0x00,0x08,0x00,0x02,0x00,0x00 } },
    { .freq = 20926651,
      .data = { 0x09,0x80,0x60,0x00,0x00,0xF4,0x74,0x14,0x0A,
                0x01,0x00,0xA0,0x00,0x08,0x00,0x02,0x00,0x00 } },
    { .freq = 20966060,
      .data = { 0x08,0x40,0x60,0x00,0x02,0x1D,0x08,0x2F,0x16,
                0x02,0xC1,0x20,0x00,0x08,0x00,0x02,0x00,0x00 } },
    { .freq = 20997588,
      .data = { 0x08,0x00,0x60,0x00,0x01,0x28,0x94,0x1B,0x0C,
                0x01,0xC0,0xA0,0x00,0x08,0x00,0x02,0x00,0x00 } },
    { .freq = 21023384,
      .data = { 0x08,0x40,0x60,0x00,0x02,0x95,0x44,0x39,0x1B,
                0x03,0x41,0x60,0x00,0x08,0x00,0x02,0x00,0x00 } },
    { .freq = 21044880,
      .data = { 0x08,0x00,0x60,0x00,0x01,0x6C,0xB0,0x1E,0x0F,
                0x01,0x80,0xE0,0x00,0x08,0x00,0x02,0x00,0x00 } },
    { .freq = 21078660,
      .data = { 0x09,0xC0,0x60,0x00,0x01,0xA0,0xD0,0x25,0x11,
                0x02,0x40,0xE0,0x00,0x08,0x00,0x02,0x00,0x00 } },
    { .freq = 21103995,
      .data = { 0x09,0xC0,0x60,0x00,0x01,0xE4,0xEC,0x28,0x14,
                0x02,0x01,0x20,0x00,0x08,0x00,0x02,0x00,0x00 } },
    { .freq = 21123700,
      .data = { 0x08,0x40,0x60,0x00,0x02,0x19,0x0C,0x2F,0x16,
                0x02,0xC1,0x20,0x00,0x08,0x00,0x02,0x00,0x00 } },
    { .freq = 21139464,
      .data = { 0x08,0x40,0x60,0x00,0x02,0x5D,0x28,0x32,0x19,
                0x02,0x81,0x60,0x00,0x08,0x00,0x02,0x00,0x00 } },
    { .freq = 21152362,
      .data = { 0x08,0x40,0x60,0x00,0x02,0x91,0x48,0x39,0x1B,
                0x03,0x41,0x60,0x00,0x08,0x00,0x02,0x00,0x00 } },
    { .freq = 21163110,
      .data = { 0x08,0x40,0x60,0x00,0x02,0xD5,0x64,0x3C,0x1E,
                0x03,0x01,0xA0,0x00,0x08,0x00,0x02,0x00,0x00 } },
    { .freq = 21281340,
      .data = { 0x08,0x00,0x60,0x00,0x01,0x68,0xB4,0x1E,0x0F,
                0x01,0x80,0xE0,0x00,0x08,0x00,0x02,0x00,0x00 } },
    { .freq = 21399569,
      .data = { 0x08,0x40,0x60,0x00,0x02,0xDD,0x68,0x3C,0x1E,
                0x03,0x01,0xA0,0x00,0x08,0x00,0x02,0x00,0x00 } },
    { .freq = 21410317,
      .data = { 0x08,0x40,0x60,0x00,0x02,0x99,0x4C,0x39,0x1B,
                0x03,0x41,0x60,0x00,0x08,0x00,0x02,0x00,0x00 } },
    { .freq = 21423215,
      .data = { 0x08,0x40,0x60,0x00,0x02,0x65,0x2C,0x32,0x19,
                0x02,0x81,0x60,0x00,0x08,0x00,0x02,0x00,0x00 } },
    { .freq = 21438979,
      .data = { 0x08,0x40,0x60,0x00,0x02,0x21,0x10,0x2F,0x16,
                0x02,0xC1,0x20,0x00,0x08,0x00,0x02,0x00,0x00 } },
    { .freq = 21458684,
      .data = { 0x08,0x40,0x60,0x00,0x01,0xEC,0xF0,0x28,0x14,
                0x02,0x01,0x20,0x00,0x08,0x00,0x02,0x00,0x00 } },
    { .freq = 21484019,
      .data = { 0x09,0xC0,0x60,0x00,0x01,0xA8,0xD4,0x25,0x11,
                0x02,0x40,0xE0,0x00,0x08,0x00,0x02,0x00,0x00 } },
    { .freq = 21517799,
      .data = { 0x08,0x00,0x60,0x00,0x01,0x74,0xB4,0x1E,0x0F,
                0x01,0x80,0xE0,0x00,0x08,0x00,0x02,0x00,0x00 } },
    { .freq = 21539295,
      .data = { 0x08,0x40,0x60,0x00,0x02,0xA5,0x4C,0x39,0x1B,
                0x03,0x41,0x60,0x00,0x08,0x00,0x02,0x00,0x00 } },
    { .freq = 21565091,
      .data = { 0x08,0x00,0x60,0x00,0x01,0x30,0x98,0x1B,0x0C,
                0x01,0xC0,0xA0,0x00,0x08,0x00,0x02,0x00,0x00 } },
    { .freq = 21596619,
      .data = { 0x08,0x40,0x60,0x00,0x02,0x2D,0x10,0x2F,0x16,
                0x02,0xC1,0x20,0x00,0x08,0x00,0x02,0x00,0x00 } },
    { .freq = 21636029,
      .data = { 0x08,0x00,0x60,0x00,0x00,0xFC,0x78,0x14,0x0A,
                0x01,0x00,0xA0,0x00,0x08,0x00,0x02,0x00,0x00 } },
    { .freq = 21668273,
      .data = { 0x08,0x40,0x60,0x00,0x02,0xA1,0x50,0x39,0x1B,
                0x03,0x41,0x60,0x00,0x08,0x00,0x02,0x00,0x00 } },
    { .freq = 21686698,
      .data = { 0x09,0xC0,0x60,0x00,0x01,0xB4,0xD4,0x25,0x11,
                0x02,0x40,0xE0,0x00,0x08,0x00,0x02,0x00,0x00 } },
    { .freq = 21706966,
      .data = { 0x08,0x40,0x60,0x00,0x02,0x6D,0x30,0x32,0x19,
                0x02,0x81,0x60,0x00,0x08,0x00,0x02,0x00,0x00 } },
    { .freq = 21754258,
      .data = { 0x0A,0x00,0x60,0x00,0x00,0xB8,0x5C,0x11,0x07,
                0x01,0x40,0x60,0x00,0x08,0x00,0x02,0x00,0x00 } },
    { .freq = 21797251,
      .data = { 0x08,0x40,0x60,0x00,0x02,0xAD,0x50,0x39,0x1B,
                0x03,0x41,0x60,0x00,0x08,0x00,0x02,0x00,0x00 } },
    { .freq = 21813373,
      .data = { 0x08,0x40,0x60,0x00,0x01,0xF4,0xF4,0x28,0x14,
                0x02,0x01,0x20,0x00,0x08,0x00,0x02,0x00,0x00 } },
    { .freq = 21848842,
      .data = { 0x08,0x00,0x60,0x00,0x01,0x3C,0x98,0x1B,0x0C,
                0x01,0xC0,0xA0,0x00,0x08,0x00,0x02,0x00,0x00 } },
    { .freq = 21872488,
      .data = { 0x09,0xC0,0x60,0x00,0x01,0xB0,0xD8,0x25,0x11,
                0x02,0x40,0xE0,0x00,0x08,0x00,0x02,0x00,0x00 } },
    { .freq = 21889378,
      .data = { 0x09,0xC0,0x60,0x00,0x01,0xB0,0xD8,0x25,0x11,
                0x02,0x40,0xE0,0x00,0x08,0x00,0x02,0x00,0x00 } },
    { .freq = 21911898,
      .data = { 0x08,0x40,0x60,0x00,0x02,0x35,0x14,0x2F,0x16,
                0x02,0xC1,0x20,0x00,0x08,0x00,0x02,0x00,0x00 } },
    { .freq = 21926229,
      .data = { 0x08,0x40,0x60,0x00,0x02,0xA9,0x54,0x39,0x1B,
                0x03,0x41,0x60,0x00,0x08,0x00,0x02,0x00,0x00 } },
    { .freq = 21990718,
      .data = { 0x08,0x00,0x60,0x00,0x00,0xF8,0x7C,0x14,0x0A,
                0x01,0x00,0xA0,0x00,0x08,0x00,0x02,0x00,0x00 } },
    { .freq = 22055206,
      .data = { 0x08,0x40,0x60,0x00,0x02,0xB5,0x54,0x39,0x1B,
                0x03,0x41,0x60,0x00,0x08,0x00,0x02,0x00,0x00 } },
    { .freq = 22069537,
      .data = { 0x08,0x40,0x60,0x00,0x02,0x31,0x18,0x2F,0x16,
                0x02,0xC1,0x20,0x00,0x08,0x00,0x02,0x00,0x00 } },
    { .freq = 22092057,
      .data = { 0x09,0xC0,0x60,0x00,0x01,0xBC,0xD8,0x25,0x11,
                0x02,0x40,0xE0,0x00,0x08,0x00,0x02,0x00,0x00 } },
    { .freq = 22132593,
      .data = { 0x08,0x00,0x60,0x00,0x01,0x38,0x9C,0x1B,0x0C,
                0x01,0xC0,0xA0,0x00,0x08,0x00,0x02,0x00,0x00 } },
    { .freq = 22168062,
      .data = { 0x08,0x40,0x60,0x00,0x01,0xFC,0xF8,0x28,0x14,
                0x02,0x01,0x20,0x00,0x08,0x00,0x02,0x00,0x00 } },
    { .freq = 22184184,
      .data = { 0x08,0x40,0x60,0x00,0x02,0xB1,0x58,0x39,0x1B,
                0x03,0x41,0x60,0x00,0x08,0x00,0x02,0x00,0x00 } },
    { .freq = 22227177,
      .data = { 0x0A,0x00,0x60,0x00,0x00,0xC4,0x5C,0x11,0x07,
                0x01,0x40,0x60,0x00,0x08,0x00,0x02,0x00,0x00 } },
    { .freq = 22274469,
      .data = { 0x08,0x40,0x60,0x00,0x02,0x7D,0x38,0x32,0x19,
                0x02,0x81,0x60,0x00,0x08,0x00,0x02,0x00,0x00 } },
    { .freq = 22294737,
      .data = { 0x09,0xC0,0x60,0x00,0x01,0xB8,0xDC,0x25,0x11,
                0x02,0x40,0xE0,0x00,0x08,0x00,0x02,0x00,0x00 } },
    { .freq = 22313162,
      .data = { 0x08,0x40,0x60,0x00,0x02,0xBD,0x58,0x39,0x1B,
                0x03,0x41,0x60,0x00,0x08,0x00,0x02,0x00,0x00 } },
    { .freq = 22345407,
      .data = { 0x08,0x00,0x60,0x00,0x01,0x04,0x7C,0x14,0x0A,
                0x01,0x00,0xA0,0x00,0x08,0x00,0x02,0x00,0x00 } },
    { .freq = 22384816,
      .data = { 0x08,0x40,0x60,0x00,0x02,0x39,0x1C,0x2F,0x16,
                0x02,0xC1,0x20,0x00,0x08,0x00,0x02,0x00,0x00 } },
    { .freq = 22416344,
      .data = { 0x08,0x00,0x60,0x00,0x01,0x44,0x9C,0x1B,0x0C,
                0x01,0xC0,0xA0,0x00,0x08,0x00,0x02,0x00,0x00 } },
    { .freq = 22442140,
      .data = { 0x08,0x40,0x60,0x00,0x02,0xB9,0x5C,0x39,0x1B,
                0x03,0x41,0x60,0x00,0x08,0x00,0x02,0x00,0x00 } },
    { .freq = 22463636,
      .data = { 0x0A,0x40,0x60,0x00,0x01,0x84,0xBC,0x1E,0x0F,
                0x01,0x80,0xE0,0x00,0x08,0x00,0x02,0x00,0x00 } },
    { .freq = 22497416,
      .data = { 0x09,0xC0,0x60,0x00,0x01,0xC4,0xDC,0x25,0x11,
                0x02,0x40,0xE0,0x00,0x08,0x00,0x02,0x00,0x00 } },
    { .freq = 22522751,
      .data = { 0x08,0x40,0x60,0x00,0x02,0x04,0xFC,0x28,0x14,
                0x02,0x01,0x20,0x00,0x08,0x00,0x02,0x00,0x00 } },
    { .freq = 22542456,
      .data = { 0x08,0x40,0x60,0x00,0x02,0x45,0x1C,0x2F,0x16,
                0x02,0xC1,0x20,0x00,0x08,0x00,0x02,0x00,0x00 } },
    { .freq = 22558220,
      .data = { 0x08,0x40,0x60,0x00,0x02,0x85,0x3C,0x32,0x19,
                0x02,0x81,0x60,0x00,0x08,0x00,0x02,0x00,0x00 } },
    { .freq = 22571118,
      .data = { 0x08,0x40,0x60,0x00,0x02,0xC5,0x5C,0x39,0x1B,
                0x03,0x41,0x60,0x00,0x08,0x00,0x02,0x00,0x00 } },
    { .freq = 22700096,
      .data = { 0x08,0x00,0x60,0x00,0x01,0x40,0xA0,0x1B,0x0C,
                0x01,0xC0,0xA0,0x00,0x08,0x00,0x02,0x00,0x00 } },
    { .freq = 22829073,
      .data = { 0x08,0x40,0x60,0x00,0x02,0xCD,0x60,0x39,0x1B,
                0x03,0x41,0x60,0x00,0x08,0x00,0x02,0x00,0x00 } },
    { .freq = 22841971,
      .data = { 0x08,0x40,0x60,0x00,0x02,0x8D,0x40,0x32,0x19,
                0x02,0x81,0x60,0x00,0x08,0x00,0x02,0x00,0x00 } },
    { .freq = 22857735,
      .data = { 0x08,0x40,0x60,0x00,0x02,0x4D,0x20,0x2F,0x16,
                0x02,0xC1,0x20,0x00,0x08,0x00,0x02,0x00,0x00 } },
    { .freq = 22877440,
      .data = { 0x08,0x40,0x60,0x00,0x02,0x0D,0x00,0x28,0x14,
                0x02,0x01,0x20,0x00,0x08,0x00,0x02,0x00,0x00 } },
    { .freq = 22902775,
      .data = { 0x09,0xC0,0x60,0x00,0x01,0xCC,0xE0,0x25,0x11,
                0x02,0x40,0xE0,0x00,0x08,0x00,0x02,0x00,0x00 } },
    { .freq = 22936555,
      .data = { 0x09,0xC0,0x60,0x00,0x01,0x8C,0xC0,0x1E,0x0F,
                0x01,0x80,0xE0,0x00,0x08,0x00,0x02,0x00,0x00 } },
    { .freq = 22958051,
      .data = { 0x08,0x40,0x60,0x00,0x02,0xC9,0x64,0x39,0x1B,
                0x03,0x41,0x60,0x00,0x08,0x00,0x02,0x00,0x00 } },
    { .freq = 22983847,
      .data = { 0x08,0x00,0x60,0x00,0x01,0x4C,0xA0,0x1B,0x0C,
                0x01,0xC0,0xA0,0x00,0x08,0x00,0x02,0x00,0x00 } },
    { .freq = 23015375,
      .data = { 0x08,0x40,0x60,0x00,0x02,0x49,0x24,0x2F,0x16,
                0x02,0xC1,0x20,0x00,0x08,0x00,0x02,0x00,0x00 } },
    { .freq = 23054785,
      .data = { 0x08,0x00,0x60,0x00,0x01,0x0C,0x80,0x14,0x0A,
                0x01,0x00,0xA0,0x00,0x08,0x00,0x02,0x00,0x00 } },
    { .freq = 23087029,
      .data = { 0x08,0x40,0x60,0x00,0x02,0xD5,0x64,0x39,0x1B,
                0x03,0x41,0x60,0x00,0x08,0x00,0x02,0x00,0x00 } },
    { .freq = 23105454,
      .data = { 0x09,0xC0,0x60,0x00,0x01,0xC8,0xE4,0x25,0x11,
                0x02,0x40,0xE0,0x00,0x08,0x00,0x02,0x00,0x00 } },
    { .freq = 23125722,
      .data = { 0x08,0x40,0x60,0x00,0x02,0x95,0x44,0x32,0x19,
                0x02,0x81,0x60,0x00,0x08,0x00,0x02,0x00,0x00 } },
    { .freq = 23173014,
      .data = { 0x09,0x80,0x60,0x00,0x00,0xCC,0x60,0x11,0x07,
                0x01,0x40,0x60,0x00,0x08,0x00,0x02,0x00,0x00 } },
    { .freq = 23216007,
      .data = { 0x08,0x40,0x60,0x00,0x02,0xD1,0x68,0x39,0x1B,
                0x03,0x41,0x60,0x00,0x08,0x00,0x02,0x00,0x00 } },
};

#define PLL_NUM_CLOCKS (sizeof(fbee_pll) / sizeof(fbee_pll[0]))


#if 0
static unsigned long get_timer(void)
{
    return * (volatile unsigned long *) 0x4ba;
}

#define SYSTEM_CLOCK    264UL
#define PLL_TIMEOUT     (1000 * SYSTEM_CLOCK)

void fbee_wait_pll(void)
{
    unsigned long timeout = Supexec(get_timer);

    while (*FBEE_VIDEO_PLL_RECONFIG < 0)
    {
        if (Supexec(get_timer) - timeout >= PLL_TIMEOUT)
        {
            // KDEBUG("wait for PLL not busy timed out\r\n");
            break;
        }
    }
}
#endif

#if 0
static long fbee_init_videl(void)
{
    unsigned long fbee_video_control = FBEE_FIFO_ON | FBEE_REFRESH_ON | FBEE_VCS | FBEE_VCKE | FBEE_VDAC_ON | FBEE_CLK_PLL;

    return 0;
}
#endif

int fbee_pll_clock_count(void)
{
    return PLL_NUM_CLOCKS;
}

int fbee_pll_clock_freq(int id, BOOL is_ntsc, ULONG *freq)
{
    (void)is_ntsc;
    if (id < 0 || id >= (int)PLL_NUM_CLOCKS)
        return -1;

    *freq = fbee_pll[id].freq;

    return 0;
}

int fbee_pll_clock_lookup(BOOL is_ntsc, ULONG *freqp)
{
    int i;
    ULONG freq;

    (void)is_ntsc;
    if (!freqp)
        return -1;

    freq = *freqp;

    /* Find the closest clock */
    for (i = 0; i < (int)PLL_NUM_CLOCKS-1; i++) {
        ULONG split;

        if (freq <= fbee_pll[i].freq)
            break;

        split = (fbee_pll[i].freq + fbee_pll[i + 1].freq) / 2;

        if (freq < split)
            break;
    }

    *freqp = fbee_pll[i].freq;

    /* If we didn't find a match, we will return the largest
     * valid clock.
     */
    return i;
}

int fbee_pll_clock_program(int clock)
{
    int i;

    if (clock < 0 || clock >= (int)PLL_NUM_CLOCKS)
         return -1;

    for (i = 0; i < 18; i++) {
        UBYTE byte = fbee_pll[clock].data[17 - i];
        int j;

        Write32(SAGA_VIDEO_PLLW, SAGA_VIDEO_PLLW_MAGIC |
                                 SAGA_VIDEO_PLLW_CS(0) |
                                 SAGA_VIDEO_PLLW_CLK(0));

        for (j = 0; j < 8; j++, byte >>= 1) {
            Write32(SAGA_VIDEO_PLLW, SAGA_VIDEO_PLLW_MAGIC |
                                     SAGA_VIDEO_PLLW_MOSI(byte & 1) |
                                     SAGA_VIDEO_PLLW_CS(0) |
                                     SAGA_VIDEO_PLLW_CLK(0));
            Write32(SAGA_VIDEO_PLLW, SAGA_VIDEO_PLLW_MAGIC |
                                     SAGA_VIDEO_PLLW_MOSI(byte & 1) |
                                     SAGA_VIDEO_PLLW_CS(0) |
                                     SAGA_VIDEO_PLLW_CLK(1));
        }
    }

    Write32(SAGA_VIDEO_PLLW, SAGA_VIDEO_PLLW_MAGIC |
                             SAGA_VIDEO_PLLW_CS(1) |
                             SAGA_VIDEO_PLLW_CLK(0));
    Write32(SAGA_VIDEO_PLLW, SAGA_VIDEO_PLLW_MAGIC |
                             SAGA_VIDEO_PLLW_CS(1) |
                             SAGA_VIDEO_PLLW_CLK(1));
    Write32(SAGA_VIDEO_PLLW, SAGA_VIDEO_PLLW_MAGIC |
                             SAGA_VIDEO_PLLW_CS(1) |
                             SAGA_VIDEO_PLLW_CLK(0) |
                             SAGA_VIDEO_PLLW_UPDATE(1));
    Write32(SAGA_VIDEO_PLLW, SAGA_VIDEO_PLLW_MAGIC |
                             SAGA_VIDEO_PLLW_CS(1) |
                             SAGA_VIDEO_PLLW_CLK(1) |
                             SAGA_VIDEO_PLLW_UPDATE(1));
    /* Send 128 clock cycles to allow the PLL to update */
    for (i = 0; i < 128; i++) {
        Write32(SAGA_VIDEO_PLLW, SAGA_VIDEO_PLLW_MAGIC |
                                 SAGA_VIDEO_PLLW_CS(1) |
                                 SAGA_VIDEO_PLLW_CLK(0));
        Write32(SAGA_VIDEO_PLLW, SAGA_VIDEO_PLLW_MAGIC |
                                 SAGA_VIDEO_PLLW_CS(1) |
                                 SAGA_VIDEO_PLLW_CLK(1));
    }

    return 0;
}

