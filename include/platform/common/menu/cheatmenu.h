#pragma once

#include "types.h"

#include "menu.h"

class CheatsMenu : public Menu {
public:
    bool processInput(UIKey key, u32 width, u32 height);
    void draw(u32 width, u32 height);
private:
    u32 cheatsPerPage = 0;
    u32 selection = 0;
};
