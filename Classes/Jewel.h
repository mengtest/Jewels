#ifndef JEWEL_H
#define JEWEL_H

#include "cocos2d.h"

USING_NS_CC;

class Jewel : public Sprite
{
public:
	static Jewel* createByType(int type);
	bool init(int type);

public:
	int getType() { return m_type; }
	void showSelection(); //显现边框
	void hideSelection(); //隐藏边框

private:
	int m_type;
	Sprite* m_selection; //触摸宝石上面显示的边框
};

#endif
