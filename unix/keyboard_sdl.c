/*
 *  Arnold emulator (c) Copyright, Kevin Thacker 1995-2001
 *
 *  This file is part of the Arnold emulator source code distribution.
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */
#include "../cpc/messages.h"
#include "global.h"
#include "ifacegen.h"
#include "display.h"
#include "../cpc/arnold.h"
#include "../cpc/cpc.h"
#include <dlfcn.h>
#include <mmenu.h>
#include <SDL/SDL.h>

// table to map KeySym values to CPC Key values
int KeySymToCPCKey[SDLK_LAST];

// Joystick handles
SDL_Joystick *joystick1, *joystick2;

// This is the area around the center of an analog Joystick where all movement
// is assumed to be jitter
#define JOYDEAD 3200

// Flasg for unicode keycode handling. Only used for spanish keyboard
// currently. Maybe used for all keyboards in the future.
int keyUnicodeFlag = 0;

#define MOUSE_NONE 0
#define MOUSE_JOY 1
#define MOUSE_SYMBI 2
int mouseType = 0;

#define SYMBIMOUSE_NONE (0<<6)
#define SYMBIMOUSE_X (1<<6)
#define SYMBIMOUSE_Y (2<<6)
#define SYMBIMOUSE_BUTTONS (3<<6)
#define SYMBIMOUSE_BL 1
#define SYMBIMOUSE_BR 2

void* mmenu;
extern char* rom_path;
char save_path[512];
const static char SAVE_DIRECTORY[] = "/mnt/SDCARD/Roms/GX4000/.gx4000/saves/";

extern SDL_Rect normal_surface;
extern SDL_Rect zoom1_surface;
extern SDL_Rect game_surface;
extern SDL_Surface *scaled_screen;
/*
 * MENU ELEMENTS
 */

//
// Font: THIN8X8.pf
// Exported from PixelFontEdit 2.7.0

typedef uint16_t u16;

