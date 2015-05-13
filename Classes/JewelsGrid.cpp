#include "JewelsGrid.h"
#include "Jewel.h"

JewelsGrid* JewelsGrid::create(int row, int col)
{
	auto grid = new JewelsGrid();
	if (grid && grid->init(row, col))
	{
		grid->autorelease();
		return grid;
	}
	else
	{
		CC_SAFE_DELETE(grid);
		return nullptr;
	}
}

bool JewelsGrid::init(int row, int col)
{
	Node::init();

	m_jewelSelected = nullptr;
	m_row = row;
	m_col = col;

	//根据行列初始化一个空的宝石容器大小
	m_JewelsBox.resize(m_row);
	for (auto &vec : m_JewelsBox)
		vec.resize(m_col);

	//创建宝石
	for (int i = 0; i < 8; i++)
	{
		for (int j = 0; j < 8; j++)
		{
			//保存对象指针
			m_JewelsBox[i][j] = addAJewel(i, j); 
		}
	}

	//加入触摸监听
	auto listener = EventListenerTouchOneByOne::create();
	listener->setSwallowTouches(true);
	listener->onTouchBegan = CC_CALLBACK_2(JewelsGrid::onTouchBegan, this);
	listener->onTouchEnded = CC_CALLBACK_2(JewelsGrid::onTouchEnded, this);

	_eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);

	log("JewelsGrid init!");
	return true;
}


bool JewelsGrid::onTouchBegan(Touch* pTouch, Event* pEvent)
{
	//获取触摸点的坐标，通过坐标索引宝石
	auto pos = this->convertToNodeSpace(pTouch->getLocation());
	int x = pos.x / GRID_WIDTH;
	int y = pos.y / GRID_WIDTH;

	if (pos.x < 0 || pos.y < 0 || x > m_row || y > m_col) //没有按在网格上面，无效
	{
		return false;
	}
	
	log("touch coor: x=%d,y=%d", x, y);

	//得到当前选中的宝石，并显示选框
	m_jewelSelected = m_JewelsBox[x][y];
	m_jewelSelected->showSelection();

	return true;
}

void JewelsGrid::onTouchEnded(Touch* pTouch, Event*)
{
	if (m_jewelSelected)
	{
		m_jewelSelected->hideSelection(); //隐藏选框
	}
}

Jewel* JewelsGrid::addAJewel(int x, int y)
{
	//随机创建宝石类型（如果宝石不合法，重新创建直到合法为止），设置其游戏像素位置，加入渲染树
	Jewel* jewel = nullptr;
	while(1)
	{
		jewel = Jewel::createByType(random(1, 7));
		
		if (isJewelLegal(jewel, x, y))
			break;
	}	

	setJewelPixPos(jewel, x, y);
	addChild(jewel);

	log("add a jewel!---type:%d---x:%d---y:%d", jewel->getType(), x, y);

	return jewel;
}

void JewelsGrid::setJewelPixPos(Jewel* jewel, float x, float y)
{
	jewel->setPosition(x * GRID_WIDTH, y * GRID_WIDTH); 
}
	
bool JewelsGrid::isJewelLegal(Jewel* jewel, int x, int y)
{
	if (!jewel) return false;

	//向后向下比较是否三连
	//如果x坐标或者y坐标小于等于1，那么该宝石在此轴肯定是合法的，但要两轴同时合法才行
	bool isBackLegal = true;
	bool isUpLegal = true;

	if (x > 1)
	{
		//向后分别比较两位，如果宝石类型不一样，那么返回合法
		if (jewel->getType() == m_JewelsBox[x-1][y]->getType() && 
			jewel->getType() == m_JewelsBox[x-2][y]->getType() &&
			m_JewelsBox[x-2][y]->getType() == m_JewelsBox[x-1][y]->getType())
		{
			isBackLegal = false;
		}
		else
			isBackLegal = true;
	}
	
	if (y > 1)
	{
		//向下分别比较两位，如果宝石类型不一样，那么返回合法
		if (jewel->getType() == m_JewelsBox[x][y-1]->getType() && 
			jewel->getType() == m_JewelsBox[x][y-2]->getType() &&
			m_JewelsBox[x][y-1]->getType() == m_JewelsBox[x][y-2]->getType())
		{
			isUpLegal = false;
		}
		else
			isUpLegal = true;
	}

	return isBackLegal && isUpLegal;
}

/*
void JewelsGrid::clearAllJewels()
{
	for (int i = 0; i < 8; i++)
		for (int j = 0; j < 8; j++)
		{
			if (m_JewelsBox[i][j])
				m_JewelsBox[i][j]->removeFromParent();
		}
}
*/