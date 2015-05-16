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
	bool getCrushState() { return m_isCrushOver; }
	bool getMoveOverState() { return m_isMoveOver; }

	bool getSwapingState() { return m_isSwaping; }
	void setX(int x) { m_x = x; }
	void setY(int y) { m_y = y; }
	void setSwapingState(bool state) { m_isSwaping = state; }
	void setMoveOverState(bool state) { m_isMoveOver = state; }

	void showSelection(); //���ֱ߿�
	void hideSelection(); //���ر߿�

	void crush(); //����

private:
	int m_type;
	Sprite* m_selection; //������ʯ������ʾ�ı߿�

	//����
	int m_x;
	int m_y;

	bool m_isSwaping; //�Ƿ����ڽ���
	bool m_isCrushOver; //�Ƿ��������
	bool m_isMoveOver; 
};

#endif
