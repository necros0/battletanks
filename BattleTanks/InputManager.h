//This entire class is cop.y/pasted from Kevin Forest's PaddleBaddle example
//Only very minor changes were made

/**********************************************************************************************//**
 * \file	InputManager.h
 *
 * Declares the input manager class.
 **************************************************************************************************/
#ifndef INPUT_MANAGER_H
#define INPUT_MANAGER_H

#include "MessageDefs.h"
#include "Defs.h"

#include <SFML/Window/Event.hpp>

#include <map>

/**********************************************************************************************//**
 * Creates a define that can be used as a global. Useful
 * if you later decide to replace your singleton with a global input manager.
 **************************************************************************************************/
#define GInputMgr (InputManager::Instance())

/**********************************************************************************************//**
 * Manages input events and notifies any entities who have registered themselves about
 * key down and key up events. Each key/state combination (presses/released) can be registered
 * exactly once. 
 *
 * \author	Kevin Forest
 * \date	20/09/2014
 **************************************************************************************************/
class InputManager
{
private:

	/**********************************************************************************************//**
	 * Singleton pattern hides constructor, cop.y constructor,
	 *  and assignment. Only constructor gets a body.
	 **************************************************************************************************/
	InputManager();
	InputManager(InputManager const &mgr); ///< Cop.y constructor (not implemented).
	InputManager& operator = (InputManager const &mgr); ///< Assignment operator (not implemented).

public:

	/**********************************************************************************************//**
	 * Gets the singleton instance. Singleton is instantiated on first call.
	 *
	 * \return	An InputManager&amp;
	 **************************************************************************************************/
	static InputManager& instance();
	~InputManager(); ///< Destructor.

	/**********************************************************************************************//**
	 * Registers the keymapping (a combination of a key and a state). Each key can have two registrations,
	 * one for when the key is pressed and one for released.
	 *
	 * \param	key	   	The keyboard key.
	 * \param	keyDown	true for key pressed, false for released.
	 * \param	msg	   	The type of the message that should be sent.
	 * \param	id	   	The ID of the entity to whom a message should be sent.
	 **************************************************************************************************/
	void registerKeymapping(sf::Keyboard::Key key, bool keyDown, MessageType msg, EID id);

	/**********************************************************************************************//**
	 * Deregisters any keymappings for the specified keyboard key. This removes any mappings for
	 * key press and key released.
	 *
	 * \param	key	The keyboard key.
	 **************************************************************************************************/
	void deregisterKeyMapping(sf::Keyboard::Key key);

	/**********************************************************************************************//**
	 * Deregisters the key mapping for the specified keyboard key/state combination.
	 *
	 * \param	key	   	The keyboard key.
	 * \param	keyDown	true for key pressed, false for released.
	 **************************************************************************************************/
	void deregisterKeyMapping(sf::Keyboard::Key key, bool keyDown);

	/**********************************************************************************************//**
	 * Clears all registered mappings.
	 **************************************************************************************************/
	void clearAllMappings();

	/**********************************************************************************************//**
	 * Checks if a key/state combination has been registered and dispatches any telegrams if
	 * a match is found.
	 *
	 * \param	key	   	The keyboard key.
	 * \param	keyDown	true if the key was pressed, false if the key was released.
	 **************************************************************************************************/
	void handleEvent(sf::Keyboard::Key key, bool keyDown);

	/**********************************************************************************************//**
	 * Gets the id used by the input manager when sending messages. Can be used to verify the sender
	 * of a telegram.
	 *
	 * \return	The ID associated with this object.
	 **************************************************************************************************/
	EID const& getID() const {
		return m_id;
	}
private:
	EID const m_id; ///< The id used when sending messages. Since the input manager is not an entity it cannot receive messages.

	/**********************************************************************************************//**
	 * The key mappings for each registered key/state pair. 
	 *
	 * \return	The m keymappings.
	 **************************************************************************************************/
	std::map<std::pair<sf::Keyboard::Key, bool>, std::pair<MessageType, EID> > m_keymappings;
};

#endif