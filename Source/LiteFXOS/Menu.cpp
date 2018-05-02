/**************************************************************************/
/*!
    @file     Menu.cpp
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

	@brief A Menu is a State and a Mode.
			Used for navigating between function mappings to buttons/events.
*/
/**************************************************************************/
#include "Menu.h"

MENU_T	*	MenuSelect;

MENU_T * Menu_GetMenu()
{
	return MenuSelect;
}

void Menu_SetMenu(MENU_T * target)
{
	MenuSelect = target;
}

void Menu_SetNextMenu()
{
	if(MenuSelect->NextMenu) MenuSelect = MenuSelect->NextMenu;
}

void Menu_StartMenu(MENU_T * target)
{
	MenuSelect = target;
	if(MenuSelect->InitFunction) MenuSelect->InitFunction();
}


void Menu_StartNextMenu()
{
	if(MenuSelect->NextMenu)
	{
		MenuSelect = MenuSelect->NextMenu;
		if(MenuSelect->InitFunction)	MenuSelect->InitFunction();
	}
}

void Menu_DoFunction(uint8_t num)
{
	if (MenuSelect->FunctionMap[num]) MenuSelect->FunctionMap[num]();
}

void Menu_DoMenuFunction(MENU_T * target, uint8_t num)
{
	if (target->FunctionMap[num]) target->FunctionMap[num]();
}
