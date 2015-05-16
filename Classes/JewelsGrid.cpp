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

	schedule(schedule_selector(JewelsGrid::onCrush));

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

	log("touch coor: x=%d,y=%d type=%d", m_JewelsBox[x][y]->getX(), m_JewelsBox[x][y]->getY(), m_JewelsBox[x][y]->getType());

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
		_eventDispatcher->pauseEventListenersForTarget(this);

		//������ѡ��ʯָ��Ϊ��
		m_jewelSelected = nullptr;

		m_touchJewel = m_JewelsBox[touchX][touchY];

		//������ʯ�����Լ�ָ��
		schedule(schedule_selector(JewelsGrid::onJewelSwaping));

		swapJewls(m_startJewel, m_touchJewel);
	}
}

void JewelsGrid::onJewelSwaping(float dt)
{
	if (m_startJewel->getSwapingState() || m_touchJewel->getSwapingState())
	{
		//������ڽ�������ô���ܿ�ʼcrush
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
			//�ı䱦ʯ�����ڵ�����
			(*pJewelsbox)[startX][jewel->getY()] = jewel;
			jewel->setMoveOverState(true);
		});

		jewel->runAction(Sequence::create(move, call, nullptr));
	}

	//�±�ʯ�����ڵı�ʯҲ��ʼ�����ƶ�
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
			//�ı䱦ʯ�����ڵ�����
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
		//�����û����������
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
			int col_count = 0; //һ��Ϊ�ո���
			while (!jewel)
			{
				//���Ϊ��
				col_count++;
				int newIndex = y+col_count;
				if (newIndex > m_row-1)
					break;
				jewel = m_JewelsBox[x][newIndex];
			}
			
			//��������Ѿ��п�ȱ
			if (col_count != 0)
			{
				//�������б�ʯ�����ƶ�col_count����λ
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
		//��������±�ʯ
		//��ʼλ���ڲ�����
		auto newJewel = Jewel::createByType(random(1, 7), jewel->getX(), m_row); 
		setJewelPixPos(newJewel, newJewel->getX(), m_row); 
		addChild(newJewel);
		m_newJewelBox.pushBack(newJewel);

		//��ʯ������ʱΪ��
		m_JewelsBox[jewel->getX()][jewel->getY()] = nullptr;

		//ԭ�б�ʯ��������
		jewel->crush();
	}

	schedule(schedule_selector(JewelsGrid::refreshJewelsGrid));
}

bool JewelsGrid::canCrush()
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

		//�����������
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
	//���������ʯ���ͣ������ʯ���Ϸ������´���ֱ���Ϸ�Ϊֹ������������Ϸ����λ�ã�������Ⱦ��
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