unsigned char gui_font[2048] =
{
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,	// Char 000 (.)
	0x7E, 0x81, 0xA5, 0x81, 0xBD, 0x99, 0x81, 0x7E,	// Char 001 (.)
	0x7E, 0xFF, 0xDB, 0xFF, 0xC3, 0xE7, 0xFF, 0x7E,	// Char 002 (.)
	0x6C, 0xFE, 0xFE, 0xFE, 0x7C, 0x38, 0x10, 0x00,	// Char 003 (.)
	0x10, 0x38, 0x7C, 0xFE, 0x7C, 0x38, 0x10, 0x00,	// Char 004 (.)
	0x38, 0x7C, 0x38, 0xFE, 0xFE, 0x7C, 0x38, 0x7C,	// Char 005 (.)
	0x10, 0x10, 0x38, 0x7C, 0xFE, 0x7C, 0x38, 0x7C,	// Char 006 (.)
	0x00, 0x00, 0x18, 0x3C, 0x3C, 0x18, 0x00, 0x00,	// Char 007 (.)
	0xFF, 0xFF, 0xE7, 0xC3, 0xC3, 0xE7, 0xFF, 0xFF,	// Char 008 (.)
	0x00, 0x3C, 0x66, 0x42, 0x42, 0x66, 0x3C, 0x00,	// Char 009 (.)
	0xFF, 0xC3, 0x99, 0xBD, 0xBD, 0x99, 0xC3, 0xFF,	// Char 010 (.)
	0x0F, 0x07, 0x0F, 0x7D, 0xCC, 0xCC, 0xCC, 0x78,	// Char 011 (.)
	0x3C, 0x66, 0x66, 0x66, 0x3C, 0x18, 0x7E, 0x18,	// Char 012 (.)
	0x3F, 0x33, 0x3F, 0x30, 0x30, 0x70, 0xF0, 0xE0,	// Char 013 (.)
	0x7F, 0x63, 0x7F, 0x63, 0x63, 0x67, 0xE6, 0xC0,	// Char 014 (.)
	0x99, 0x5A, 0x3C, 0xE7, 0xE7, 0x3C, 0x5A, 0x99,	// Char 015 (.)
	0x80, 0xE0, 0xF8, 0xFE, 0xF8, 0xE0, 0x80, 0x00,	// Char 016 (.)
	0x02, 0x0E, 0x3E, 0xFE, 0x3E, 0x0E, 0x02, 0x00,	// Char 017 (.)
	0x18, 0x3C, 0x7E, 0x18, 0x18, 0x7E, 0x3C, 0x18,	// Char 018 (.)
	0x66, 0x66, 0x66, 0x66, 0x66, 0x00, 0x66, 0x00,	// Char 019 (.)
	0x7F, 0xDB, 0xDB, 0x7B, 0x1B, 0x1B, 0x1B, 0x00,	// Char 020 (.)
	0x3E, 0x63, 0x38, 0x6C, 0x6C, 0x38, 0xCC, 0x78,	// Char 021 (.)
	0x00, 0x00, 0x00, 0x00, 0x7E, 0x7E, 0x7E, 0x00,	// Char 022 (.)
	0x18, 0x3C, 0x7E, 0x18, 0x7E, 0x3C, 0x18, 0xFF,	// Char 023 (.)
	0x18, 0x3C, 0x7E, 0x18, 0x18, 0x18, 0x18, 0x00,	// Char 024 (.)
	0x18, 0x18, 0x18, 0x18, 0x7E, 0x3C, 0x18, 0x00,	// Char 025 (.)
	0x00, 0x18, 0x0C, 0xFE, 0x0C, 0x18, 0x00, 0x00,	// Char 026 (.) right arrow
	0x00, 0x30, 0x60, 0xFE, 0x60, 0x30, 0x00, 0x00,	// Char 027 (.)
	0x00, 0x00, 0xC0, 0xC0, 0xC0, 0xFE, 0x00, 0x00,	// Char 028 (.)
	0x00, 0x24, 0x66, 0xFF, 0x66, 0x24, 0x00, 0x00,	// Char 029 (.)
	0x00, 0x18, 0x3C, 0x7E, 0xFF, 0xFF, 0x00, 0x00,	// Char 030 (.)
	0x00, 0xFF, 0xFF, 0x7E, 0x3C, 0x18, 0x00, 0x00,	// Char 031 (.)
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,	// Char 032 ( )
	0x40, 0x40, 0x40, 0x40, 0x40, 0x00, 0x40, 0x00,	// Char 033 (!)
	0x90, 0x90, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,	// Char 034 (")
	0x50, 0x50, 0xF8, 0x50, 0xF8, 0x50, 0x50, 0x00,	// Char 035 (#)
	0x20, 0x78, 0xA0, 0x70, 0x28, 0xF0, 0x20, 0x00,	// Char 036 ($)
	0xC8, 0xC8, 0x10, 0x20, 0x40, 0x98, 0x98, 0x00,	// Char 037 (%)
	0x70, 0x88, 0x50, 0x20, 0x54, 0x88, 0x74, 0x00,	// Char 038 (&)
	0x60, 0x20, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00,	// Char 039 (')
	0x20, 0x40, 0x80, 0x80, 0x80, 0x40, 0x20, 0x00,	// Char 040 (()
	0x20, 0x10, 0x08, 0x08, 0x08, 0x10, 0x20, 0x00,	// Char 041 ())
	0x00, 0x20, 0xA8, 0x70, 0x70, 0xA8, 0x20, 0x00,	// Char 042 (*)
	0x00, 0x00, 0x20, 0x20, 0xF8, 0x20, 0x20, 0x00,	// Char 043 (+)
	0x00, 0x00, 0x00, 0x00, 0x00, 0x60, 0x20, 0x40,	// Char 044 (,)
	0x00, 0x00, 0x00, 0x00, 0xF8, 0x00, 0x00, 0x00,	// Char 045 (-)
	0x00, 0x00, 0x00, 0x00, 0x00, 0x60, 0x60, 0x00,	// Char 046 (.)
	0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80, 0x00,	// Char 047 (/)
	0x70, 0x88, 0x98, 0xA8, 0xC8, 0x88, 0x70, 0x00,	// Char 048 (0)
	0x40, 0xC0, 0x40, 0x40, 0x40, 0x40, 0xE0, 0x00,	// Char 049 (1)
	0x70, 0x88, 0x08, 0x10, 0x20, 0x40, 0xF8, 0x00,	// Char 050 (2)
	0x70, 0x88, 0x08, 0x10, 0x08, 0x88, 0x70, 0x00,	// Char 051 (3)
	0x08, 0x18, 0x28, 0x48, 0xFC, 0x08, 0x08, 0x00,	// Char 052 (4)
	0xF8, 0x80, 0x80, 0xF0, 0x08, 0x88, 0x70, 0x00,	// Char 053 (5)
	0x20, 0x40, 0x80, 0xF0, 0x88, 0x88, 0x70, 0x00,	// Char 054 (6)
	0xF8, 0x08, 0x10, 0x20, 0x40, 0x40, 0x40, 0x00,	// Char 055 (7)
	0x70, 0x88, 0x88, 0x70, 0x88, 0x88, 0x70, 0x00,	// Char 056 (8)
	0x70, 0x88, 0x88, 0x78, 0x08, 0x08, 0x70, 0x00,	// Char 057 (9)
	0x00, 0x00, 0x60, 0x60, 0x00, 0x60, 0x60, 0x00,	// Char 058 (:)
	0x00, 0x00, 0x60, 0x60, 0x00, 0x60, 0x60, 0x20,	// Char 059 (;)
	0x10, 0x20, 0x40, 0x80, 0x40, 0x20, 0x10, 0x00,	// Char 060 (<)
	0x00, 0x00, 0xF8, 0x00, 0xF8, 0x00, 0x00, 0x00,	// Char 061 (=)
	0x80, 0x40, 0x20, 0x10, 0x20, 0x40, 0x80, 0x00,	// Char 062 (>)
	0x78, 0x84, 0x04, 0x08, 0x10, 0x00, 0x10, 0x00,	// Char 063 (?)
	0x70, 0x88, 0x88, 0xA8, 0xB8, 0x80, 0x78, 0x00,	// Char 064 (@)
	0x20, 0x50, 0x88, 0x88, 0xF8, 0x88, 0x88, 0x00,	// Char 065 (A)
	0xF0, 0x88, 0x88, 0xF0, 0x88, 0x88, 0xF0, 0x00,	// Char 066 (B)
	0x70, 0x88, 0x80, 0x80, 0x80, 0x88, 0x70, 0x00,	// Char 067 (C)
	0xF0, 0x88, 0x88, 0x88, 0x88, 0x88, 0xF0, 0x00,	// Char 068 (D)
	0xF8, 0x80, 0x80, 0xE0, 0x80, 0x80, 0xF8, 0x00,	// Char 069 (E)
	0xF8, 0x80, 0x80, 0xE0, 0x80, 0x80, 0x80, 0x00,	// Char 070 (F)
	0x70, 0x88, 0x80, 0x80, 0x98, 0x88, 0x78, 0x00,	// Char 071 (G)
	0x88, 0x88, 0x88, 0xF8, 0x88, 0x88, 0x88, 0x00,	// Char 072 (H)
	0xE0, 0x40, 0x40, 0x40, 0x40, 0x40, 0xE0, 0x00,	// Char 073 (I)
	0x38, 0x10, 0x10, 0x10, 0x10, 0x90, 0x60, 0x00,	// Char 074 (J)
	0x88, 0x90, 0xA0, 0xC0, 0xA0, 0x90, 0x88, 0x00,	// Char 075 (K)
	0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0xF8, 0x00,	// Char 076 (L)
	0x82, 0xC6, 0xAA, 0x92, 0x82, 0x82, 0x82, 0x00,	// Char 077 (M)
	0x84, 0xC4, 0xA4, 0x94, 0x8C, 0x84, 0x84, 0x00,	// Char 078 (N)
	0x70, 0x88, 0x88, 0x88, 0x88, 0x88, 0x70, 0x00,	// Char 079 (O)
	0xF0, 0x88, 0x88, 0xF0, 0x80, 0x80, 0x80, 0x00,	// Char 080 (P)
	0x70, 0x88, 0x88, 0x88, 0xA8, 0x90, 0x68, 0x00,	// Char 081 (Q)
	0xF0, 0x88, 0x88, 0xF0, 0xA0, 0x90, 0x88, 0x00,	// Char 082 (R)
	0x70, 0x88, 0x80, 0x70, 0x08, 0x88, 0x70, 0x00,	// Char 083 (S)
	0xF8, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x00,	// Char 084 (T)
	0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x70, 0x00,	// Char 085 (U)
	0x88, 0x88, 0x88, 0x50, 0x50, 0x20, 0x20, 0x00,	// Char 086 (V)
	0x82, 0x82, 0x82, 0x82, 0x92, 0x92, 0x6C, 0x00,	// Char 087 (W)
	0x88, 0x88, 0x50, 0x20, 0x50, 0x88, 0x88, 0x00,	// Char 088 (X)
	0x88, 0x88, 0x88, 0x50, 0x20, 0x20, 0x20, 0x00,	// Char 089 (Y)
	0xF8, 0x08, 0x10, 0x20, 0x40, 0x80, 0xF8, 0x00,	// Char 090 (Z)
	0xE0, 0x80, 0x80, 0x80, 0x80, 0x80, 0xE0, 0x00,	// Char 091 ([)
	0x80, 0x40, 0x20, 0x10, 0x08, 0x04, 0x02, 0x00,	// Char 092 (\)
	0xE0, 0x20, 0x20, 0x20, 0x20, 0x20, 0xE0, 0x00,	// Char 093 (])
	0x20, 0x50, 0x88, 0x00, 0x00, 0x00, 0x00, 0x00,	// Char 094 (^)
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xF8, 0x00,	// Char 095 (_)
	0x40, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,	// Char 096 (`)
	0x00, 0x00, 0x70, 0x08, 0x78, 0x88, 0x74, 0x00,	// Char 097 (a)
	0x80, 0x80, 0xB0, 0xC8, 0x88, 0xC8, 0xB0, 0x00,	// Char 098 (b)
	0x00, 0x00, 0x70, 0x88, 0x80, 0x88, 0x70, 0x00,	// Char 099 (c)
	0x08, 0x08, 0x68, 0x98, 0x88, 0x98, 0x68, 0x00,	// Char 100 (d)
	0x00, 0x00, 0x70, 0x88, 0xF8, 0x80, 0x70, 0x00,	// Char 101 (e)
	0x30, 0x48, 0x40, 0xE0, 0x40, 0x40, 0x40, 0x00,	// Char 102 (f)
	0x00, 0x00, 0x34, 0x48, 0x48, 0x38, 0x08, 0x30,	// Char 103 (g)
	0x80, 0x80, 0xB0, 0xC8, 0x88, 0x88, 0x88, 0x00,	// Char 104 (h)
	0x20, 0x00, 0x60, 0x20, 0x20, 0x20, 0x70, 0x00,	// Char 105 (i)
	0x10, 0x00, 0x30, 0x10, 0x10, 0x10, 0x90, 0x60,	// Char 106 (j)
	0x80, 0x80, 0x88, 0x90, 0xA0, 0xD0, 0x88, 0x00,	// Char 107 (k)
	0xC0, 0x40, 0x40, 0x40, 0x40, 0x40, 0xE0, 0x00,	// Char 108 (l)
	0x00, 0x00, 0xEC, 0x92, 0x92, 0x92, 0x92, 0x00,	// Char 109 (m)
	0x00, 0x00, 0xB0, 0xC8, 0x88, 0x88, 0x88, 0x00,	// Char 110 (n)
	0x00, 0x00, 0x70, 0x88, 0x88, 0x88, 0x70, 0x00,	// Char 111 (o)
	0x00, 0x00, 0xB0, 0xC8, 0xC8, 0xB0, 0x80, 0x80,	// Char 112 (p)
	0x00, 0x00, 0x68, 0x98, 0x98, 0x68, 0x08, 0x08,	// Char 113 (q)
	0x00, 0x00, 0xB0, 0xC8, 0x80, 0x80, 0x80, 0x00,	// Char 114 (r)
	0x00, 0x00, 0x78, 0x80, 0x70, 0x08, 0xF0, 0x00,	// Char 115 (s)
	0x40, 0x40, 0xE0, 0x40, 0x40, 0x50, 0x20, 0x00,	// Char 116 (t)
	0x00, 0x00, 0x88, 0x88, 0x88, 0x98, 0x68, 0x00,	// Char 117 (u)
	0x00, 0x00, 0x88, 0x88, 0x88, 0x50, 0x20, 0x00,	// Char 118 (v)
	0x00, 0x00, 0x82, 0x82, 0x92, 0x92, 0x6C, 0x00,	// Char 119 (w)
	0x00, 0x00, 0x88, 0x50, 0x20, 0x50, 0x88, 0x00,	// Char 120 (x)
	0x00, 0x00, 0x88, 0x88, 0x98, 0x68, 0x08, 0x70,	// Char 121 (y)
	0x00, 0x00, 0xF8, 0x10, 0x20, 0x40, 0xF8, 0x00,	// Char 122 (z)
	0x10, 0x20, 0x20, 0x40, 0x20, 0x20, 0x10, 0x00,	// Char 123 ({)
	0x40, 0x40, 0x40, 0x00, 0x40, 0x40, 0x40, 0x00,	// Char 124 (|)
	0x40, 0x20, 0x20, 0x10, 0x20, 0x20, 0x40, 0x00,	// Char 125 (})
	0x76, 0xDC, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,	// Char 126 (~)
	0x00, 0x10, 0x38, 0x6C, 0xC6, 0xC6, 0xFE, 0x00	// Char 127 (.)
};

