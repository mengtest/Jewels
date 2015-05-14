#ifndef JEWEL_H
#define JEWEL_H

#include "cocos2d.h"

USING_NS_CC;

class Jewel : public Sprite
{
public:
	static Jewel* createByType(int type, int x, int y);
	bool init(int type, int x, int y);

public:
	int getType() { return m_type; }
	int getX() { return m_x; }
	int getY() { return m_y; }

	void showSelection(); //显现边框
	void hideSelection(); //隐藏边框

	//上下左右移动一格
	void moveUp();
	void moveDown();
	void moveLeft();
	void moveRight();

private:
	int m_type;
	Sprite* m_selection; //触摸宝石上面显示的边框

	//坐标
	int m_x;
	int m_y;
};

#endif
