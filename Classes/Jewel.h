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
	//根据对象的x，y轴值，设置其游戏实际像素位置。网格坐标轴以左上角为原点，x右y下为正轴
	void setPos(float x, float y) { this->setPosition(x * 40, 480 - y * 40); }  
	int getType() { return m_type; }

private:
	int m_type;
};

#endif