SDL_Surface *menuSurface = NULL;

int gui_Zoom = 0;
int gui_Frameskip = 0;
int gui_CRTC = 0;
int gui_RealSpeed = 1;
int gui_Joy = 0;


static const char *gui_JoyInput[2] = {"J1", "J2"};

static const char *gui_YesNo[2] = {"no", "yes"};

static const char *CRTC_Types[5] = { "CRTC 0", "CRTC 1", "CRTC 2", "CRTC 3",
	"CRTC 4" };
static const char *zoom_Values[4] = { "Normal", "Zoom" };

void menu_Exit()
{
	SDL_FillRect(sdl_screen, NULL, 0x000000);
	SDL_FillRect(scaled_screen, NULL, 0x000000);
	SDL_FillRect(screen, NULL, 0x000000);
	SDL_Flip(screen);
	SDL_Quit();
}

void menu_ChangeRealSpeed() {
	puts("MENU_RealSpeed BEGIN");
	if(gui_RealSpeed == 0) {
		sdl_LockSpeed = 0;
	}
	else {
		sdl_LockSpeed = 1;
	}
	puts("MENU_RealSpeed END");
}

void menu_ChangeFrameskip() {
	CPC_SetFrameSkip(gui_Frameskip);
}

void menu_ChangeRatio() {
	// normal: fullscreen 320*240
	if(gui_Zoom == 0) {
		game_surface = normal_surface;
		if (scaled_screen != NULL)
			free(scaled_screen);
		scaled_screen = SDL_CreateRGBSurface(SDL_HWSURFACE, 320, 240, 16, 0, 0, 0, 0);
	}
	// zoom 1 : cropped 280*210 (avenger, abu simbel)
	if(gui_Zoom == 1) {
		game_surface = zoom1_surface;
		if (scaled_screen != NULL)
			free(scaled_screen);
		scaled_screen = SDL_CreateRGBSurface(SDL_HWSURFACE, 256, 192, 16, 0, 0, 0, 0);
	}
}

void menu_ChangeCRTC() {
	CurrentCRTCType = gui_CRTC;
	CPC_SetCRTCType(CurrentCRTCType);
}

void menu_ChangeJoyInput() {
}

typedef struct {
	const char *itemName;
	int *itemPar;
	int itemParMaxValue;
	const char **itemParName;
	void (*itemOnA)();
} MENUITEM;

typedef struct {
	int itemNum; // number of items
	int itemCur; // current item
	MENUITEM *m; // array of items
} MENU;

// some custom ratios ?
// frameskip 0 - 5
// CRTC choice
// Throttle/real speed
// Exit

MENUITEM gui_MainMenuItems[] = {
	{(const char *)"Aspect: ", &gui_Zoom, 1, (const char **)&zoom_Values, &menu_ChangeRatio},
	{(const char *)"Input for: ", &gui_Joy, 1, (const char **)&gui_JoyInput, &menu_ChangeJoyInput},
	{(const char *)"Frameskip: ", &gui_Frameskip, 5, 0, &menu_ChangeFrameskip},
	{(const char *)"CRTC choice: ", &gui_CRTC, 4, (const char **)&CRTC_Types, &menu_ChangeCRTC},
	{(const char *)"Realspeed: ", &gui_RealSpeed, 1, (const char **)&gui_YesNo, &menu_ChangeRealSpeed},
	{(const char *)"Exit", 0, 0, 0, &menu_Exit}
};

MENU gui_MainMenu = { 6, 0, (MENUITEM *)&gui_MainMenuItems };

#define color16(red, green, blue) ((red << 11) | (green << 5) | blue)

#define COLOR_BG            color16(05, 03, 02)
#define COLOR_ROM_INFO      color16(22, 36, 26)
#define COLOR_ACTIVE_ITEM   color16(31, 63, 31)
#define COLOR_INACTIVE_ITEM color16(13, 40, 18)
#define COLOR_FRAMESKIP_BAR color16(15, 31, 31)
#define COLOR_HELP_TEXT     color16(16, 40, 24)

