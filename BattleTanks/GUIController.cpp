#include "GUIController.h"
#include "Telegram.h"
#include "MessageDefs.h"
#include "Menu.h"
#include "Button.h"
#include "Slider.h"
#include "BtnCmds.h"
#include "MessageDispatcher.h"
#include "MessageDefs.h"
#include "Game.h"
#include "Tank.h"

#include <iostream>
#include <sstream>

GUIController::GUIController(void) :
	mTank(NULL)
{
}


GUIController::~GUIController(void)
{
	clearMenus();
}

GUIController& GUIController::instance()
{
	static GUIController instance;
	return instance;
}

void GUIController::draw(sf::RenderTarget& target)
{
	monitorTank(target); //Show HUD info

	//No menu to draw
	if (mMenuStack.empty())
		return;


	mMenuStack.top()->draw(target);
}

void GUIController::monitorTank(sf::RenderTarget& target)
{
	if (!mTank) //no tank to monitor
		return;

	if (mTank->mItems & Tank::IT_KINETIC)
	{
		Vec2 pos;
		if (mTank->mShieldPower > 0)
			pos = Vec2(8.0f, 140.0f);
		else
			pos = Vec2(8.0f, 72.0f);

		sf::RectangleShape mIcon;
		mIcon.setPosition(pos.toScreenVec());
		mIcon.setTexture(&Game::instance().Texture().get("resources/texture/kinetic.png")->getTexture());
		mIcon.setSize(Vec2(64.0f, 64.0f));
		target.draw(mIcon);
	}

	if (mTank->mShieldPower > 0)
	{
		Vec2 pos = Vec2(8.0f, 72.0f);
		sf::RectangleShape mIcon;
		mIcon.setPosition(pos.toScreenVec());
		mIcon.setTexture(&Game::instance().Texture().get("resources/texture/healing.png")->getTexture());
		mIcon.setSize(Vec2(64.0f, 64.0f));
		target.draw(mIcon);

	}

	std::stringstream ss;
	ss << mTank->mHealth;
	sf::Text mLabel(ss.str(), *Game::Font().get("resources/font/Ubuntu-M.ttf"), 16);
	//Set label's origin to below where icons appear
	mLabel.setOrigin(Vec2(mLabel.getLocalBounds().width * 0.5f, mLabel.getLocalBounds().height * 0.5f));
	mLabel.setPosition(Vec2(16.0f, 18.0f).toScreenVec());
	target.draw(mLabel);
}

bool GUIController::isMenuActive()
{
	return !mMenuStack.empty();
}

bool GUIController::handleMessage(Telegram const &telegram)
{
	if (telegram.msg == MessageType::MSG_MENU_ESC)
	{
		if (mMenuStack.empty())
			pushMenu(MenuType::MENU_OPTIONS);
		else
			popMenu();
	}
	else if (telegram.msg == MessageType::MSG_MENU_BACK)
	{
		//if (mMenuStack.size() > 1) //back doesn't let you quit, just pops once
			popMenu();
	}
	else if (telegram.msg == MessageType::MSG_MENU_RESUME)
	{
		if (mMenuStack.empty()) //already empty (how did this happen?)
			return true;

		//can't put the sound in clearMenus because clearMenus is called in the destructor
		Game::Sound().get("resources/sound/menu_cancel.wav")->play(1.0f, 1);
		clearMenus();
	}
	else if (telegram.msg == MessageType::MSG_MENU_AUDIO)
	{
		pushMenu(MenuType::MENU_AUDIO);
	}
	else if (telegram.msg == MessageType::MSG_MENU_CONTROLS)
	{
		pushMenu(MenuType::MENU_KEYS);
	}
	else if (telegram.msg == MessageType::MSG_MENU_SAVE)
	{
		pushMenu(MenuType::MENU_SAVE);
	}
	else if (telegram.msg == MessageType::MSG_MENU_LOAD)
	{
		pushMenu(MenuType::MENU_LOAD);
	}
	else if (telegram.msg == MessageType::MSG_MENU_QUIT)
	{
		clearMenus();
	}
	else if (telegram.msg == MessageType::MSG_MENU_START)
	{
		pushMenu(MenuType::MENU_START);
	}
	else if (telegram.msg == MessageType::MSG_MENU_NEWOPT)
	{
		pushMenu(MenuType::MENU_NEWOPT);
	}
	else if (telegram.msg == MessageType::MSG_MENU_WIN)
	{
		pushMenu(MenuType::MENU_WIN);
	}
	else if (telegram.msg == MessageType::MSG_MENU_LOSS)
	{
		pushMenu(MenuType::MENU_LOSS);
	}

	return true;
}

