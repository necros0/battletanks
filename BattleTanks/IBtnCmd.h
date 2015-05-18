#pragma once


class IBtnCmd
{
public:
	IBtnCmd(void) {};
	virtual ~IBtnCmd(void) {};

	//the command to perform
	virtual void run() = 0;
};