/*
	Shows menu items and pointing arrow
*/
void menu_ShowMenu(MENU *menu)
{
	/*
	 *
	 */

	int i;
	MENUITEM *mi = menu->m;

	// clear buffer
	SDL_FillRect(menuSurface, NULL, COLOR_BG);

	// show menu lines
	for(i = 0; i < menu->itemNum; i++, mi++) {
		int fg_color;
		if(menu->itemCur == i) fg_color = COLOR_ACTIVE_ITEM; else fg_color = COLOR_INACTIVE_ITEM;
		ShowMenuItem(80, (8 + i) * 10, mi, fg_color);
	}

	print_string("Arnold for Trimui : " __DATE__ " build", COLOR_HELP_TEXT, COLOR_BG, 5, 2);
	print_string("[B] = Return to game", COLOR_HELP_TEXT, COLOR_BG, 5, 210);
	print_string("Port by Gameblabla", COLOR_HELP_TEXT, COLOR_BG, 5, 220);
	print_string("Trimui version by Liartes", COLOR_HELP_TEXT, COLOR_BG, 5, 230);

}

void ShowMenuItem(int x, int y, MENUITEM *m, int fg_color)
{
	static char i_str[24];

	// if no parameters, show simple menu item
	if(m->itemPar == NULL) print_string(m->itemName, fg_color, COLOR_BG, x, y);
	else {
		if(m->itemParName == NULL) {
			// if parameter is a digit
			snprintf(i_str, sizeof(i_str), "%s%i", m->itemName, *m->itemPar);
		} else {
			// if parameter is a name in array
			snprintf(i_str, sizeof(i_str),  "%s%s", m->itemName, *(m->itemParName + *m->itemPar));
		}
		print_string(i_str, fg_color, COLOR_BG, x, y);
	}
}
void ShowChar(SDL_Surface *s, int x, int y, unsigned char a, int fg_color, int bg_color)
{
	Uint16 *dst;
	int w, h;

	if(SDL_MUSTLOCK(s)) SDL_LockSurface(s);
	for(h = 8; h; h--) {
		dst = (Uint16 *)s->pixels + (y+8-h)*s->w + x;
		for(w = 8; w; w--) {
			Uint16 color = bg_color; // background
			if((gui_font[a*8 + (8-h)] >> w) & 1) color = fg_color; // test bits 876543210
			*dst++ = color;
		}
	}
	if(SDL_MUSTLOCK(s)) SDL_UnlockSurface(s);
}


void print_string(const char *s, u16 fg_color, u16 bg_color, int x, int y)
{
	int i, j = strlen(s);
	for(i = 0; i < j; i++, x += 8) ShowChar(menuSurface, x, y, s[i], fg_color, bg_color);
}

/*
	Main function that runs all the stuff
*/
void menu_MainShow(MENU *menu)
{
	SDL_Event gui_event;
	MENUITEM *mi;

	BOOL done = FALSE;


	/* **************refresh config****************** */
	gui_RealSpeed = sdl_LockSpeed;
	gui_CRTC = CurrentCRTCType;
	gui_Frameskip = FrameSkip;
	/* ********************************************** */

	while(!done)
	{
		mi = menu->m + menu->itemCur; // pointer to highlit menu option

		while(SDL_PollEvent(&gui_event))
		{
			puts("MENU SDL_POLLEVENT LOOP");
			if(gui_event.type == SDL_KEYDOWN) {
				// DINGOO A - apply parameter or enter submenu
				//if(gui_event.key.keysym.sym == SDLK_RETURN) if(mi->itemOnA != NULL) (*mi->itemOnA)();
				// if(gui_event.key.keysym.sym == SDLK_LCTRL) if(mi->itemOnA != NULL) (*mi->itemOnA)();
				puts("VALIDATION MENU ITEM BEGIN");
				if(gui_event.key.keysym.sym == SDLK_SPACE) if(mi->itemOnA != NULL) mi->itemOnA(); // TRIMUI A
				puts("VALIDATION MENU ITEM END");
				// DINGOO B - exit or back to previous menu
				//if(gui_event.key.keysym.sym == SDLK_ESCAPE) return;
				// if(gui_event.key.keysym.sym == SDLK_LALT) return;
				if(gui_event.key.keysym.sym == SDLK_LCTRL) return; // TRIMUI B

				// DINGOO UP - arrow down
				if(gui_event.key.keysym.sym == SDLK_UP) if(--menu->itemCur < 0) menu->itemCur = menu->itemNum - 1;

				// DINGOO DOWN - arrow up
				if(gui_event.key.keysym.sym == SDLK_DOWN) if(++menu->itemCur == menu->itemNum) menu->itemCur = 0;

				// DINGOO LEFT - decrease parameter value
				if(gui_event.key.keysym.sym == SDLK_LEFT) {
					if(mi->itemPar != NULL && *mi->itemPar > 0) *mi->itemPar -= 1;
				}
				// DINGOO RIGHT - increase parameter value
				if(gui_event.key.keysym.sym == SDLK_RIGHT) {
					if(mi->itemPar != NULL && *mi->itemPar < mi->itemParMaxValue) *mi->itemPar += 1;
				}
			}
		}
		if(!done) menu_ShowMenu(menu); // show menu items
		SDL_Delay(16);
		gui_Flip();
	}
	SDL_FillRect(menuSurface, NULL, 0);
	SDL_Flip(menuSurface);
	SDL_FillRect(menuSurface, NULL, 0);
	SDL_Flip(menuSurface);
	#ifdef SDL_TRIPLEBUF
	SDL_FillRect(menuSurface, NULL, 0);
	SDL_Flip(menuSurface);
	#endif
}

void gui_Flip()
{
	if (screen->w == 320 || screen->w == 240)
	{
		SDL_BlitSurface(menuSurface, NULL, screen, NULL);
	}
	else
	{
		SDL_SoftStretch(menuSurface, NULL, screen, NULL);
	}
	SDL_Flip(screen);
}

/*
 * END MENU ELEMENTS
 */

char intto6bitsigned(int x) {
	char ax5 = ((char) abs(x)) & 0x1f;
	if (x < 0) {
		return ((ax5 ^ 0x3f) + 1);
	} else {
		return (ax5);
	}
}

extern void quit(void);		// FIXME

