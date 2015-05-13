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
	//���ݶ����x��y��ֵ����������Ϸʵ������λ�á����������������Ͻ�Ϊԭ�㣬x��y��Ϊ����
	void setPos(float x, float y) { this->setPosition(x * 40, 480 - y * 40); }  
	int getType() { return m_type; }

private:
	int m_type;
};

#endif
