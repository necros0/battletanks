#include "MenuItem.h"
#include "MouseListener.h"

#include <iostream>

MenuItem::MenuItem(MouseListener* listener) :
	mouseListener(listener),
	mIsActive(true)
{
}

MenuItem::~MenuItem(void)
{
	delete(mouseListener);
}

void MenuItem::setActive(bool b)
{
	mIsActive = b;
}