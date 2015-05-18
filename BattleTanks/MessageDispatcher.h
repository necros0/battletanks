#pragma once

#include "Telegram.h"
#include "Defs.h"

#include <set>

class MessageDispatcher
{
private:
	//Hide this for singleton
	MessageDispatcher() {};
	MessageDispatcher(MessageDispatcher const &dsp);
	MessageDispatcher &operator=(MessageDispatcher const &dsp);
public:
	~MessageDispatcher(void);
	static MessageDispatcher& MessageDispatcher::instance();

	/**********************************************************************************************//**
	 * Based on Kevin's message dispatcher
	 *
	 * \param	t				The time when the message should be sent
	 * \param	sender_id   	Identifier for the sender.
	 * \param	receiver_id 	Identifier for the receiver.
	 * \param	msg_type		Type of the message.
	 * \param [in,out]	info	If non-null, the information.
	 **************************************************************************************************/
	void dispatchMessage(TIME t, EID sender_id, EID receiver_id, int msg_type, void* info);
	//Handles the sending of all messages
	void dispatch(TIME delta);
	
	//do queued message
	void justDispatch();

private:

	void discharge(Telegram const &telegram);

	//Time when last dispatch was called
	TIME mCurrentTime;
	//Sort telegrams by dispatch time
	std::set<Telegram> mTelegrams;
};

