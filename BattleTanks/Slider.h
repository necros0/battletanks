#pragma once

#include "MenuItem.h"
#include "MouseListener.h"

#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/Text.hpp>
#include <SFML/Graphics/Color.hpp>

class Slider : public MenuItem
{
public:
	Slider(Vec2 mins, Vec2 size, std::string label, bool isMusic);
	virtual ~Slider(void);

	//Draw yourself
	void draw(sf::RenderTarget& target);

private:
	sf::RectangleShape mBackgroundShape;
	sf::RectangleShape mSliderBarShape;
	sf::RectangleShape mSliderHandleShape;
	sf::Text mLabel;

	float mSliderWidth; //width of the bar
	Vec2 mSliderBarPos; //middle of the bar
	//Vec2 mSliderHandlePos;
	//Vec2 mSliderHandleSize;

	//The actual value of the slider
	float mSliderValue;
	
	//float mSliderAbsPosY;
	
	bool mIsSliderHeld;

	bool mIsMusicSlider;

	//Vec2 mSliderAbsMins;
	//Vec2 mSliderAbsMaxs;

	void setSliderHandleShapePosition();
	Vec2 getSliderHandlePosition();


	static const sf::Color COLOR_HOVER;
	static const sf::Color COLOR_IDLE;
	static const sf::Color COLOR_PRESSED;
	static const sf::Color COLOR_HANDLE_HOVER;
	static const sf::Color COLOR_HANDLE_IDLE;
	static const sf::Color COLOR_HANDLE_PRESSED;

	class SliderMouseListener : public MouseListener
	{
		friend class Button;
	public:
		SliderMouseListener(Vec2 absmin, Vec2 absmax, Slider& self) :
			MouseListener(absmin, absmax, true), //use overloaded constructor to notify this listener it is for menus
			self(self)
		{};
		
		virtual ~SliderMouseListener() {};

		virtual void mouseHover(sf::Event& e);
		virtual void mouseHoverOut(sf::Event& e);
		virtual void mousePressed(sf::Event& e);
		virtual void mouseClick(sf::Event& e);
		virtual void mouseDrag(sf::Event& e);

	private:
		Slider& self;
	};
};


