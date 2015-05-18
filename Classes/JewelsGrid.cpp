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

	m_row = row;
	m_col = col;

	m_jewelSelected = nullptr;
	m_jewelSwapped = nullptr;

	m_isSwapOver = true;
	m_isFreshOver = true;
	m_isCrushOver = true;

	//�������г�ʼ��һ���յı�ʯ������С
	m_JewelsBox.resize(m_row);
	for (auto &vec : m_JewelsBox)
		vec.resize(m_col);

	//1.���ݲ��ִ�С��������ʯ����
	//2.�������������½�Ϊԭ�㣬x��y��Ϊ������
	for (int x = 0; x < m_col; x++)
	{
		for (int y = 0; y < m_row; y++)
		{
			//����һ�ű�ʯ����������ָ�뵽��ʯ������
			m_JewelsBox[x][y] = createAJewel(x, y); 
		}
	}

	//���봥������
	auto listener = EventListenerTouchOneByOne::create();
	listener->setSwallowTouches(true);
	listener->onTouchBegan = CC_CALLBACK_2(JewelsGrid::onTouchBegan, this);
	listener->onTouchMoved = CC_CALLBACK_2(JewelsGrid::onTouchMoved, this);

	_eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);

	log("JewelsGrid init!");
	return true;
}

Jewel* JewelsGrid::createAJewel(int x, int y)
{
	//1.���ݲ������괴��һ�ű�ʯ���������
	//2.�жϸñ�ʯ�Ƿ�Ϸ�������������
	//3.���øñ�ʯ����������
	//4.���ñ�ʯ������Ⱦ�ڵ�
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

bool JewelsGrid::isJewelLegal(Jewel* jewel, int x, int y)
{
	//1.�ֱ��ж��¼���ı�ʯ��x��y�᷽���Ƿ������
	//2.�����Ǵ���������뱦ʯ�����ֻ�����������ж�
	//3.x��y����С�ڵ���1�����ж�
	//4.����ͬʱ�Ϸ����Ϸ�
	bool isXLegal = true;
	bool isYLegal = true;

	if (x > 1)
	{
		//��x�Ḻ����ֱ�Ƚ���λ�������ʯ���Ͷ�һ������ô��ʾ�������ñ�ʯ���Ϸ�
		if (jewel->getType() == m_JewelsBox[x-1][y]->getType() && 
			jewel->getType() == m_JewelsBox[x-2][y]->getType()
			)
		{
			isXLegal = false;
		}
		else
			isXLegal = true;
	}
	
	if (y > 1)
	{
		//��y�Ḻ����ֱ�Ƚ���λ�������ʯ���Ͷ�һ������ô��ʾ�������ñ�ʯ���Ϸ�
		if (jewel->getType() == m_JewelsBox[x][y-1]->getType() && 
			jewel->getType() == m_JewelsBox[x][y-2]->getType())
		{
			isYLegal = false;
		}
		else
			isYLegal = true;
	}

	return isXLegal && isYLegal;
}

void JewelsGrid::setJewelPixPos(Jewel* jewel, float x, float y)
{
	jewel->setPosition(x * GRID_WIDTH, y * GRID_WIDTH); 
}

bool JewelsGrid::onTouchBegan(Touch* pTouch, Event* pEvent)
{
	//�������������ת��Ϊģ������
	auto pos = this->convertToNodeSpace(pTouch->getLocation());
	
	//�Ƿ��а��ڱ�ʯ������
	if (Rect(0, 0, m_col*GRID_WIDTH, m_row*GRID_WIDTH).containsPoint(pos))
	{
		//�õ���������
		int x = pos.x / GRID_WIDTH;
		int y = pos.y / GRID_WIDTH;

		//�õ���ǰѡ�еı�ʯ
		m_jewelSelected = m_JewelsBox[x][y];

		//log("touch coordinate: x=%d,y=%d jewel's type:%d", x, y, m_jewelSelected->getType());

		return true;
	}
	else
	{
		return false;
	}
}

void JewelsGrid::onTouchMoved(Touch* pTouch, Event* pEvent)
{
	//���û��ѡ��ʯ����ô����
	if (!m_jewelSelected)
	{
		return;
	}

	//��ѡ��ʯ�Ĳ�������
	int startX = m_jewelSelected->getX();
	int startY = m_jewelSelected->getY();

	//������Ĳ�������
	auto pos = this->convertToNodeSpace(pTouch->getLocation());
	int touchX = pos.x / GRID_WIDTH;
	int touchY = pos.y / GRID_WIDTH;

	//��������㲻�ڲ����ڣ����ߴ����㲼���������ѡ��ʯ��������һ������ô����
	if (!Rect(0, 0, m_col*GRID_WIDTH, m_row*GRID_WIDTH).containsPoint(pos) || Vec2(touchX, touchY) == Vec2(startX, startY)) 
	{
		return;
	}

	//�ж���ѡ��ʯ�Ĳ��������봥����Ĳ��������Ƿ����һ����λ
	if (abs(startX - touchX) + abs(startY - touchY) != 1)
	{
		//log("touch pos not on border");
		return;
	}

	//���µ����������������ı�ʯ���������н����ı�ʯ
	//ͨ��������������ȡ�������ı�ʯ
	m_jewelSwapped = m_JewelsBox[touchX][touchY];

	//������ʯ����������״̬��׽�������ڽ�����ɺ��ж��Ƿ����������
	swapJewels(m_jewelSelected, m_jewelSwapped);
	schedule(schedule_selector(JewelsGrid::onJewelsSwaping));
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
	if (m_jewelSelected->getSwapingState() || m_jewelSwapped->getSwapingState())
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
	if (m_jewelSelected->getSwapingState() || m_jewelSwapped->getSwapingState())
	{
		return;
	}
	else
	{
		log("swap back!");

		_eventDispatcher->resumeEventListenersForTarget(this);

		m_jewelSelected = nullptr;

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

		m_jewelSelected = nullptr;

		goCrush();
		schedule(schedule_selector(JewelsGrid::onJewelsCrushing));
	}
	else
	{
		log("no, cant crush!");

		if (m_jewelSelected)
		{
			swapJewels(m_jewelSelected, m_jewelSwapped);
			schedule(schedule_selector(JewelsGrid::onJewelsSwapingBack));
		}
		else
		{
			_eventDispatcher->resumeEventListenersForTarget(this);
		}
	}
}