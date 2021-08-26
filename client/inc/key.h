#ifndef SFML_KEY_H
#define SFML_KEY_H

class key
{
public:
	bool justPressed();
	bool justRelease();
	bool isPressed();
	bool isRelease();

	int pressCount = 0;
	int releaseCount = 0;
private:

};

#endif