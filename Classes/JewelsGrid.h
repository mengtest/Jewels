#ifndef JEWELSJewelsGrid_H
#define JEWELSJewelsGrid_H

#include "cocos2d.h"
#include <vector>

USING_NS_CC;
using namespace std;

#define GRID_WIDTH 40 //一个格子像素为40
#define MOVE_SPEED 0.2 //宝石移动一格速度
#define FIRST_JEWEL_ID 1
#define LAST_JEWEL_ID 7

class Jewel;

//网格类，包含了宝石在网格内的逻辑
//该网格布局坐标以左下角为原点，x左y上为正方向
class JewelsGrid : public Node
{
public:
	static JewelsGrid* create(int row, int col); //根据行跟列创建网格
	bool init(int row, int col);

private:
	//关于创建宝石的方法
	Jewel* createAJewel(int x, int y); //根据布局坐标创建一个宝石
	bool isJewelLegal(Jewel* jewel, int x, int y); //判断创建的宝石是否不会三消（即合法）
	void setJewelPixPos(Jewel* jewel, float x, float y); //设置其游戏实际像素位置。网格坐标轴以左下角为原点，x右y上为正轴

	//关于操作宝石的方法
	void swapJewels(Jewel *jewelA, Jewel *jewelB); //交换两个宝石
	void swapJewelToNewPos(Jewel* jewel); //移动到新位置

	void refreshJewelsGrid();
	void refreshJewelsToNewPos(int startX);

private:
	//事件响应部分
	bool onTouchBegan(Touch*, Event*);
	void onTouchMoved(Touch*, Event*);

private:
	//游戏逻辑部分
	void canCrush(); //判断能否crush
	void goCrush(); //开始crush

	//捕捉函数，捕捉游戏步骤是否完成
	void onJewelsSwaping(float dt);
	void onJewelsCrushing(float dt);
	void onJewelsFreshing(float dt);
	void onJewelsSwapingBack(float dt);

public:
	int getRow() { return m_row; }
	int getCol() { return m_col; }

private:
	int m_row; //行数
	int m_col; //列数

	Jewel* m_jewelSelected; //当前选择的宝石
	Jewel* m_jewelSwapped; //欲交换的宝石

	bool m_isSwapOver;
	bool m_isFreshOver;
	bool m_isCrushOver;

	vector<vector<Jewel*>> m_JewelsBox; //存放宝石对象的容器，通过坐标索引宝石
	Vector<Jewel*> m_crushJewelBox; //准备消除的宝石容器
	Vector<Jewel*> m_newJewelBox; //准备加入布局的宝石的容器
};

#endif

