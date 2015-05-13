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
	Jewel* addAJewel(int x, int y);
	bool isJewelLegal(Jewel* jewel, int x, int y);
	//void clearAllJewels();

	//根据对象的x，y轴值，设置其游戏实际像素位置。网格坐标轴以左上角为原点，x右y下为正轴
	void setJewelPixPos(Jewel* jewel, float x, float y); 

private:
	bool onTouchBegan(Touch*, Event*);
	void onTouchEnded(Touch*, Event*);

private:
	int m_row; //行数
	int m_col; //列数

	Jewel* m_jewelSelected; //当前选中的宝石

	vector<vector<Jewel*>> m_JewelsBox; //存放宝石对象的容器
};

#endif

