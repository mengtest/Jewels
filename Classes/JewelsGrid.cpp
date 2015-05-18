#include "JewelsGrid.h"
#include "Jewel.h"

JewelsGrid* JewelsGrid::create(int row, int col)
{
	auto jewelsgrid = new JewelsGrid();
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

	m_isSwapOver = true;
	m_isFreshOver = true;
	m_isCrushOver = true;

	//根据行列初始化一个空的宝石容器大小
	m_JewelsBox.resize(m_row);
	for (auto &vec : m_JewelsBox)
		vec.resize(m_col);

	//创建宝石
	for (int x = 0; x < m_col; x++)
	{
		for (int y = 0; y < m_row; y++)
		{
			//保存对象指针
			m_JewelsBox[x][y] = addAJewel(x, y); 
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

	//log("touch coor: x=%d,y=%d type=%d", m_JewelsBox[x][y]->getX(), m_JewelsBox[x][y]->getY(), m_JewelsBox[x][y]->getType());

	//得到当前选中的宝石
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
	else
	{
		//设置已选宝石指针为空
		m_jewelSelected = nullptr;

		m_touchJewel = m_JewelsBox[touchX][touchY];

		//交换宝石坐标以及指针
		swapJewels(m_startJewel, m_touchJewel);
		schedule(schedule_selector(JewelsGrid::onJewelsSwaping));		
	}
}

void JewelsGrid::onTouchEnded(Touch* pTouch, Event*)
{
	m_jewelSelected = nullptr;
}

void JewelsGrid::onJewelsFreshing(float dt)
{
	if (m_newJewelBox.size() != 0)
	{
		//log("refreshing!");
		return;
	}
	else
	{
		log("refresh over!");

		unschedule(schedule_selector(JewelsGrid::onJewelsFreshing));

		log("is it can crush?");
		canCrush();
	}
}

void JewelsGrid::onJewelsCrushing(float dt)
{
	for (auto jewel : m_crushJewelBox)
	{
		if (jewel->getCrushOverState() == false)
		{
			//log("crushing");
			return;
		}
	}
	
	m_crushJewelBox.clear();
	unschedule(schedule_selector(JewelsGrid::onJewelsCrushing));
	
	log("crush over!");

	log("begin to refresh!");
	refreshJewelsGrid();
	schedule(schedule_selector(JewelsGrid::onJewelsFreshing));
}

void JewelsGrid::onJewelsSwaping(float dt)
{
	if (m_startJewel->getSwapingState() || m_touchJewel->getSwapingState())
	{
		return;
	}
	else
	{
		log("swap over!");

		unschedule(schedule_selector(JewelsGrid::onJewelsSwaping));

		log("is it can crush?");
		canCrush();
	}
}

void JewelsGrid::onJewelsSwapingBack(float dt)
{
	if (m_startJewel->getSwapingState() || m_touchJewel->getSwapingState())
	{
		return;
	}
	else
	{
		log("swap back!");

		_eventDispatcher->resumeEventListenersForTarget(this);

		unschedule(schedule_selector(JewelsGrid::onJewelsSwapingBack));
	}
}

void JewelsGrid::swapJewels(Jewel *jewelA, Jewel *jewelB)
{
	_eventDispatcher->pauseEventListenersForTarget(this);

	//1.交换宝石容器内的宝石指针
	//2.交换宝石坐标
	//3.宝石移动到新的位置
	auto temp = m_JewelsBox[jewelA->getX()][jewelA->getY()];
	m_JewelsBox[jewelA->getX()][jewelA->getY()] = m_JewelsBox[jewelB->getX()][jewelB->getY()];
	m_JewelsBox[jewelB->getX()][jewelB->getY()] = temp;

	auto tempX = jewelA->getX();
	jewelA->setX(jewelB->getX());
	jewelB->setX(tempX);

	auto tempY = jewelA->getY();
	jewelA->setY(jewelB->getY());
	jewelB->setY(tempY);

	swapJewelToNewPos(jewelA);
	swapJewelToNewPos(jewelB);
}

void JewelsGrid::swapJewelToNewPos(Jewel* jewel)
{
	jewel->setSwapingState(true);
	auto move = MoveTo::create(MOVE_SPEED, Vec2(jewel->getX() * GRID_WIDTH, jewel->getY() * GRID_WIDTH));
	auto call = CallFunc::create([jewel](){
		jewel->setSwapingState(false);
	});
	jewel->runAction(Sequence::create(move, call, nullptr));
}

void JewelsGrid::refreshJewelsToNewPos(int startX)
{
	int n = 0;
	auto pJewelsbox = &m_JewelsBox;

	for (int y = 0; y < m_row; y++)
	{
		auto jewel = m_JewelsBox[startX][y];

		if (!jewel)
		{
			n++;
			continue;
		}
		else if (n != 0)
		{
			jewel->setY(jewel->getY() - n);
			jewel->setFreshOverState(false);
			auto move = MoveBy::create(0.2, Vec2(0, -n*GRID_WIDTH));
			auto call = CallFunc::create([startX, pJewelsbox, jewel](){
				//改变宝石盒子内的数据
				(*pJewelsbox)[startX][jewel->getY()] = jewel;
				jewel->setFreshOverState(true);
			});

			jewel->runAction(Sequence::create(move, call, nullptr));
		}
	}
	
	int i = n;

	for (auto jewel : m_newJewelBox)
	{
		if (jewel->getX() == startX)
		{
			jewel->setY(m_row - i);

			jewel->setFreshOverState(false);
			auto delay = DelayTime::create(0.2);
			auto move = MoveBy::create(0.2, Vec2(0, -i*GRID_WIDTH));
			//改变宝石盒子内的数据
			auto call = CallFunc::create([jewel, pJewelsbox, startX, this, i](){
				(*pJewelsbox)[startX][jewel->getY()] = jewel;
				m_newJewelBox.eraseObject(jewel);
				jewel->setFreshOverState(true);
			});

			jewel->runAction(Sequence::create(delay, move, call, nullptr));
			i--;
		}
	}
}

void JewelsGrid::refreshJewelsGrid()
{
	for (int x = 0; x < m_col; x++)
	{
		int empty_count = 0; //一列总的空格子数

		for (int y = 0; y < m_row; y++)
		{
			auto jewel = m_JewelsBox[x][y];
			if (!jewel)
				empty_count++;
		}
		if (empty_count)
		{
			log("the %d col has %d empty", x, empty_count);
			refreshJewelsToNewPos(x);
		}
	}
}

void JewelsGrid::goCrush()
{
	for (auto jewel : m_crushJewelBox)
	{
		//生成随机新宝石
		//初始位置在布局外
		auto newJewel = Jewel::createByType(random(FIRST_JEWEL_ID, LAST_JEWEL_ID), jewel->getX(), m_row); 
		setJewelPixPos(newJewel, newJewel->getX(), m_row); 
		addChild(newJewel);
		m_newJewelBox.pushBack(newJewel);

		//宝石盒子内应当刷新的宝石置刷新位
		m_JewelsBox[jewel->getX()][jewel->getY()] = nullptr;

		//原有宝石对象消除
		jewel->crush();
	}
}

void JewelsGrid::canCrush()
{
	int count = 0; //连续数
	Jewel *JewelBegin = nullptr; //左边第一个宝石
	Jewel *JewelNext = nullptr;

	//遍历每一列
	for (int x = 0; x < m_col; x++) 
	{
		for (int y = 0; y < m_row - 1;)
		{
			count = 1;
			JewelBegin = m_JewelsBox[x][y];
			JewelNext = m_JewelsBox[x][y + 1];

			 //如果是同类型
			while (JewelBegin->getType() == JewelNext->getType())
			{
				count++;
				int nextIndex = y + count;
				if (nextIndex > m_row - 1)
					break;
				JewelNext = m_JewelsBox[x][nextIndex];
			}
			if (count >= 3)
			{
				for (int n = 0; n < count; n++)
				{
					auto jewel = m_JewelsBox[x][y+n];
					m_crushJewelBox.pushBack(jewel);
				}
			}
			y += count;
		}
	}

	//遍历每一行
	for (int y = 0; y < m_row; y++) 
	{
		for (int x = 0; x < m_col - 1;)
		{
			count = 1;
			JewelBegin = m_JewelsBox[x][y];
			JewelNext = m_JewelsBox[x+1][y];

			 //如果是同类型
			while (JewelBegin->getType() == JewelNext->getType())
			{
				count++;
				int nextIndex = x + count;
				if (nextIndex > m_col - 1)
					break;
				JewelNext = m_JewelsBox[nextIndex][y];
			}
			if (count >= 3)
			{
				for (int n = 0; n < count; n++)
				{
					auto jewel = m_JewelsBox[x+n][y];
					if (m_crushJewelBox.find(jewel) != m_crushJewelBox.end())
					{
						continue;
					}
					m_crushJewelBox.pushBack(jewel);
				}
			}
			x += count;
		}
	}

	if (!m_crushJewelBox.empty())
	{
		log("yes,crush!");

		m_startJewel = nullptr;
		m_touchJewel = nullptr;

		goCrush();
		schedule(schedule_selector(JewelsGrid::onJewelsCrushing));
	}
	else
	{
		log("no, cant crush!");
		if (m_startJewel && m_startJewel)
		{
			swapJewels(m_startJewel, m_touchJewel);
			schedule(schedule_selector(JewelsGrid::onJewelsSwapingBack));
		}
		else
		{
			_eventDispatcher->resumeEventListenersForTarget(this);
		}
	}
}

Jewel* JewelsGrid::addAJewel(int x, int y)
{
	//随机创建宝石类型（如果宝石不合法，重新创建直到合法为止），设置其游戏像素位置，加入渲染树
	Jewel* jewel = nullptr;
	while(1)
	{
		jewel = Jewel::createByType(random(FIRST_JEWEL_ID, LAST_JEWEL_ID), x, y);
		
		if (isJewelLegal(jewel, x, y))
			break;
	}	

	setJewelPixPos(jewel, x, y);
	addChild(jewel);

	//log("add a jewel!---type:%d---x:%d---y:%d", jewel->getType(), x, y);

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