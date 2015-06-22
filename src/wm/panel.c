#include <abi.h>
#include <fudge.h>
#include "box.h"
#include "draw.h"
#include "panel.h"

void panel_draw(struct panel *panel, struct ctrl_videosettings *settings, unsigned int line)
{

    if (line < panel->size.y || line >= panel->size.y + panel->size.h)
        return;

    if (line == panel->size.y + 0 || line >= panel->size.y + panel->size.h - 1)
    {

        fill(settings, WM_COLOR_DARK, panel->size.x, panel->size.w);

    }

    else if (line == panel->size.y + 1 || line >= panel->size.y + panel->size.h - 2)
    {

        fill(settings, WM_COLOR_DARK, panel->size.x + 0, 1);
        fill(settings, WM_COLOR_DARK, panel->size.x + panel->size.w - 1, 1);
        fill(settings, panel->active ? WM_COLOR_ACTIVEFRAME : WM_COLOR_PASSIVEFRAME, panel->size.x + 1, panel->size.w - 2);

    }

    else
    {

        fill(settings, WM_COLOR_DARK, panel->size.x + 0, 1);
        fill(settings, WM_COLOR_DARK, panel->size.x + panel->size.w - 1, 1);
        fill(settings, panel->active ? WM_COLOR_ACTIVEFRAME : WM_COLOR_PASSIVEFRAME, panel->size.x + 1, 1);
        fill(settings, panel->active ? WM_COLOR_ACTIVEFRAME : WM_COLOR_PASSIVEFRAME, panel->size.x + panel->size.w - 2, 1);
        fill(settings, panel->active ? WM_COLOR_ACTIVEBACK : WM_COLOR_PASSIVEBACK, panel->size.x + 2, panel->size.w - 4);

    }

}

void panel_init(struct panel *panel)
{

    box_setsize(&panel->size, 0, 0, 0, 0);

    panel->active = 0;

}
