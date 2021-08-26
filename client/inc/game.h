#ifndef SFML_GAME_H
#define SFML_GAME_H

#include <stdlib.h>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/System.hpp>

#include "input.h"

class game
{
public:
	game();
	virtual ~game();

	void start();
	void end();

	virtual void load();
	virtual void poll_event();
	virtual void update(double dt);
	virtual void render();
	virtual void unload();

	bool is_running();

	sf::RenderTexture canvas;

private:
	sf::RenderWindow* window;
	sf::Event event;
	sf::Clock clock;

	sf::Sprite sprite;

	double delta_time = 0.0;

	bool running = false;
};

#endif