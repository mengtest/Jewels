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

	void showSelection(); //���ֱ߿�
	void hideSelection(); //���ر߿�

	//���������ƶ�һ��
	void moveUp();
	void moveDown();
	void moveLeft();
	void moveRight();

private:
	int m_type;
	Sprite* m_selection; //������ʯ������ʾ�ı߿�

	//����
	int m_x;
	int m_y;
};

#endif
