#ifndef JEWELSJewelsGrid_H
#define JEWELSJewelsGrid_H

#include "cocos2d.h"
#include <vector>

USING_NS_CC;
using namespace std;

#define GRID_WIDTH 40 //一个格子像素为40
#define MOVE_SPEED 0.2 //宝石移动一格速度

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
	void swapJewls(Jewel *jewelA, Jewel *jewelB); //交换两个宝石
	void moveJewelToNewPos(Jewel* jewel); //移动到新位置

	void refreshJewelsGrid(float dt);
	void refreshJewelsToNewPos(int startX, int startY, int count);

	void onCrush(float dt);  //准备crush
	bool canCrush(); //判断能否crush
	void goCrush(); //开始cursh

	void onJewelSwaping(float dt);

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

	bool m_isOnCrush;

	Jewel* m_jewelSelected; //当前选中的宝石
	Jewel* m_startJewel;
	Jewel* m_touchJewel; 

	vector<vector<Jewel*>> m_JewelsBox; //存放宝石对象的容器
	Vector<Jewel*> m_crushJewelBox; //准备消除的宝石容器
	Vector<Jewel*> m_newJewelBox; //准备加入布局的宝石的容器
};

#endif

