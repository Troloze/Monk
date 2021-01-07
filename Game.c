#include "gameImport.h"

object * mouseObj, * fishR, * fishL;
renderPalette * buttonPalette1, * buttonPalette2;
renderMetasprite * cursor, * seaBG;
audioChunk * bgm, * click, * fishSFX;
Uint32 killCount = 0;
char * printCount;
bool * r, l = false;

bool onFishCol(void * self, void * overlap);
bool FishStart(void * self);
bool lFishUpdate(void * self);
bool rFishUpdate(void * self);

bool monkStart(bool * running) {
    r = running;
    bgm = audioLoadChunk("Audio/BGM.wav", 64);
    click = audioLoadChunk("Audio/Fish.wav", 64);
    fishSFX = audioLoadChunk("Audio/click.wav", 64);
    audioCreateChannels(15);
    
    mouseObj = createObject(0, 0, OBJ_MASK_OBJ, OBJ_TYPE_OBJECT, mouseObj, NULL);
    createArea(1, 1, 0, AREA_L_MASK(2), mouseObj, NULL);
    cursor = createMetaFromSheet("Sprites/cursor.bmp", -1, 2, 2, -1, -1, -1, -1, 0, 0, mouseObj, 0);
    setObjectInstanceID(mouseObj, "cursor");

    seaBG = createMetaFromSheet("Sprites/Game/SeaBG.bmp", -1, 32, 32, -1, -1, -1, -1, 0, 0, NULL, 50);

    fishL = createObject( 0, 0, OBJ_MASK_OBJ, OBJ_TYPE_OBJECT, fishL, "Fish");
    createMetaFromSheet("Sprites/Game/FishB.bmp", -1, 4, 2, -1, -1, -1, -1, 0, 0, fishL, 10);
    createArea(32, 16, 2, AREA_L_MASK(0), fishL, NULL);
    addFuncToObj(fishL, FishStart, lFishUpdate, NULL);
    
    fishR = createObject( 0, 0, OBJ_MASK_OBJ, OBJ_TYPE_OBJECT, fishR, "Fish");
    createMetaFromSheet("Sprites/Game/FishA.bmp", -1, 4, 2, -1, -1, -1, -1, 0, 0, fishR, 10);
    createArea(32, 16, 2, AREA_L_MASK(0), fishR, NULL);
    addFuncToObj(fishR, FishStart, rFishUpdate, NULL);
    
    addFuncToInstOverlap("cursor", "Fish", NULL, onFishCol, NULL);

    createTrigger("Exit", SDL_SCANCODE_ESCAPE);

    loadFile("SavedData/Fish");
    killCount = atoi(readData("KillCount"));
    closeFile();

    if (killCount > 0) {
        printf("Você já havia matado %d peixes!\n", killCount);
    }

    return true;
}

bool monkAfterLogo() {
    addMetaToRender(cursor);
    addMetaToRender(seaBG);
    setBgColor(0xFF0F3B51);
    audioPlay(0, bgm, -1);
    l = true;
    return true;
}

bool monkUpdate(Uint32 c) {

    if (c%60 == 0 && l) {
        if (rand()%2) {
            instanceObject(fishR,-32, 32 + rand()%192);
        } 
        else {
            instanceObject(fishL, 256, 32 + rand()%192);
        } 
    }

    if (l && getMouse().leftButtonState == 1) {
        audioPlay(-1, click, 0);
    }

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
    destroyObject(cursor->obj);
    destroyObject(seaBG->obj);
    saveFile("SavedData/Fish");
    if (killCount > 0) {
        printCount = malloc(sizeof(char) * ((int)(log10(killCount) + 1)));
        sprintf(printCount, "%d", killCount);
        writeData("KillCount",printCount);
        free(printCount);
    }
    else {
        writeData("KillCount", "0");
    }
    closeFile();
    printf("Você matou %d peixe%s no total!\n", killCount, (killCount == 1) ? "" : "s");
    return true;
}

bool FishStart(void * self) {
    addMetaToRender(((object*)((object*) self)->children[0])->item);
    return true;
}

bool lFishUpdate(void * self) {
    if (!((object*)self)->isInst) return true;
    ((object*)self)->localX -= 2;
    if (((object*)self)->localX < -32) destroyObject(self);
    return true;
}

bool rFishUpdate(void * self) {
    if (!((object*)self)->isInst) return true;
    ((object*)self)->localX += 2;
    if (((object*)self)->localX > 256) destroyObject(self);
    return true;
}

bool onFishCol(void * self, void * overlap) {
    if (!((object*)overlap)->isInst) return true;
    if (getMouse().leftButtonState == 1) {
        destroyObject(((object*)overlap)->parent);
        audioPlay( -1, fishSFX, 0);
        killCount++;
    }
    return true;
}
