#ifndef JEWELSJewelsGrid_H
#define JEWELSJewelsGrid_H

#include "cocos2d.h"
#include <vector>

USING_NS_CC;
using namespace std;

#define GRID_WIDTH 40 //һ����������Ϊ40

class Jewel;

//�����࣬�����˱�ʯ�������ڵ��߼�
class JewelsGrid : public Node
{
public:
	static JewelsGrid* create(int row, int col); //�����и��д�������
	bool init(int row, int col);

private:
	Jewel* addAJewel(int x, int y);
	bool isJewelLegal(Jewel* jewel, int x, int y);
	//void clearAllJewels();

	//���ݶ����x��y��ֵ����������Ϸʵ������λ�á����������������Ͻ�Ϊԭ�㣬x��y��Ϊ����
	void setJewelPixPos(Jewel* jewel, float x, float y); 

private:
	bool onTouchBegan(Touch*, Event*);
	void onTouchEnded(Touch*, Event*);

private:
	int m_row; //����
	int m_col; //����

	Jewel* m_jewelSelected; //��ǰѡ�еı�ʯ

	vector<vector<Jewel*>> m_JewelsBox; //��ű�ʯ���������
};

#endif

