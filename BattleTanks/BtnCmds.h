#pragma once

#include "IBtnCmd.h"

class BtnCmdResume : public IBtnCmd
{
public:
	BtnCmdResume(void) {};
	~BtnCmdResume(void) {};

	virtual void run();
};

class BtnCmdAudio : public IBtnCmd
{
public:
	BtnCmdAudio(void) {};
	~BtnCmdAudio(void) {};

	virtual void run();
};

class BtnCmdControls : public IBtnCmd
{
public:
	BtnCmdControls(void) {};
	~BtnCmdControls(void) {};

	virtual void run();
};

class BtnCmdSave : public IBtnCmd
{
public:
	BtnCmdSave(void) {};
	~BtnCmdSave(void) {};

	virtual void run();
};

class BtnCmdLoad : public IBtnCmd
{
public:
	BtnCmdLoad(void) {};
	~BtnCmdLoad(void) {};

	virtual void run();
};

class BtnCmdQuit : public IBtnCmd
{
public:
	BtnCmdQuit(void) {};
	~BtnCmdQuit(void) {};

	virtual void run();
};

class BtnCmdNewGame : public IBtnCmd
{
public:
	BtnCmdNewGame(void) {};
	~BtnCmdNewGame(void) {};

	virtual void run();
};

class BtnCmdNewOpt : public IBtnCmd
{
public:
	BtnCmdNewOpt(void) {};
	~BtnCmdNewOpt(void) {};

	virtual void run();
};

class BtnCmdBack : public IBtnCmd
{
public:
	BtnCmdBack(void) {};
	~BtnCmdBack(void) {};

	virtual void run();
};

class BtnCmdPathingDemo : public IBtnCmd
{
public:
	BtnCmdPathingDemo(void) {};
	~BtnCmdPathingDemo(void) {};

	virtual void run();
};