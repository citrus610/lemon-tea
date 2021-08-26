#include "key.h"

bool key::justPressed()
{
	return this->pressCount == 1;
}

bool key::justRelease()
{
	return this->releaseCount == 1;
}

bool key::isPressed()
{
	return this->pressCount > 0;
}

bool key::isRelease()
{
	return this->releaseCount > 0;
}