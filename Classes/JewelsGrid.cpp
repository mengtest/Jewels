#include "JewelsGrid.h"
#include "Jewel.h"

JewelsGrid* JewelsGrid::jewelsgrid = nullptr;

JewelsGrid* JewelsGrid::create(int row, int col)
{
	JewelsGrid::jewelsgrid = new JewelsGrid();
	if (jewelsgrid && jewelsgrid->init(row, col))
	{
		jewelsgrid->autorelease();
		return jewelsgrid;
	}
	else
	{
		CC_SAFE_DELETE(jewelsgrid);
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
	listener->onTouchMoved = CC_CALLBACK_2(JewelsGrid::onTouchMoved, this);
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

	if (pos.x < 0 || pos.y < 0 || x > m_row - 1 || y > m_col - 1) //没有按在网格上面，无效
	{
		return false;
	}

	log("touch coor: x=%d,y=%d type=%d", m_JewelsBox[x][y]->getX(), m_JewelsBox[x][y]->getY(), m_JewelsBox[x][y]->getType());

	//得到当前选中的宝石，并显示选框
	m_jewelSelected = m_JewelsBox[x][y];
	m_startJewel = m_JewelsBox[x][y];

	return true;
}

void JewelsGrid::onTouchMoved(Touch* pTouch, Event* pEvent)
{	
	//如果没有选择宝石，那么返回
	if (!m_jewelSelected)
		return;

	//开始触摸时的坐标
	int startX = m_startJewel->getX();
	int startY = m_startJewel->getY();

	//触摸点的坐标
	auto pos = this->convertToNodeSpace(pTouch->getLocation());
	int touchX = pos.x / GRID_WIDTH;
	int touchY = pos.y / GRID_WIDTH;

	//坐标touchX,touchY是否出界，是否和开始触摸坐标一致
	if (pos.x < 0 || pos.y < 0 || touchX > m_row - 1 || touchY > m_col - 1 || Vec2(touchX, touchY) == Vec2(startX, startY)) 
	{
		return;
	}

	//判断开始坐标与触摸坐标是否相隔一个单位
	if (abs(startX - touchX) + abs(startY - touchY) != 1)
	{
		//log("touch pos not on border");
		return;
	}

	//设置已选宝石指针为空
	m_jewelSelected = nullptr;

	m_touchJewel = m_JewelsBox[touchX][touchY];

	//交换宝石坐标以及指针
	swapJewls(m_startJewel, m_touchJewel, &m_JewelsBox[startX][startY], &m_JewelsBox[touchX][touchY]);

	schedule(schedule_selector(JewelsGrid::onCrush), 0, 0, 0.3);
}

void JewelsGrid::onTouchEnded(Touch* pTouch, Event*)
{
	m_jewelSelected = nullptr;
}

bool JewelsGrid::canCrush()
{
	m_crushJewelBox.clear(); //清空消除容器

	int count = 0; //连续数
	Jewel *JewelBegin = nullptr; //左边第一个宝石
	Jewel *JewelNext = nullptr;

	//遍历每一列
	for (int i = 0; i < m_col; i++) 
	{
		for (int j = 0; j < m_row - 1;)
		{
			count = 1;
			JewelBegin = m_JewelsBox[i][j];
			JewelNext = m_JewelsBox[i][j + 1];
			 //如果是同类型
			while (JewelBegin->getType() == JewelNext->getType())
			{
				count++;
				int nextIndex = j + count;
				if (nextIndex > m_row - 1)
					break;
				JewelNext = m_JewelsBox[i][nextIndex];
			}
			if (count >= 3)
			{
				for (int n = 0; n < count; n++)
					m_crushJewelBox.pushBack(m_JewelsBox[i][j+n]);
			}
			j += count;
		}
	}

	//遍历每一行
	for (int i = 0; i < m_row; i++) 
	{
		for (int j = 0; j < m_col - 1;)
		{
			count = 1;
			JewelBegin = m_JewelsBox[j][i];
			JewelNext = m_JewelsBox[j+1][i];
			 //如果是同类型
			while (JewelBegin->getType() == JewelNext->getType())
			{
				count++;
				int nextIndex = j + count;
				if (nextIndex > m_col - 1)
					break;
				JewelNext = m_JewelsBox[nextIndex][i];
			}
			if (count >= 3)
			{
				for (int n = 0; n < count; n++)
					m_crushJewelBox.pushBack(m_JewelsBox[j+n][i]);
			}
			j += count;
		}
	}

	if (!m_crushJewelBox.empty())
	{
		return true;
	}
	else
	{
		return false;
	}
}

void JewelsGrid::onCrush(float dt)
{
	if (!canCrush())
	{
		log("cant Crush!");

		//如果不能消除
		int startX = m_startJewel->getX();
		int startY = m_startJewel->getY();

		int touchX = m_touchJewel->getX();
		int touchY = m_touchJewel->getY();

		swapJewls(m_startJewel, m_touchJewel, &m_JewelsBox[startX][startY], &m_JewelsBox[touchX][touchY]);
	}
	else
	{
		log("Crush!");
	}
}

void JewelsGrid::moveJewelToNewPos(Jewel* jewel)
{
	auto call1 = CallFunc::create([this]()
	{
		//开始移动时暂停触摸
		Director::getInstance()->getEventDispatcher()->pauseEventListenersForTarget(this);
	});
	auto move = MoveTo::create(.2f, Vec2(jewel->getX() * GRID_WIDTH, jewel->getY() * GRID_WIDTH));
	auto call2 = CallFunc::create([this]()
	{
		//移动完后开启触摸
		Director::getInstance()->getEventDispatcher()->resumeEventListenersForTarget(this);
	});
	jewel->runAction(Sequence::create(call1, move, call2, nullptr));
}

void JewelsGrid::swapJewls(Jewel *jewelA, Jewel *jewelB, Jewel** AInBoxADD, Jewel** BInBoxADD)
{
	//1.交换宝石坐标
	//2.交换宝石容器内的宝石指针
	//3.宝石移动到新的位置
	auto tempX = jewelA->getX();
	jewelA->setX(jewelB->getX());
	jewelB->setX(tempX);

	auto tempY = jewelA->getY();
	jewelA->setY(jewelB->getY());
	jewelB->setY(tempY);

	auto temp = *AInBoxADD;
	*AInBoxADD = *BInBoxADD;
	*BInBoxADD = temp;

	//该函数将在移动完后开启触摸
	moveJewelToNewPos(jewelA);
	moveJewelToNewPos(jewelB);
}

Jewel* JewelsGrid::addAJewel(int x, int y)
{
	//随机创建宝石类型（如果宝石不合法，重新创建直到合法为止），设置其游戏像素位置，加入渲染树
	Jewel* jewel = nullptr;
	while(1)
	{
		jewel = Jewel::createByType(random(1, 7), x, y);
		
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