#include "game.h"

game::game()
{
	this->window = nullptr;
}

game::~game()
{
	delete this->window;
}

void game::start()
{
	this->window = new sf::RenderWindow(sf::VideoMode(960, 540), " ");
	//this->window = new sf::RenderWindow(sf::VideoMode(1920, 1080), " ");
	this->canvas.create(1920, 1080);
	this->window->setKeyRepeatEnabled(true);
	this->window->setFramerateLimit(60);
	this->window->setVerticalSyncEnabled(true);

	this->running = true;

	this->load();

	while (this->running)
	{
		this->poll_event();
		this->delta_time = clock.restart().asSeconds();
		this->update(this->delta_time);
		this->window->clear();
		this->render(); // user's function
		this->canvas.display();
		const sf::Texture& canvas_texture = this->canvas.getTexture();
		this->sprite.setTexture(canvas_texture);
		float sprite_scale = 0.0f; // find the scale of the main canvas
		if ((float)((float)this->window->getView().getSize().x / (float)this->window->getView().getSize().y) >= (float)((float)16 / (float)9)) {
			sprite_scale = (float)this->window->getView().getSize().y / 1080;
		}
		else {
			sprite_scale = (float)this->window->getView().getSize().x / 1920;
		}
		this->sprite.setScale(sprite_scale, sprite_scale);
		this->sprite.setPosition((this->window->getView().getSize().x - 1920 * sprite_scale) / 2, (this->window->getView().getSize().y - 1080 * sprite_scale) / 2);
		this->window->draw(this->sprite);
		this->window->display();
	}

	this->unload();
	this->window->close();
}

void game::end()
{
	this->running = false;
}

void game::poll_event()
{
	while (this->window->pollEvent(this->event))
	{
		switch (this->event.type)
		{
		case sf::Event::Closed:
			this->running = false;
			break;
			// catch the resize events
		case sf::Event::Resized:
			// update the view to the new size of the window
			sf::FloatRect visibleArea(0.0f, 0.0f, event.size.width, event.size.height);
			this->window->setView(sf::View(visibleArea));
			break;
		}
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) {
		input::up.pressCount++;
		input::up.releaseCount = 0;
	}
	else {
		input::up.releaseCount++;
		input::up.pressCount = 0;
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) {
		input::down.pressCount++;
		input::down.releaseCount = 0;
	}
	else {
		input::down.releaseCount++;
		input::down.pressCount = 0;
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
		input::right.pressCount++;
		input::right.releaseCount = 0;
	}
	else {
		input::right.releaseCount++;
		input::right.pressCount = 0;
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
		input::left.pressCount++;
		input::left.releaseCount = 0;
	}
	else {
		input::left.releaseCount++;
		input::left.pressCount = 0;
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space)) {
		input::space.pressCount++;
		input::space.releaseCount = 0;
	}
	else {
		input::space.releaseCount++;
		input::space.pressCount = 0;
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::C)) {
		input::c.pressCount++;
		input::c.releaseCount = 0;
	}
	else {
		input::c.releaseCount++;
		input::c.pressCount = 0;
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::X)) {
		input::x.pressCount++;
		input::x.releaseCount = 0;
	}
	else {
		input::x.releaseCount++;
		input::x.pressCount = 0;
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Z)) {
		input::z.pressCount++;
		input::z.releaseCount = 0;
	}
	else {
		input::z.releaseCount++;
		input::z.pressCount = 0;
	}
}

bool game::is_running()
{
	return this->running;
}

void game::load()
{

}

void game::update(double dt)
{

}

void game::render()
{

}

void game::unload()
{

}

