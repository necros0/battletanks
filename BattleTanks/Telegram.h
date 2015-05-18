#pragma once

#include "Defs.h"

#include <cmath>

//Granularity of the comparator for the set collection.
TIME const SMALLEST_DELAY = 0.016f; ///< The smallest time difference between telegrams, smaller values are considered to be the same time.

struct Telegram
{
public:
	EID sender;
	EID receiver;
	int msg;
	TIME dispatchTime;

	//TODO: What is mutable...
	mutable void *info;

	///////////////////////////////////
	//From Kevin's PaddleBaddle example
	Telegram(TIME dispatchTime, EID sender, EID receiver, int msg, void *info)
		: sender(sender), receiver(receiver), msg(msg), dispatchTime(dispatchTime), info(info)
	{}

	
	template<typename T>
	void cleanup() const
	{
		if (info == NULL)
			return;
		delete (T*)info;
		info = NULL;
	}

	template<typename T>
	T* getInfo() const
	{
		return (T *)info;
	}
	///////////////////////////////////
};


//Below is copied from PaddleBaddle example game

// These are needed so that Telegrams can be placed in an std::set (the message dispatcher, for
// example). If sort messages based on the dispatch time. When it finds two messages who dispatch
// time is within SMALLEST_DELAY, it checks to see if the messages are identical (i.e. same ids,
// info, type, etc...) 

/**********************************************************************************************//**
 * Equality operator. Two telegrams are considered equal if the sender, receiver, message
 * type, pointer address are exactly equal and if the difference in the dispatch time is less
 * than SMALLEST_DELAY.
 *
 * \param	left 	The first instance to compare.
 * \param	right	The second instance to compare.
 *
 * \return	true if the parameters are considered equivalent.
 **************************************************************************************************/
inline bool operator==(Telegram const &left, Telegram const &right) {
	return ((std::abs(left.dispatchTime - right.dispatchTime) < SMALLEST_DELAY) &&
		left.sender == right.sender && left.receiver == right.receiver &&
		left.msg == right.msg && left.info == right.info);
}

/**********************************************************************************************//**
 * Less-than comparison operator. Compares telegrams on dispatch time (lower dispatch time is
 * the lower telegram).
 *
 * \param	left 	The first instance to compare.
 * \param	right	The second instance to compare.
 *
 * \return	true if the first parameter is less than the second.
 **************************************************************************************************/
inline bool operator<(Telegram const &left, Telegram const &right) {
	if (left == right)
		return false;
	else
		return (left.dispatchTime < right.dispatchTime);
}
