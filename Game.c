#include "gameImport.h"

object * mouseObj;
bool * r;

bool monkStart(bool * running) {
    r = running;
    mouseObj = createObject(0, 0, OBJ_MASK_OBJ, OBJ_TYPE_OBJECT, mouseObj, NULL);
    if (!createArea(1, 1, 0, AREA_L_MASK(1), mouseObj, NULL)) reportError();
    
    renderMetasprite * cursor = createMetaFromSheet("Sprites/cursor.bmp", -1, 2, 2, -1, -1, -1, -1, 0, 0, mouseObj, 0);
    addMetaToRender(cursor);

    setObjectInstanceID(mouseObj, "cursor");

    createTrigger("Exit", SDL_SCANCODE_ESCAPE);
    
    return true;
}

bool monkUpdate() {

    if (getTrigger("Exit").value == 1) {
            *r = false;
    }

    mouseObj->localX = getMouse().x * getWindowRatioX();
    mouseObj->localY = getMouse().y * getWindowRatioY();
    return true;
}

bool monkOnLogoUpdate() {

    return true;
}

bool monkExit() {

    return true;
}

