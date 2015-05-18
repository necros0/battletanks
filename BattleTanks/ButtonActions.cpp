/*


#include "ButtonActions.h"
#include "Button.h"

void ButtonMouseListener::mouseHover(sf::Event& e)
{
	std::cout << "mouseHover" << std::endl;
	self.shape.setFillColor(COLOR_HOVER);
}

void ButtonMouseListener::mouseHoverOut(sf::Event& e)
{
	std::cout << "mouseHoverOut" << std::endl;
	self.shape.setFillColor(COLOR_IDLE);
}

void ButtonMouseListener::mousePressed(sf::Event& e)
{
	std::cout << "mousePressed" << std::endl;
	self.shape.setFillColor(COLOR_PRESSED);
}

void ButtonMouseListener::mouseClick(sf::Event& e)
{
	std::cout << "mouseClick" << std::endl;
	self.shape.setFillColor(COLOR_HOVER);
	//send this frame, but not instantly so that listeners have time to loop through everything else
	MessageDispatcher::instance().dispatchMessage(1, -1, -1, MSG_MENU_RESUME, NULL);
}

*/