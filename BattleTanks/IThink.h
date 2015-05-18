#pragma once

struct Telegram;

class IThink
{
public:
	IThink(void) {};
	virtual ~IThink(void) {};

	virtual void think() = 0;
	virtual void handleMessage(Telegram const &telegram) = 0;
};

