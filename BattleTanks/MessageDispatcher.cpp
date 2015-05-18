#include "MessageDispatcher.h"
#include "EntityManager.h"
#include "AEntity.h"
#include "Game.h"
#include "Telegram.h"
#include "GUIController.h"

#include <iostream>

//#define CHECK_LEGAL_RECEIVER

MessageDispatcher::~MessageDispatcher(void)
{
	while (!mTelegrams.empty())
	{
		Telegram const &t = *(mTelegrams.begin());
		if (t.info)
			delete(t.info);
		mTelegrams.erase(mTelegrams.begin());
	}
}

MessageDispatcher& MessageDispatcher::instance()
{
	static MessageDispatcher instance;
	return instance;
}


/**********************************************************************************************//**
 * Register a message with the dispatcher. Sender and receiver are denoted with IDs. In order
 * for delivery to occur the receiver ID must be valid (there is no check to confirm the sender
 * ID). For instant deliver use a delay of SEND_MSG_IMMEDIATELY.
 *
 * \param	delay			The delay before the message should be delivered.
 * \param	sender_id   	Identifier for the sender.
 * \param	receiver_id 	Identifier for the receiver.
 * \param	msg_type		Type of the message.
 * \param [in,out]	info	If non-null, the information.
 **************************************************************************************************/
void MessageDispatcher::dispatchMessage(TIME t, EID sender_id, EID receiver_id, int msg_type, void* info)
{
#ifdef CHECK_LEGAL_RECEIVER
	//Verify the message has a legal receiver (this is checked when delivery is attempted).
	AEntity *receiver = EntityManager::instance().getEntityByID(receiver_id);
	//HACK: Not willing to make GUIController an Entity for now.  -1 is for GUIController
	if (receiver_id != -1 && receiver == NULL) {
		std::cout << "No receiver with ID " << receiver_id << " found. Message aborted." << std::endl;
		return;
	}
#endif

	//Create the telegram.
	Telegram telegram(0, sender_id, receiver_id, msg_type, info);
	
	//Check the delay. If there is none, send immediately, otherwise
	// add to the message queue.
	if (t == 0) //t < Game::instance().getTime())
	{
		this->discharge(telegram);
	}
	else
	{
		telegram.dispatchTime = t;
		std::pair<std::set<Telegram>::iterator,bool> ret = this->mTelegrams.insert(telegram);
		if (!ret.second)
		{
			//std::cerr << "Telegram was dropped." << std::endl;
			if (info) //clean up since the telegram is lostdd
				delete(info);
		}
	}
}

//From Kevin's PaddleBaddle example
void MessageDispatcher::discharge(Telegram const &telegram)
{
	AEntity *receiver = EntityManager::instance().getEntityByID(telegram.receiver);

	if (receiver == NULL)
	{
		if (telegram.receiver == -1) //HACK: Not willing to make GUIController an Entity for now.  -1 is for GUIController
			GUIController::instance().handleMessage(telegram);
		else if (telegram.receiver == -2) //HACK: Not willing to make Game an Entity for now.  -2 is for Game
			Game::instance().handleMessage(telegram);
	}
	else
	{//we got a receiver, handle it.
		receiver->handleMessage(telegram);
	}

	//We'll just never use an info that needs a destructor and we'll always treat telegram info objects as fleeting
	if (telegram.info != NULL)
		delete(telegram.info);

}

/**********************************************************************************************//**
 * Checks for any telegrams in the queue that need to be delivered and delivers them.
 **************************************************************************************************/
void MessageDispatcher::justDispatch() {
	//Check if the front telegram in the queue is due for delivery.
	while (!mTelegrams.empty() &&
		(mTelegrams.begin()->dispatchTime < Game::instance().getTime()) &&
		(mTelegrams.begin()->dispatchTime > 0))
	{
		//Remove the telegram from the queue and send it.
		Telegram const &t = *(mTelegrams.begin());
		discharge(t);
		mTelegrams.erase(mTelegrams.begin());
	}
}