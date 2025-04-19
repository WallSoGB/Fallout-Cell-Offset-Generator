#pragma once

#include "Menu.hpp"

class LoadingMenu : public Menu {
public:
	static LoadingMenu* GetSingleton();
};