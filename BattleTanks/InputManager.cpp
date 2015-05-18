//This entire class is cop.y/pasted from Kevin Forest's PaddleBaddle example
//Only very minor changes were made

/**********************************************************************************************//**
 * \file	InputManager.cpp
 *
 * Implements the input manager class.
 **************************************************************************************************/
#include "InputManager.h"
#include "MessageDispatcher.h"

/**********************************************************************************************//**
 * Gets the singleton instance. Singleton is instantiated on first call.
 *
 * \return	An InputManager&amp;
 **************************************************************************************************/
InputManager &InputManager::instance() {
	static InputManager instance;
	return instance;
}

/**********************************************************************************************//**
 * Default constructor.
 **************************************************************************************************/
InputManager::InputManager() : m_id(-3) {

}

/**********************************************************************************************//**
 * Destructor.
 **************************************************************************************************/
InputManager::~InputManager()
{
}

/**********************************************************************************************//**
 * Registers the keymapping (a combination of a key and a state). Each key can have two
 * registrations, one for when the key is pressed and one for released.
 *
 * \param	key	   	The keyboard key.
 * \param	keyDown	true for key pressed, false for released.
 * \param	msg	   	The type of the message that should be sent.
 * \param	id	   	The ID of the entity to whom a message should be sent.
 **************************************************************************************************/
void InputManager::registerKeymapping(sf::Keyboard::Key key, bool keyDown, MessageType msg, EID id)
{
	m_keymappings[std::make_pair(key, keyDown)] = std::make_pair(msg, id);
}

/**********************************************************************************************//**
 * Deregisters any keymappings for the specified keyboard key. This removes any mappings for key
 * press and key released.
 *
 * \param	key	The keyboard key.
 **************************************************************************************************/
void InputManager::deregisterKeyMapping(sf::Keyboard::Key key)
{
	deregisterKeyMapping(key, true);
	deregisterKeyMapping(key, false);
}

/**********************************************************************************************//**
 * Deregisters the key mapping for the specified keyboard key/state combination.
 *
 * \param	key	   	The keyboard key.
 * \param	keyDown	true for key pressed, false for released.
 **************************************************************************************************/
void InputManager::deregisterKeyMapping(sf::Keyboard::Key key, bool keyDown)
{
	//std::map<std::pair<sf::Keyboard::Key, bool>, std::pair<MessageType, EID> >::iterator
	//CJ: using auto for simplicity
	auto it = m_keymappings.find(std::make_pair(key, keyDown));
	if (it != m_keymappings.end()) 
	{
		m_keymappings.erase(it);
	}
}

/**********************************************************************************************//**
 * Clears all registered mappings.
 **************************************************************************************************/
void InputManager::clearAllMappings()
{
	m_keymappings.clear();
}

/**********************************************************************************************//**
 * Checks if a key/state combination has been registered and dispatches any telegrams if a match
 * is found.
 *
 * \param	key	   	The keyboard key.
 * \param	keyDown	true if the key was pressed, false if the key was released.
 **************************************************************************************************/
void InputManager::handleEvent(sf::Keyboard::Key key, bool keyDown) {
	//Seach the map to see if the event is registered.
	//std::map<std::pair<sf::Keyboard::Key, bool>, std::pair<MessageType, EID> >::iterator 
	//CJ: using auto for simplicity
	auto it = m_keymappings.find(std::make_pair(key, keyDown));
	if (it != m_keymappings.end()) {
		//If the event has been registered send the message to the entity.
		MessageDispatcher::instance().dispatchMessage(0, m_id, it->second.second, it->second.first, NULL);
	}
}