// State is True for Key Pressed, False for Key Release.
// theEvent holds the keyboard event.
void HandleKey(SDL_KeyboardEvent *theEvent) {
	//int State = ( theEvent->type == SDL_KEYDOWN ) ? TRUE : FALSE;
	int key_upper_code;
	//int keycode;
	CPC_KEY_ID theKeyPressed;

	// get KeySym
	SDL_keysym *keysym = &theEvent->keysym;
	SDLKey keycode = keysym->sym;

	/* Handle Function keys to control emulator */
	/*if (keycode == SDLK_F1 && theEvent->type == SDL_KEYDOWN ) {
	 CPC_Reset();
	 } else if (keycode == SDLK_F2 && theEvent->type == SDL_KEYDOWN ) {
	 //DisplayMode ^=0x0ff;
	 sdl_toggleDisplayFullscreen();
	 } else if (keycode == SDLK_F3 && theEvent->type == SDL_KEYDOWN ) {
	 SDL_GrabMode grabmode = SDL_WM_GrabInput(SDL_GRAB_QUERY);
	 fprintf(stderr,"%i\n",grabmode);
	 if (grabmode == SDL_GRAB_OFF) {
	 fprintf(stderr,"Grab\n");
	 SDL_WM_GrabInput(SDL_GRAB_ON);
	 SDL_ShowCursor(SDL_DISABLE);
	 } else {
	 fprintf(stderr,"Ungrab\n");
	 SDL_WM_GrabInput(SDL_GRAB_OFF);
	 SDL_ShowCursor(SDL_ENABLE);
	 }
	 } else if (keycode == SDLK_F4 && theEvent->type == SDL_KEYDOWN ) {
	 quit();
	 } else if (keycode == SDLK_F11 && theEvent->type == SDL_KEYDOWN ) {
	 // save a snapshot, don't bother whether 128K or 64KB RAM are used
	 GenericInterface_SnapshotSave("arnold01.sna", 3, 128);
	 } else if (keycode == SDLK_F12 && theEvent->type == SDL_KEYDOWN ) {
	 GenericInterface_LoadSnapshot("arnold01.sna");
	 // Handle CPC keys
	 } else*/{
		//printf("Keycode: <%04x> <%04x> <%04x> <%04x>\n",
		//	keysym->scancode, keysym->sym, keysym->mod, keysym->unicode );

		if (keycode <= SDLK_LAST) {
			theKeyPressed = KeySymToCPCKey[keycode];
			if (keyUnicodeFlag) {
				/* Test the UNICODE key value */
				theKeyPressed = KeySymToCPCKey[keysym->unicode];
			}
			if (theKeyPressed == CPC_KEY_NULL)
				printf(Messages[86], keysym->sym);
		} else {
			theKeyPressed = CPC_KEY_NULL;
			printf(Messages[87], keysym->sym);
		}

		// set or release key depending on state
		if (theEvent->type == SDL_KEYDOWN) {
			switch (keycode) {
			case SDLK_LCTRL:
				if(gui_Joy == 0) {
					CPC_SetKey(CPC_KEY_JOY_FIRE1);
				}
				else {
					CPC_SetKey(CPC_JOY1_FIRE1);
				}
				break;
			case SDLK_LALT:
				if(gui_Joy == 0) {
					CPC_SetKey(CPC_KEY_JOY_FIRE2);
				}
				else {
					CPC_SetKey(CPC_JOY1_FIRE2);
				}
				break;
			case SDLK_UP:
				if(gui_Joy == 0) {
					CPC_SetKey(CPC_KEY_JOY_UP);
				}
				else {
					CPC_SetKey(CPC_JOY1_UP);
				}
				break;
			case SDLK_DOWN:
				if(gui_Joy == 0) {
					CPC_SetKey(CPC_KEY_JOY_DOWN);
				}
				else {
					CPC_SetKey(CPC_JOY1_DOWN);
				}
				break;
			case SDLK_LEFT:
				if(gui_Joy == 0) {
					CPC_SetKey(CPC_KEY_JOY_LEFT);
				}
				else {
					CPC_SetKey(CPC_JOY1_LEFT);
				}
				break;
			case SDLK_RIGHT:
				if(gui_Joy == 0) {
					CPC_SetKey(CPC_KEY_JOY_RIGHT);
				}
				else {
					CPC_SetKey(CPC_JOY1_RIGHT);
				}
				break;
			case SDLK_RETURN:
				CPC_SetKey(CPC_KEY_P);
				break;
			case SDLK_TAB:
				CPC_SetKey(CPC_KEY_G);
				break;
			case SDLK_BACKSPACE:
				CPC_SetKey(CPC_KEY_F);
				break;
			default:
				break;
			}
			// set key
			//CPC_SetKey(theKeyPressed);
		} else if (theEvent->type == SDL_KEYUP) {
			switch (keycode) {
			case SDLK_LCTRL:
				if(gui_Joy == 0){
					CPC_ClearKey(CPC_KEY_JOY_FIRE1);
				}
				else {
					CPC_ClearKey(CPC_JOY1_FIRE1);
				}
				break;
			case SDLK_LALT:
				if(gui_Joy == 0){
				CPC_ClearKey(CPC_KEY_JOY_FIRE2);
				}
				else {
					CPC_ClearKey(CPC_JOY1_FIRE2);
				}
				break;
			case SDLK_UP:
				if(gui_Joy == 0){
				CPC_ClearKey(CPC_KEY_JOY_UP);
				}
				else {
					CPC_ClearKey(CPC_JOY1_UP);
				}
				break;
			case SDLK_DOWN:
				if(gui_Joy == 0){
				CPC_ClearKey(CPC_KEY_JOY_DOWN);
				}
				else {
					CPC_ClearKey(CPC_JOY1_DOWN);
				}
				break;
			case SDLK_LEFT:
				if(gui_Joy == 0){
				CPC_ClearKey(CPC_KEY_JOY_LEFT);
				}
				else {
					CPC_ClearKey(CPC_JOY1_LEFT);
				}
				break;
			case SDLK_RIGHT:
				if(gui_Joy == 0){
				CPC_ClearKey(CPC_KEY_JOY_RIGHT);
				}
				else {
					CPC_ClearKey(CPC_JOY1_RIGHT);
				}
				break;
			case SDLK_RETURN:
				CPC_ClearKey(CPC_KEY_P);
				break;
			case SDLK_TAB:
				CPC_ClearKey(CPC_KEY_G);
				break;
			case SDLK_BACKSPACE:
				CPC_ClearKey(CPC_KEY_F);
				break;
			default:
				break;
			}
			// release key
			//CPC_ClearKey(theKeyPressed);
		}
	}
}

void HandleJoy(SDL_JoyAxisEvent *event) {
	if (event->axis == 0) {
		/* Left-right movement */
		if ((event->value < -JOYDEAD)) {
			CPC_SetKey(CPC_KEY_JOY_LEFT);
			CPC_ClearKey(CPC_KEY_JOY_RIGHT);
		} else if ((event->value > JOYDEAD)) {
			CPC_ClearKey(CPC_KEY_JOY_LEFT);
			CPC_SetKey(CPC_KEY_JOY_RIGHT);
		} else {
			CPC_ClearKey(CPC_KEY_JOY_LEFT);
			CPC_ClearKey(CPC_KEY_JOY_RIGHT);
		}
	}
	if (event->axis == 1) {
		/* Up-Down movement */
		if ((event->value < -JOYDEAD)) {
			CPC_SetKey(CPC_KEY_JOY_UP);
			CPC_ClearKey(CPC_KEY_JOY_DOWN);
		} else if ((event->value > JOYDEAD)) {
			CPC_ClearKey(CPC_KEY_JOY_UP);
			CPC_SetKey(CPC_KEY_JOY_DOWN);
		} else {
			CPC_ClearKey(CPC_KEY_JOY_UP);
			CPC_ClearKey(CPC_KEY_JOY_DOWN);
		}
	}

	if (event->axis == 2) {
		/* Left-right movement */
		if ((event->value < -JOYDEAD)) {
			CPC_SetKey(CPC_JOY1_LEFT);
			CPC_ClearKey(CPC_JOY1_RIGHT);
		} else if ((event->value > JOYDEAD)) {
			CPC_ClearKey(CPC_JOY1_LEFT);
			CPC_SetKey(CPC_JOY1_RIGHT);
		} else {
			CPC_ClearKey(CPC_JOY1_LEFT);
			CPC_ClearKey(CPC_JOY1_RIGHT);
		}
	}
	if (event->axis == 3) {
		/* Up-Down movement */
		if ((event->value < -JOYDEAD)) {
			CPC_SetKey(CPC_JOY1_UP);
			CPC_ClearKey(CPC_JOY1_DOWN);
		} else if ((event->value > JOYDEAD)) {
			CPC_ClearKey(CPC_JOY1_UP);
			CPC_SetKey(CPC_JOY1_DOWN);
		} else {
			CPC_ClearKey(CPC_JOY1_UP);
			CPC_ClearKey(CPC_JOY1_DOWN);
		}
	}
}

