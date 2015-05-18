#pragma once

#include "MouseListener.h"
#include "Telegram.h"

#include <SFML/Graphics/RenderTarget.hpp>

#include <stack>
#include <map>

class MouseListener;
class Menu;
class Tank;

class GUIController
{
public:
	GUIController(void);
	~GUIController(void);

	enum MenuType {
		MENU_TITLE,
		MENU_MAIN,
		MENU_OPTIONS,
		MENU_AUDIO,
		MENU_SAVE,
		MENU_LOAD,
		MENU_KEYS,
		MENU_START,
		MENU_NEWOPT,
		MENU_WIN,
		MENU_LOSS
	};

	static GUIController& instance();

	//Do menu stuff
	void draw(sf::RenderTarget& target);

	// Returns true if menu is on, false if menu is not.
	bool isMenuActive();

	//Handles telegram messages
	bool handleMessage(Telegram const &telegram);
	
	//Pushes a new menu onto the stack
	void pushMenu(MenuType);
	void push(Menu* m);

	//Removes top menu
	void popMenu();

	//remove all menus
	void clearMenus();

	//set which player to monitor
	void setMonitoring(Tank*);
	
private:
	MouseListener* mouseListener; 
	std::stack<Menu*> mMenuStack;
	Tank* mTank;

	void monitorTank(sf::RenderTarget& target);
};