void GUIController::pushMenu(MenuType type)
{
	if(type == MenuType::MENU_OPTIONS)
	{
		Menu* menu = new Menu("Options");
		menu->addItem(new Button(Vec2(WINDOW_WIDTH * 0.5f - 100, menu->getNextItemHeight()), Vec2(200, 60), "Resume", new BtnCmdResume()));
		menu->addItem(new Button(Vec2(WINDOW_WIDTH * 0.5f - 100, menu->getNextItemHeight()), Vec2(200, 60), "Audio", new BtnCmdAudio()));
		menu->addItem(new Button(Vec2(WINDOW_WIDTH * 0.5f - 100, menu->getNextItemHeight()), Vec2(200, 60), "Controls", new BtnCmdControls()));
		menu->addItem(new Button(Vec2(WINDOW_WIDTH * 0.5f - 100, menu->getNextItemHeight()), Vec2(200, 60), "Save", new BtnCmdSave()));
		menu->addItem(new Button(Vec2(WINDOW_WIDTH * 0.5f - 100, menu->getNextItemHeight()), Vec2(200, 60), "Load", new BtnCmdLoad()));
		menu->addItem(new Button(Vec2(WINDOW_WIDTH * 0.5f - 100, menu->getNextItemHeight()), Vec2(200, 60), "Quit", new BtnCmdQuit()));
		push(menu);
	}
	else if(type == MenuType::MENU_AUDIO)
	{
		Menu* menu = new Menu("Audio");
		menu->addItem(new Button(Vec2(WINDOW_WIDTH * 0.5f - 100, menu->getNextItemHeight()), Vec2(200, 60), "Back", new BtnCmdBack()));
		menu->addItem(new Slider(Vec2(WINDOW_WIDTH * 0.5f - 100, menu->getNextItemHeight()), Vec2(200, 60), "Sound", false));
		menu->addItem(new Slider(Vec2(WINDOW_WIDTH * 0.5f - 100, menu->getNextItemHeight()), Vec2(200, 60), "Music", true));
		push(menu);
	}
	else if(type == MenuType::MENU_KEYS)
	{
		Menu* menu = new Menu("Controls");
		menu->addItem(new Button(Vec2(WINDOW_WIDTH * 0.5f - 100, menu->getNextItemHeight()), Vec2(200, 60), "Back", new BtnCmdBack()));
		push(menu);
	}
	else if(type == MenuType::MENU_SAVE)
	{
		Menu* menu = new Menu("Save Game");
		menu->addItem(new Button(Vec2(WINDOW_WIDTH * 0.5f - 100, menu->getNextItemHeight()), Vec2(200, 60), "Back", new BtnCmdBack()));
		push(menu);
	}
	else if(type == MenuType::MENU_LOAD)
	{
		Menu* menu = new Menu("Load Game");
		menu->addItem(new Button(Vec2(WINDOW_WIDTH * 0.5f - 100, menu->getNextItemHeight()), Vec2(200, 60), "Back", new BtnCmdBack()));
		push(menu);
	}
	else if(type == MenuType::MENU_START)
	{
		Menu* menu = new Menu("BATTLE TANKS");
		menu->addItem(new Button(Vec2(WINDOW_WIDTH * 0.5f - 100, menu->getNextItemHeight()), Vec2(200, 60), "New Game", new BtnCmdNewGame()));
		menu->addItem(new Button(Vec2(WINDOW_WIDTH * 0.5f - 100, menu->getNextItemHeight()), Vec2(200, 60), "Options", new BtnCmdNewOpt()));
		menu->addItem(new Button(Vec2(WINDOW_WIDTH * 0.5f - 100, menu->getNextItemHeight()), Vec2(200, 60), "Quit", new BtnCmdQuit()));
		menu->addItem(new Button(Vec2(WINDOW_WIDTH * 0.5f - 100, menu->getNextItemHeight()), Vec2(200, 60), "Pathing Demo", new BtnCmdPathingDemo()));
		//push(menu);
		mMenuStack.push(menu);
	}
	else if(type == MenuType::MENU_NEWOPT)
	{
		Menu* menu = new Menu("BATTLE TANKS");
		menu->addItem(new Button(Vec2(WINDOW_WIDTH * 0.5f - 100, menu->getNextItemHeight()), Vec2(200, 60), "Back", new BtnCmdBack()));
		menu->addItem(new Button(Vec2(WINDOW_WIDTH * 0.5f - 100, menu->getNextItemHeight()), Vec2(200, 60), "Audio", new BtnCmdAudio()));
		menu->addItem(new Button(Vec2(WINDOW_WIDTH * 0.5f - 100, menu->getNextItemHeight()), Vec2(200, 60), "Controls", new BtnCmdControls()));
		push(menu);
	}
	else if(type == MenuType::MENU_WIN)
	{
		Menu* menu = new Menu("VICTORY!");
		menu->addItem(new Button(Vec2(WINDOW_WIDTH * 0.5f - 100, menu->getNextItemHeight()), Vec2(200, 60), "Quit", new BtnCmdQuit()));
		push(menu);
	}
	else if(type == MenuType::MENU_LOSS)
	{
		Menu* menu = new Menu("DEFEAT!");
		menu->addItem(new Button(Vec2(WINDOW_WIDTH * 0.5f - 100, menu->getNextItemHeight()), Vec2(200, 60), "Quit", new BtnCmdQuit()));
		push(menu);
	}
}

void GUIController::push(Menu* m)
{
	Game::Sound().get("resources/sound/menu_ok.wav")->play(1.0f, 1);
	if (!mMenuStack.empty())
		mMenuStack.top()->setActive(false);
	mMenuStack.push(m);
}

void GUIController::popMenu()
{
	//in case...
	if (mMenuStack.empty())
		return;

	Game::Sound().get("resources/sound/menu_cancel.wav")->play(1.0f, 1);
	delete(mMenuStack.top());
	mMenuStack.pop();

	if (mMenuStack.empty())
		return;

	mMenuStack.top()->setActive(true);
}

void GUIController::clearMenus()
{
	while (!mMenuStack.empty())
	{
		delete(mMenuStack.top());
		mMenuStack.pop();
	}
}

void GUIController::setMonitoring(Tank* t)
{
	this->mTank = t;
}