int mousex = 0;
int mousey = 0;
int mouseb = 0;
int mousebchanged = 0;

void sdl_HandleMouse(SDL_MouseMotionEvent *event) {
	if (mouseType == MOUSE_NONE)
		return;
	if (event == NULL) {
		if (mousex < 0) {
			if (mouseType == MOUSE_JOY) {
				CPC_ClearKey(CPC_KEY_JOY_LEFT);
				mousex = 0;
			}
		} else if (mousex > 0) {
			if (mouseType == MOUSE_JOY) {
				CPC_ClearKey(CPC_KEY_JOY_RIGHT);
				mousex = 0;
			}
		}
		if (mousey < 0) {
			if (mouseType == MOUSE_JOY) {
				CPC_ClearKey(CPC_KEY_JOY_UP);
				mousey = 0;
			}
		} else if (mousey > 0) {
			if (mouseType == MOUSE_JOY) {
				CPC_ClearKey(CPC_KEY_JOY_DOWN);
				mousey = 0;
			}
		}
		return;
	}
	//printf("xrel: %i, yrel: %i!\n",
	//	event->xrel,
	//	event->yrel);
	mousex = event->xrel;
	mousey = event->yrel;
	if (event->xrel < 0) {
		if (mouseType == MOUSE_JOY) {
			CPC_SetKey(CPC_KEY_JOY_LEFT);
			CPC_ClearKey(CPC_KEY_JOY_RIGHT);
		}
	} else if (event->xrel > 0) {
		if (mouseType == MOUSE_JOY) {
			CPC_SetKey(CPC_KEY_JOY_RIGHT);
			CPC_ClearKey(CPC_KEY_JOY_LEFT);
		}
	}
	if (event->yrel < 0) {
		if (mouseType == MOUSE_JOY) {
			CPC_SetKey(CPC_KEY_JOY_UP);
			CPC_ClearKey(CPC_KEY_JOY_DOWN);
		}
	} else if (event->yrel > 0) {
		if (mouseType == MOUSE_JOY) {
			CPC_SetKey(CPC_KEY_JOY_DOWN);
			CPC_ClearKey(CPC_KEY_JOY_UP);
		}
	}
}

BOOL sdl_ProcessSystemEvents() {
	SDL_Event event;
	while (SDL_PollEvent(&event)) {
		switch (event.type) {
		case SDL_QUIT:
			printf("Oh! SDL_Quit\n");
			return TRUE;

		case SDL_KEYDOWN:
		case SDL_KEYUP: {
			switch (event.key.keysym.sym) {
			case SDLK_ESCAPE:	// menu button on Trimui
				mmenu = dlopen("libmmenu.so", RTLD_LAZY);

				if (mmenu) {
					ShowMenu_t ShowMenu = (ShowMenu_t) dlsym(mmenu, "ShowMenu");

					int8_t savename[512];
					strcpy(savename, rom_path);
					strcpy(strrchr(savename, '.'), ".%i.sna");
					snprintf(save_path, 512, "%s%s", SAVE_DIRECTORY,
							strrchr(savename, '/') + 1);

					SDL_PauseAudio(1);
//							MenuReturnStatus status = ShowMenu(rom_path, save_path, screen, kMenuEventKeyDown);
					MenuReturnStatus status = ShowMenu(rom_path, save_path,
							screen, kMenuEventKeyDown);

					if (status == kStatusExitGame) {
						menu_Exit();
						return TRUE;
					} else if (status == kStatusOpenMenu) {
						menu_MainShow(&gui_MainMenu);
					} else if (status >= kStatusLoadSlot) {
						int slot = status - kStatusLoadSlot;

						char filename[512];
						snprintf(filename, 512, save_path, slot);

						if (!GenericInterface_LoadSnapshot(filename)) {
							printf(Messages[89], filename);
						}
					} else if (status >= kStatusSaveSlot) {
						int slot = status - kStatusSaveSlot;

						char filename[512];
						int nSize = 128;
						snprintf(filename, 512, save_path, slot);

						if (!GenericInterface_SnapshotSave(filename, 3,
								nSize)) {
							printf(Messages[90], filename);
						}

					}
					SDL_PauseAudio(0);
				} else {
					menu_MainShow(&gui_MainMenu);
				}
				break;
			default:
				break;
			}
			HandleKey((SDL_KeyboardEvent *) &event);
		}
			break;

		case SDL_JOYAXISMOTION: /* Handle Joystick Motion */
			HandleJoy((SDL_JoyAxisEvent *) &event);
			break;

		case SDL_JOYBUTTONDOWN: /* Handle Joystick Buttons */
			if (event.jbutton.button == 0) {
				CPC_SetKey(CPC_KEY_JOY_FIRE1);
			} else if (event.jbutton.button == 2) {
				CPC_SetKey(CPC_KEY_JOY_FIRE2);
			}
			break;

		case SDL_JOYBUTTONUP: /* Handle Joystick Buttons */
			if (event.jbutton.button == 0) {
				CPC_ClearKey(CPC_KEY_JOY_FIRE1);
			} else if (event.jbutton.button == 2) {
				CPC_ClearKey(CPC_KEY_JOY_FIRE2);
			}
			break;

		case SDL_MOUSEMOTION: /* Handle Mouse Motion */
			sdl_HandleMouse((SDL_MouseMotionEvent *) &event);
			break;

		case SDL_MOUSEBUTTONDOWN: /* Handle Mouse Buttons */
			if (mouseType == MOUSE_NONE)
				break;
			if (event.button.button == SDL_BUTTON_LEFT) {
				if (mouseType == MOUSE_JOY) {
					CPC_SetKey(CPC_KEY_JOY_FIRE1);
				} else if (mouseType == MOUSE_SYMBI) {
					mouseb |= SYMBIMOUSE_BL;
					mousebchanged |= SYMBIMOUSE_BL;
				}
			} else if (event.button.button == SDL_BUTTON_RIGHT) {
				if (mouseType == MOUSE_JOY) {
					CPC_SetKey(CPC_KEY_JOY_FIRE2);
				} else if (mouseType == MOUSE_SYMBI) {
					mouseb |= SYMBIMOUSE_BR;
					mousebchanged |= SYMBIMOUSE_BR;
				}
			}
			break;

		case SDL_MOUSEBUTTONUP: /* Handle Mouse Buttons */
			if (mouseType == MOUSE_NONE)
				break;
			if (event.button.button == SDL_BUTTON_LEFT) {
				if (mouseType == MOUSE_JOY) {
					CPC_ClearKey(CPC_KEY_JOY_FIRE1);
				} else if (mouseType == MOUSE_SYMBI) {
					mouseb &= ~SYMBIMOUSE_BL;
					mousebchanged |= SYMBIMOUSE_BL;
				}
			} else if (event.button.button == SDL_BUTTON_RIGHT) {
				if (mouseType == MOUSE_JOY) {
					CPC_ClearKey(CPC_KEY_JOY_FIRE2);
				} else if (mouseType == MOUSE_SYMBI) {
					mouseb &= ~SYMBIMOUSE_BR;
					mousebchanged |= SYMBIMOUSE_BR;
				}
			}
			break;

		default:
			break;
		}
	}

	return FALSE;
}

