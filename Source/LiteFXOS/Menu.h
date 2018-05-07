/**************************************************************************/
/*!
    @file     Menu.h
    @author   The Firebrand Forge

    @section LICENSE

    Copyright (C) 2018 The Firebrand Forge Inc

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/
/**************************************************************************/
#ifndef MENU_H
#define MENU_H

#define MENU_FUNCTIONS_COUNT 10

typedef const struct Menu
{
	const struct Menu *	NextMenu;
	const struct Menu *	PrevMenu;
	void (* const InitFunction)(void);
	//Can include other menu data here later;
	void (* const FunctionMap[MENU_FUNCTIONS_COUNT])(void);
	//void 	(** FunctionMap)(void);
} MENU_T;

extern MENU_T * Menu_GetMenu(void);
extern void Menu_SetMenu(MENU_T * target);
extern void Menu_StartMenu(MENU_T * target);
extern void Menu_SetNextMenu(void);
extern void Menu_StartNextMenu(void);
extern void Menu_DoFunction(uint8_t num);

#endif
