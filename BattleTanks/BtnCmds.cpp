#include "BtnCmds.h"
#include "MessageDispatcher.h"
#include "MessageDefs.h"
#include "Game.h"

#include <iostream>

void BtnCmdResume::run()
{
	//send this frame, but not instantly so that listeners have time to loop through everything else
	MessageDispatcher::instance().dispatchMessage(0.1f, -1, -1, MessageType::MSG_MENU_RESUME, NULL);
}

void BtnCmdAudio::run()
{
	//send this frame, but not instantly so that listeners have time to loop through everything else
	MessageDispatcher::instance().dispatchMessage(0.1f, -1, -1, MessageType::MSG_MENU_AUDIO, NULL);
}

void BtnCmdControls::run()
{
	//send this frame, but not instantly so that listeners have time to loop through everything else
	MessageDispatcher::instance().dispatchMessage(0.01f, -1, -1, MessageType::MSG_MENU_CONTROLS, NULL);
}

void BtnCmdSave::run()
{
	//send this frame, but not instantly so that listeners have time to loop through everything else
	MessageDispatcher::instance().dispatchMessage(0.01f, -1, -1, MessageType::MSG_MENU_SAVE, NULL);
}

void BtnCmdLoad::run()
{
	//send this frame, but not instantly so that listeners have time to loop through everything else
	MessageDispatcher::instance().dispatchMessage(0.01f, -1, -1, MessageType::MSG_MENU_LOAD, NULL);
}

void BtnCmdQuit::run()
{
	//send this frame, but not instantly so that listeners have time to loop through everything else
	MessageDispatcher::instance().dispatchMessage(0.01f, -1, -1, MessageType::MSG_MENU_QUIT, NULL);
	MessageDispatcher::instance().dispatchMessage(0.02f, -1, -2, MessageType::MSG_GAME_QUIT, NULL);
}

void BtnCmdNewGame::run()
{
	//send this frame, but not instantly so that listeners have time to loop through everything else
	MessageDispatcher::instance().dispatchMessage(0.01f, -1, -1, MessageType::MSG_MENU_RESUME, NULL);
	MessageDispatcher::instance().dispatchMessage(0.02, -1, -2, MessageType::MSG_GAME_START, NULL);
}

void BtnCmdNewOpt::run()
{
	//send this frame, but not instantly so that listeners have time to loop through everything else
	MessageDispatcher::instance().dispatchMessage(0.01f, -1, -1, MessageType::MSG_MENU_NEWOPT, NULL);
}

void BtnCmdBack::run()
{
	//send this frame, but not instantly so that listeners have time to loop through everything else
	MessageDispatcher::instance().dispatchMessage(0.01f, -1, -1, MessageType::MSG_MENU_BACK, NULL);
}

void BtnCmdPathingDemo::run()
{
	Game::instance().demoMode = 1;
	MessageDispatcher::instance().dispatchMessage(0.01f, -1, -1, MessageType::MSG_MENU_RESUME, NULL);
	MessageDispatcher::instance().dispatchMessage(0.02, -1, -2, MessageType::MSG_GAME_START, NULL);
}