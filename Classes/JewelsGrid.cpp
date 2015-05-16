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
	m_isOnCrush = false;

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

	schedule(schedule_selector(JewelsGrid::onCrush));

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
		_eventDispatcher->pauseEventListenersForTarget(this);

		//设置已选宝石指针为空
		m_jewelSelected = nullptr;

		m_touchJewel = m_JewelsBox[touchX][touchY];

		//交换宝石坐标以及指针
		schedule(schedule_selector(JewelsGrid::onJewelSwaping));

		swapJewls(m_startJewel, m_touchJewel);
	}
}

void JewelsGrid::onJewelSwaping(float dt)
{
	if (m_startJewel->getSwapingState() || m_touchJewel->getSwapingState())
	{
		//如果还在交换，那么不能开始crush
		m_isOnCrush = false;
	}
	else
	{
		unschedule(schedule_selector(JewelsGrid::onJewelSwaping));
		m_isOnCrush = true;
	}
}

void JewelsGrid::swapJewls(Jewel *jewelA, Jewel *jewelB)
{
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

	moveJewelToNewPos(jewelA);
	moveJewelToNewPos(jewelB);
}

void JewelsGrid::moveJewelToNewPos(Jewel* jewel)
{
	jewel->setSwapingState(true);
	auto move = MoveTo::create(MOVE_SPEED, Vec2(jewel->getX() * GRID_WIDTH, jewel->getY() * GRID_WIDTH));
	auto call = CallFunc::create([jewel](){
		jewel->setSwapingState(false);
	});
	jewel->runAction(Sequence::create(move, call, nullptr));
}

void JewelsGrid::onTouchEnded(Touch* pTouch, Event*)
{
	m_jewelSelected = nullptr;
}

void JewelsGrid::refreshJewelsToNewPos(int startX, int startY, int count)
{
	auto pJewelsbox = &m_JewelsBox;

	for (int y = startY+count; y < m_row; y++)
	{
		auto jewel = m_JewelsBox[startX][y];

		jewel->setY(jewel->getY() - count);

		jewel->setMoveOverState(false);
		auto move = MoveBy::create(0.2, Vec2(0, -count*GRID_WIDTH));
		auto call = CallFunc::create([startX, y, count, pJewelsbox, jewel](){
			//改变宝石盒子内的数据
			(*pJewelsbox)[startX][jewel->getY()] = jewel;
			jewel->setMoveOverState(true);
		});

		jewel->runAction(Sequence::create(move, call, nullptr));
	}

	//新宝石盒子内的宝石也开始往下移动
	int i = count;
	int delta = 1;

	for (auto jewel : m_newJewelBox)
	{
		if (jewel->getX() == startX)
		{
			jewel->setY(m_row - i);

			jewel->setMoveOverState(false);
			auto delay = DelayTime::create(0.2);
			auto move = MoveBy::create(0.2, Vec2(0, -i*GRID_WIDTH));
			//改变宝石盒子内的数据
			auto call = CallFunc::create([jewel, pJewelsbox, startX, this, i](){
				(*pJewelsbox)[startX][jewel->getY()] = jewel;
				m_newJewelBox.eraseObject(jewel);
				jewel->setMoveOverState(true);
			});

			jewel->runAction(Sequence::create(delay, move, call, nullptr));
			i--;
		}
	}
}

void JewelsGrid::refreshJewelsGrid(float dt)
{
	for (auto &jewel : m_crushJewelBox)
	{
		//如果还没有消除结束
		if (!jewel->getCrushState())
			return;
	}
	
	m_crushJewelBox.clear();
	unschedule(schedule_selector(JewelsGrid::refreshJewelsGrid));

	int delay_time = m_newJewelBox.size()*0.2;

	for (int x = 0; x < m_col; x++)
	{
		for (int y = 0; y < m_row;)
		{
			auto jewel = m_JewelsBox[x][y];
			int col_count = 0; //一列为空个数
			while (!jewel)
			{
				//如果为空
				col_count++;
				int newIndex = y+col_count;
				if (newIndex > m_row-1)
					break;
				jewel = m_JewelsBox[x][newIndex];
			}
			
			//如果该列已经有空缺
			if (col_count != 0)
			{
				//该列所有宝石往下移动col_count个单位
				//log("the %d col has %d empty", x, col_count);
				refreshJewelsToNewPos(x, y, col_count);
				y += col_count;
			}
			else
			{
				y++;
			}
		}
	}

	m_isOnCrush = true;
	schedule(schedule_selector(JewelsGrid::onCrush));
}

void JewelsGrid::goCrush()
{
	/*
	_eventDispatcher->pauseEventListenersForTarget(this);

	if (m_crushJewelBox.empty())
	{
		_eventDispatcher->resumeEventListenersForTarget(this);
		return;
	}
	*/

	for (auto jewel : m_crushJewelBox)
	{
		//生成随机新宝石
		//初始位置在布局外
		auto newJewel = Jewel::createByType(random(1, 7), jewel->getX(), m_row); 
		setJewelPixPos(newJewel, newJewel->getX(), m_row); 
		addChild(newJewel);
		m_newJewelBox.pushBack(newJewel);

		//宝石盒子暂时为空
		m_JewelsBox[jewel->getX()][jewel->getY()] = nullptr;

		//原有宝石对象消除
		jewel->crush();
	}

	schedule(schedule_selector(JewelsGrid::refreshJewelsGrid));
}

bool JewelsGrid::canCrush()
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
		return true;
	}
	else
	{
		return false;
	}
}

void JewelsGrid::onCrush(float dt)
{
	if (!m_isOnCrush)
		return;

	for (int x = 0; x < m_col; x++)
		for (int y = 0; y < m_row; y++)
		{
			if (m_JewelsBox[x][y] == nullptr)
				return;
			if (!m_JewelsBox[x][y]->getMoveOverState())
				return;
		}

	m_isOnCrush = false;

	if (!canCrush())
	{
		log("cant Crush!");

		_eventDispatcher->resumeEventListenersForTarget(this);

		//如果不能消除
		if (m_startJewel && m_touchJewel)
		{
			int startX = m_startJewel->getX();
			int startY = m_startJewel->getY();

			int touchX = m_touchJewel->getX();
			int touchY = m_touchJewel->getY();

			swapJewls(m_startJewel, m_touchJewel);
		}
	}
	else
	{
		goCrush();
		log("Crush!");
	}

	m_startJewel = nullptr;
	m_touchJewel = nullptr;
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