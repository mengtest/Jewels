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

	//�������г�ʼ��һ���յı�ʯ������С
	m_JewelsBox.resize(m_row);
	for (auto &vec : m_JewelsBox)
		vec.resize(m_col);

	//������ʯ
	for (int x = 0; x < m_col; x++)
	{
		for (int y = 0; y < m_row; y++)
		{
			//�������ָ��
			m_JewelsBox[x][y] = addAJewel(x, y); 
		}
	}

	//���봥������
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
	//��ȡ����������꣬ͨ������������ʯ
	auto pos = this->convertToNodeSpace(pTouch->getLocation());
	int x = pos.x / GRID_WIDTH;
	int y = pos.y / GRID_WIDTH;

	if (pos.x < 0 || pos.y < 0 || x > m_row - 1 || y > m_col - 1) //û�а����������棬��Ч
	{
		return false;
	}

	//log("touch coor: x=%d,y=%d type=%d", m_JewelsBox[x][y]->getX(), m_JewelsBox[x][y]->getY(), m_JewelsBox[x][y]->getType());

	//�õ���ǰѡ�еı�ʯ
	m_jewelSelected = m_JewelsBox[x][y];
	m_startJewel = m_JewelsBox[x][y];

	return true;
}

void JewelsGrid::onTouchMoved(Touch* pTouch, Event* pEvent)
{	
	//���û��ѡ��ʯ����ô����
	if (!m_jewelSelected)
		return;

	//��ʼ����ʱ������
	int startX = m_startJewel->getX();
	int startY = m_startJewel->getY();

	//�����������
	auto pos = this->convertToNodeSpace(pTouch->getLocation());
	int touchX = pos.x / GRID_WIDTH;
	int touchY = pos.y / GRID_WIDTH;

	//����touchX,touchY�Ƿ���磬�Ƿ�Ϳ�ʼ��������һ��
	if (pos.x < 0 || pos.y < 0 || touchX > m_row - 1 || touchY > m_col - 1 || Vec2(touchX, touchY) == Vec2(startX, startY)) 
	{
		return;
	}

	//�жϿ�ʼ�����봥�������Ƿ����һ����λ
	if (abs(startX - touchX) + abs(startY - touchY) != 1)
	{
		//log("touch pos not on border");
		return;
	}
	else
	{
		//������ѡ��ʯָ��Ϊ��
		m_jewelSelected = nullptr;

		m_touchJewel = m_JewelsBox[touchX][touchY];

		//������ʯ�����Լ�ָ��
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

	//1.������ʯ�����ڵı�ʯָ��
	//2.������ʯ����
	//3.��ʯ�ƶ����µ�λ��
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
				//�ı䱦ʯ�����ڵ�����
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
			//�ı䱦ʯ�����ڵ�����
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
		int empty_count = 0; //һ���ܵĿո�����

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
		//��������±�ʯ
		//��ʼλ���ڲ�����
		auto newJewel = Jewel::createByType(random(FIRST_JEWEL_ID, LAST_JEWEL_ID), jewel->getX(), m_row); 
		setJewelPixPos(newJewel, newJewel->getX(), m_row); 
		addChild(newJewel);
		m_newJewelBox.pushBack(newJewel);

		//��ʯ������Ӧ��ˢ�µı�ʯ��ˢ��λ
		m_JewelsBox[jewel->getX()][jewel->getY()] = nullptr;

		//ԭ�б�ʯ��������
		jewel->crush();
	}
}

void JewelsGrid::canCrush()
{
	int count = 0; //������
	Jewel *JewelBegin = nullptr; //��ߵ�һ����ʯ
	Jewel *JewelNext = nullptr;

	//����ÿһ��
	for (int x = 0; x < m_col; x++) 
	{
		for (int y = 0; y < m_row - 1;)
		{
			count = 1;
			JewelBegin = m_JewelsBox[x][y];
			JewelNext = m_JewelsBox[x][y + 1];

			 //�����ͬ����
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

	//����ÿһ��
	for (int y = 0; y < m_row; y++) 
	{
		for (int x = 0; x < m_col - 1;)
		{
			count = 1;
			JewelBegin = m_JewelsBox[x][y];
			JewelNext = m_JewelsBox[x+1][y];

			 //�����ͬ����
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
	//���������ʯ���ͣ������ʯ���Ϸ������´���ֱ���Ϸ�Ϊֹ������������Ϸ����λ�ã�������Ⱦ��
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

	//������±Ƚ��Ƿ�����
	//���x�������y����С�ڵ���1����ô�ñ�ʯ�ڴ���϶��ǺϷ��ģ���Ҫ����ͬʱ�Ϸ�����
	bool isBackLegal = true;
	bool isUpLegal = true;

	if (x > 1)
	{
		//���ֱ�Ƚ���λ�������ʯ���Ͳ�һ������ô���غϷ�
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
		//���·ֱ�Ƚ���λ�������ʯ���Ͳ�һ������ô���غϷ�
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