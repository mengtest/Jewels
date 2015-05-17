#ifndef JEWELSJewelsGrid_H
#define JEWELSJewelsGrid_H

#include "cocos2d.h"
#include <vector>

USING_NS_CC;
using namespace std;

#define GRID_WIDTH 40 //һ����������Ϊ40
#define MOVE_SPEED 0.2 //��ʯ�ƶ�һ���ٶ�
#define FIRST_JEWEL_ID 1
#define LAST_JEWEL_ID 6

class Jewel;

//�����࣬�����˱�ʯ�������ڵ��߼�
class JewelsGrid : public Node
{
public:
	static JewelsGrid* create(int row, int col); //�����и��д�������
	bool init(int row, int col);

private:
	static JewelsGrid* jewelsgrid;

	Jewel* addAJewel(int x, int y);
	bool isJewelLegal(Jewel* jewel, int x, int y);
	//void clearAllJewels();

	//���ݶ����x��y��ֵ����������Ϸʵ������λ�á����������������Ͻ�Ϊԭ�㣬x��y��Ϊ����
	void setJewelPixPos(Jewel* jewel, float x, float y);
	void swapJewls(Jewel *jewelA, Jewel *jewelB); //����������ʯ
	void moveJewelToNewPos(Jewel* jewel); //�ƶ�����λ��

	void refreshJewelsGrid();
	void refreshJewelsToNewPos(int startX);

	void stateController(float dt);

	void canCrush(); //�ж��ܷ�crush
	void goCrush(); //��ʼcursh

	void onJewelsSwaping(float dt);
	void onJewelsCrushing(float dt);
	void onJewelsFreshing(float dt);
	void onJewelsSwapingBack(float dt);

private:
	bool onTouchBegan(Touch*, Event*);
	void onTouchMoved(Touch*, Event*);
	void onTouchEnded(Touch*, Event*);

public:
	static JewelsGrid* sharedJewelsGrid() { return jewelsgrid; };

	int getRow() { return m_row; }
	int getCol() { return m_col; }

private:
	int m_row; //����
	int m_col; //����

	bool m_isSwapOver;
	bool m_isFreshOver;
	bool m_isCrushOver;

	int m_state;

	bool m_canCrush;
	bool m_canSwap;

	Jewel* m_jewelSelected; //��ǰѡ�еı�ʯ
	Jewel* m_startJewel;
	Jewel* m_touchJewel; 

	vector<vector<Jewel*>> m_JewelsBox; //��ű�ʯ���������
	Vector<Jewel*> m_crushJewelBox; //׼�������ı�ʯ����
	Vector<Jewel*> m_newJewelBox; //׼�����벼�ֵı�ʯ������
};

#endif

