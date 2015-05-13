#ifndef GAMESCENE_H
#define GAMESCENE_H

#include "cocos2d.h"

USING_NS_CC;

class Jewel;

class GameScene : public Layer
{
public:
	static Scene* createScene();
	CREATE_FUNC(GameScene);
	bool init();
};

#endif
