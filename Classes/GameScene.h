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

private:
	void initGrid();  //初始化网格里面的宝石
	Jewel* addAJewel(int x, int y); //根据坐标（不是像素，是以左上角为原点的坐标）给主层添加一颗宝石
	void clearAllJewels(); //清除所有保存在m_jewels里面的宝石对象

	bool isJewelLegal(Jewel* jewel, int x, int y); //判断一个宝石是否合法，即没有三连

private:
	Jewel* m_jewels[8][8]; //代表一个8行8列的网格（因为素材就只有8行8列的）,里面存放Jewel*
};

#endif
