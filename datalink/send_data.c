/* This software is Copyright 1996 by Karl R. Hakimian
 *
 * datazap: Linux version of Timex/Microsoft SDK for Timex datalink watches.
 *
 * Written by Karl R. Hakimian 10/3/96
 * 
 * Permission is hereby granted to copy, reproduce, redistribute or otherwise
 * use this software as long as: there is no monetary profit gained
 * specifically from the use or reproduction of this software, it is not
 * sold, rented, traded or otherwise marketed, and this copyright notice is
 * included prominently in any copy made. 
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. ANY USE OF THIS
 * SOFTWARE IS AT THE USER'S OWN RISK.
 *
 */

#include <stdio.h>
#include <vga.h>
#include "datalink_macros.h"

#define MODEL_70 0
#define MODEL_150 1
#define MODEL_IRONMAN 2

send_data(type, packets, npckts)
int type;
unsigned char **packets;
int npckts;
{
	int i;
	register int j;
	register unsigned char byte;
	unsigned char white[80];
	unsigned char black[80];
	int inc = (type == MODEL_70) ? 1 : 2;

	for (i = 0; i < 80; i++) {
		white[i] = 0xff;
		black[i] = 0x00;
	}

/* Become root to set vga mode. */
	seteuid(0);
	vga_init();
#ifdef MACH64_HACK
/* For some reason, the latest mach 64 svga driver requires root on the
   next call even though vga_init gave up root privileges. */
	seteuid(0);
#endif
	vga_setmode(G640x480x2);
#ifdef MACH64_HACK
	seteuid(getuid());
#endif

	SYNC
	DATASTART

	for (i = 0; i < npckts; i++) {

		for (j = 0; j <= *packets[i]; j += inc) {
			if( type == MODEL_IRONMAN && j == *packets[i] )
				break;
			printf("j is %d, *packets[i] is %d\n", j, *packets[i]);
			printf("sync\n");
			vga_waitretrace();
			byte = packets[i][j];
			WRITE_BYTE1(1, byte)
			printf("1-%x;  ", byte);

			if (type == MODEL_70)
				continue;

			if ( type == MODEL_IRONMAN )
			{
				byte = packets[i][j+1];
				WRITE_BYTE2(1, byte)
				printf("2-%x;  ", byte);
			}
			else
			{
				if (j + 1 == packets[i][0])
					WRITE_BYTE2(0, 0xff)
				else {
					byte = packets[i][j + 1];
					WRITE_BYTE2(1, byte)
				}
			}

		}

		END_PACKET
	}

#ifdef MACH64_HACK
/* For some reason, the latest mach 64 svga driver requires root on the
   next call even though vga_init gave up root privileges. */
	seteuid(0);
#endif
	vga_setmode(TEXT);
#ifdef MACH64_HACK
	seteuid(getuid());
#endif

}