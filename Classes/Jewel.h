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
	void showSelection(); //���ֱ߿�
	void hideSelection(); //���ر߿�

private:
	int m_type;
	Sprite* m_selection; //������ʯ������ʾ�ı߿�
};

#endif
