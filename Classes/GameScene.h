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
	void initGrid();  //��ʼ����������ı�ʯ
	Jewel* addAJewel(int x, int y); //�������꣨�������أ��������Ͻ�Ϊԭ������꣩���������һ�ű�ʯ
	void clearAllJewels(); //������б�����m_jewels����ı�ʯ����

	bool isJewelLegal(Jewel* jewel, int x, int y); //�ж�һ����ʯ�Ƿ�Ϸ�����û������

private:
	Jewel* m_jewels[8][8]; //����һ��8��8�е�������Ϊ�زľ�ֻ��8��8�еģ�,������Jewel*
};

#endif
