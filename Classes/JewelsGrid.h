#ifndef JEWELSJewelsGrid_H
#define JEWELSJewelsGrid_H

#include "cocos2d.h"
#include <vector>

USING_NS_CC;
using namespace std;

#define GRID_WIDTH 40 //一个格子像素为40

class Jewel;

//网格类，包含了宝石在网格内的逻辑
class JewelsGrid : public Node
{
public:
	static JewelsGrid* create(int row, int col); //根据行跟列创建网格
	bool init(int row, int col);

private:
	static JewelsGrid* jewelsgrid;

	Jewel* addAJewel(int x, int y);
	bool isJewelLegal(Jewel* jewel, int x, int y);
	//void clearAllJewels();

	//根据对象的x，y轴值，设置其游戏实际像素位置。网格坐标轴以左上角为原点，x右y下为正轴
	void setJewelPixPos(Jewel* jewel, float x, float y);
	void swapJewls(Jewel **jewelA, Jewel **jewelB); //交换两个宝石的存储数据，由于交换的是指针，所以还需要用二级指针

private:
	bool onTouchBegan(Touch*, Event*);
	void onTouchMoved(Touch*, Event*);
	void onTouchEnded(Touch*, Event*);

public:
	static JewelsGrid* sharedJewelsGrid() { return jewelsgrid; };

	int getRow() { return m_row; }
	int getCol() { return m_col; }

private:
	int m_row; //行数
	int m_col; //列数

	Jewel* m_jewelSelected; //当前选中的宝石
	bool m_canJewelMoveUp; //标明是否已开始移动宝石,为了防止连续触摸而让宝石连续移动，因为一次只能移动一格
	bool m_canJewelMoveDown;
	bool m_canJewelMoveLeft;
	bool m_canJewelMoveRight;

	vector<vector<Jewel*>> m_JewelsBox; //存放宝石对象的容器
};

#endif