unsigned char symbimouseReadPort(Z80_WORD Port) {
	int ret = 0;
	if (mouseType == MOUSE_SYMBI) {
		if (mousex != 0) {
			//fprintf(stderr,"x: %i, %i\n", mousex, intto6bitsigned(mousex));
			ret = (SYMBIMOUSE_X | intto6bitsigned(mousex));
			mousex = 0;
		} else if (mousey != 0) {
			//fprintf(stderr,"y: %i, %i\n", mousey, intto6bitsigned(mousey));
			ret = (SYMBIMOUSE_Y | intto6bitsigned(-mousey));
			mousey = 0;
		} else if (mousebchanged != 0) {
			ret = (SYMBIMOUSE_BUTTONS | mouseb);
			mousebchanged = 0;
		}
	}
	return ret;
}

CPCPortRead symbimousePortRead = { 0xfd10, 0xfd10, &symbimouseReadPort };

void sdl_InitialiseJoysticks() {
	int numJoys = 0;
	CPC_InstallReadPort(&symbimousePortRead);	// FIXME

	numJoys = SDL_NumJoysticks();
	fprintf(stderr, Messages[88], numJoys);
	if (numJoys > 0) {
		SDL_JoystickEventState(SDL_ENABLE);
		joystick1 = SDL_JoystickOpen(0);
		if (numJoys > 1) {
			joystick2 = SDL_JoystickOpen(1);
		}
	}
}

void sdl_EnableJoysticks(BOOL state) {
	SDL_JoystickEventState((state == TRUE) ? SDL_ENABLE : SDL_DISABLE);
}

/*void	sdl_EnableMouse(BOOL state)
 {
 mouseEnable = state;
 }*/

void sdl_SetMouseType(int t) {
	mouseType = t;
}

// forward declarations
void sdl_InitialiseKeyboardMapping_qwertz();
void sdl_InitialiseKeyboardMapping_azerty();
void sdl_InitialiseKeyboardMapping_spanish();

void sdl_InitialiseKeyboardMapping(int layout) {
	int i;

	printf("sdl_InitialiseKeyboardMapping(%i)\n", layout);
	//printf("SDLK_LAST: %i 0x%04x\n", SDLK_LAST, SDLK_LAST);
	keyUnicodeFlag = 0;
	SDL_EnableUNICODE(0); /* Disable UNICODE keyboard translation */
	for (i = 0; i < SDLK_LAST; i++) {
		KeySymToCPCKey[i] = CPC_KEY_NULL;
	}

	/* International key mappings */
	KeySymToCPCKey[SDLK_0] = CPC_KEY_ZERO;
	KeySymToCPCKey[SDLK_1] = CPC_KEY_1;
	KeySymToCPCKey[SDLK_2] = CPC_KEY_2;
	KeySymToCPCKey[SDLK_3] = CPC_KEY_3;
	KeySymToCPCKey[SDLK_4] = CPC_KEY_4;
	KeySymToCPCKey[SDLK_5] = CPC_KEY_5;
	KeySymToCPCKey[SDLK_6] = CPC_KEY_6;
	KeySymToCPCKey[SDLK_7] = CPC_KEY_7;
	KeySymToCPCKey[SDLK_8] = CPC_KEY_8;
	KeySymToCPCKey[SDLK_9] = CPC_KEY_9;
	KeySymToCPCKey[SDLK_a] = CPC_KEY_A;
	KeySymToCPCKey[SDLK_b] = CPC_KEY_B;
	KeySymToCPCKey[SDLK_c] = CPC_KEY_C;
	KeySymToCPCKey[SDLK_d] = CPC_KEY_D;
	KeySymToCPCKey[SDLK_e] = CPC_KEY_E;
	KeySymToCPCKey[SDLK_f] = CPC_KEY_F;
	KeySymToCPCKey[SDLK_g] = CPC_KEY_G;
	KeySymToCPCKey[SDLK_h] = CPC_KEY_H;
	KeySymToCPCKey[SDLK_i] = CPC_KEY_I;
	KeySymToCPCKey[SDLK_j] = CPC_KEY_J;
	KeySymToCPCKey[SDLK_k] = CPC_KEY_K;
	KeySymToCPCKey[SDLK_l] = CPC_KEY_L;
	KeySymToCPCKey[SDLK_m] = CPC_KEY_M;
	KeySymToCPCKey[SDLK_n] = CPC_KEY_N;
	KeySymToCPCKey[SDLK_o] = CPC_KEY_O;
	KeySymToCPCKey[SDLK_p] = CPC_KEY_P;
	KeySymToCPCKey[SDLK_q] = CPC_KEY_Q;
	KeySymToCPCKey[SDLK_r] = CPC_KEY_R;
	KeySymToCPCKey[SDLK_s] = CPC_KEY_S;
	KeySymToCPCKey[SDLK_t] = CPC_KEY_T;
	KeySymToCPCKey[SDLK_u] = CPC_KEY_U;
	KeySymToCPCKey[SDLK_v] = CPC_KEY_V;
	KeySymToCPCKey[SDLK_w] = CPC_KEY_W;
	KeySymToCPCKey[SDLK_x] = CPC_KEY_X;
	KeySymToCPCKey[SDLK_y] = CPC_KEY_Y;
	KeySymToCPCKey[SDLK_z] = CPC_KEY_Z;
	KeySymToCPCKey[SDLK_SPACE] = CPC_KEY_SPACE;
	KeySymToCPCKey[SDLK_COMMA] = CPC_KEY_COMMA;
	KeySymToCPCKey[SDLK_PERIOD] = CPC_KEY_DOT;
	KeySymToCPCKey[SDLK_SEMICOLON] = CPC_KEY_COLON;
	KeySymToCPCKey[SDLK_MINUS] = CPC_KEY_MINUS;
	KeySymToCPCKey[SDLK_EQUALS] = CPC_KEY_HAT;
	KeySymToCPCKey[SDLK_LEFTBRACKET] = CPC_KEY_AT;
	KeySymToCPCKey[SDLK_RIGHTBRACKET] = CPC_KEY_OPEN_SQUARE_BRACKET;

	KeySymToCPCKey[SDLK_TAB] = CPC_KEY_TAB;
	KeySymToCPCKey[SDLK_RETURN] = CPC_KEY_RETURN;
	KeySymToCPCKey[SDLK_BACKSPACE] = CPC_KEY_DEL;
	KeySymToCPCKey[SDLK_ESCAPE] = CPC_KEY_ESC;

	//KeySymToCPCKey[SDLK_Equals & 0x0ff)] = CPC_KEY_CLR;

	KeySymToCPCKey[SDLK_UP] = CPC_KEY_CURSOR_UP;
	KeySymToCPCKey[SDLK_DOWN] = CPC_KEY_CURSOR_DOWN;
	KeySymToCPCKey[SDLK_LEFT] = CPC_KEY_CURSOR_LEFT;
	KeySymToCPCKey[SDLK_RIGHT] = CPC_KEY_CURSOR_RIGHT;

	KeySymToCPCKey[SDLK_KP0] = CPC_KEY_F0;
	KeySymToCPCKey[SDLK_KP1] = CPC_KEY_F1;
	KeySymToCPCKey[SDLK_KP2] = CPC_KEY_F2;
	KeySymToCPCKey[SDLK_KP3] = CPC_KEY_F3;
	KeySymToCPCKey[SDLK_KP4] = CPC_KEY_F4;
	KeySymToCPCKey[SDLK_KP5] = CPC_KEY_F5;
	KeySymToCPCKey[SDLK_KP6] = CPC_KEY_F6;
	KeySymToCPCKey[SDLK_KP7] = CPC_KEY_F7;
	KeySymToCPCKey[SDLK_KP8] = CPC_KEY_F8;
	KeySymToCPCKey[SDLK_KP9] = CPC_KEY_F9;

	KeySymToCPCKey[SDLK_KP_PERIOD] = CPC_KEY_FDOT;

	KeySymToCPCKey[SDLK_LSHIFT] = CPC_KEY_SHIFT;
	KeySymToCPCKey[SDLK_RSHIFT] = CPC_KEY_SHIFT;
	KeySymToCPCKey[SDLK_LCTRL] = CPC_KEY_CONTROL;
	KeySymToCPCKey[SDLK_RCTRL] = CPC_KEY_CONTROL;
	KeySymToCPCKey[SDLK_CAPSLOCK] = CPC_KEY_CAPS_LOCK;

	KeySymToCPCKey[SDLK_KP_ENTER] = CPC_KEY_SMALL_ENTER;

	KeySymToCPCKey[SDLK_DELETE] = CPC_KEY_JOY_LEFT;
	KeySymToCPCKey[SDLK_END] = CPC_KEY_JOY_DOWN;
	KeySymToCPCKey[SDLK_PAGEDOWN] = CPC_KEY_JOY_RIGHT;
	KeySymToCPCKey[SDLK_INSERT] = CPC_KEY_JOY_FIRE1;
	KeySymToCPCKey[SDLK_HOME] = CPC_KEY_JOY_UP;
	KeySymToCPCKey[SDLK_PAGEUP] = CPC_KEY_JOY_FIRE2;

	KeySymToCPCKey[0x0134] = CPC_KEY_COPY; /* Alt */
	KeySymToCPCKey[0x0137] = CPC_KEY_COPY; /* Compose */

	switch (layout) {
	case QWERTZ:
		sdl_InitialiseKeyboardMapping_qwertz();
		break;
	case AZERTY:
		sdl_InitialiseKeyboardMapping_azerty();
		break;
	case SPANISH:
		sdl_InitialiseKeyboardMapping_spanish();
		break;
	}
}

void sdl_InitialiseKeyboardMapping_qwertz() {
	/* German key mappings */
	KeySymToCPCKey[0x00fc] = CPC_KEY_AT; /* ue */
	KeySymToCPCKey[0x002b] = CPC_KEY_OPEN_SQUARE_BRACKET; /* Plus */
	KeySymToCPCKey[0x00f6] = CPC_KEY_COLON; /* oe */
	KeySymToCPCKey[0x00e4] = CPC_KEY_SEMICOLON; /* ae */
	KeySymToCPCKey[0x0023] = CPC_KEY_CLOSE_SQUARE_BRACKET; /* Hash */
	KeySymToCPCKey[0x00df] = CPC_KEY_MINUS; /* sz */
	KeySymToCPCKey[0x00b4] = CPC_KEY_HAT; /* Accent */
	KeySymToCPCKey[0x005e] = CPC_KEY_CLR; /* Hat */
	KeySymToCPCKey[0x003c] = CPC_KEY_FORWARD_SLASH; /* Less */

	/* The next one might break US keyboards?!? */
	KeySymToCPCKey[SDLK_MINUS] = CPC_KEY_BACKSLASH;
}

void sdl_InitialiseKeyboardMapping_azerty() {
	// Ajout Ramlaid
	KeySymToCPCKey[SDLK_LALT] = CPC_KEY_COPY;

	KeySymToCPCKey[SDLK_AMPERSAND] = CPC_KEY_1;
	KeySymToCPCKey[SDLK_WORLD_73] = CPC_KEY_2;
	KeySymToCPCKey[SDLK_QUOTEDBL] = CPC_KEY_3;
	KeySymToCPCKey[SDLK_QUOTE] = CPC_KEY_4;
	KeySymToCPCKey[SDLK_LEFTPAREN] = CPC_KEY_5;
	KeySymToCPCKey[SDLK_MINUS] = CPC_KEY_6;
	KeySymToCPCKey[SDLK_WORLD_72] = CPC_KEY_7;
	KeySymToCPCKey[SDLK_UNDERSCORE] = CPC_KEY_8;
	KeySymToCPCKey[SDLK_WORLD_71] = CPC_KEY_9;
	KeySymToCPCKey[SDLK_WORLD_64] = CPC_KEY_ZERO;

	KeySymToCPCKey[SDLK_RIGHTPAREN] = CPC_KEY_MINUS;
	KeySymToCPCKey[SDLK_EQUALS] = CPC_KEY_HAT;
	KeySymToCPCKey[SDLK_CARET] = CPC_KEY_AT;
	KeySymToCPCKey[SDLK_DOLLAR] = CPC_KEY_OPEN_SQUARE_BRACKET;
	KeySymToCPCKey[SDLK_WORLD_89] = CPC_KEY_SEMICOLON;
	KeySymToCPCKey[SDLK_ASTERISK] = CPC_KEY_CLOSE_SQUARE_BRACKET;
	KeySymToCPCKey[SDLK_COMMA] = CPC_KEY_COMMA;
	KeySymToCPCKey[SDLK_SEMICOLON] = CPC_KEY_DOT;
	KeySymToCPCKey[SDLK_COLON] = CPC_KEY_COLON;
	KeySymToCPCKey[SDLK_EXCLAIM] = CPC_KEY_BACKSLASH;
	KeySymToCPCKey[SDLK_LESS] = CPC_KEY_FORWARD_SLASH;
}

void sdl_InitialiseKeyboardMapping_spanish() {
	keyUnicodeFlag = -1;
	SDL_EnableUNICODE(1); /* Enable UNICODE keyboard translation */
	/* Needed for special keys of spanish keyboard */
	KeySymToCPCKey[SDLK_QUOTE] = CPC_KEY_HAT; /* Pta+0x0027 */
	KeySymToCPCKey[SDLK_WORLD_1] = CPC_KEY_CLR; /* CLR 0x00a1 */
	KeySymToCPCKey[SDLK_PLUS] = CPC_KEY_OPEN_SQUARE_BRACKET; /* [ 0x002b */
	KeySymToCPCKey[SDLK_WORLD_71] = CPC_KEY_CLOSE_SQUARE_BRACKET; /* ] 0x00e7 */
	KeySymToCPCKey[SDLK_WORLD_26] = CPC_KEY_BACKSLASH; /* / 0x00ba */
	KeySymToCPCKey[SDLK_LESS] = CPC_KEY_FORWARD_SLASH; /* \ 0x003c */
	KeySymToCPCKey[SDLK_WORLD_81] = CPC_KEY_COLON; /* : 0x00f1 */
	KeySymToCPCKey[SDLK_WORLD_20] = CPC_KEY_SEMICOLON; /* ; 0x00b4 */
	KeySymToCPCKey[SDLK_WORLD_8] = CPC_KEY_SEMICOLON; /* + 0x00a8 */
	KeySymToCPCKey[SDLK_BACKQUOTE] = CPC_KEY_AT; /* @ 0x0060 */
	KeySymToCPCKey[SDLK_CARET] = CPC_KEY_AT; /* | +0x005e */
}

