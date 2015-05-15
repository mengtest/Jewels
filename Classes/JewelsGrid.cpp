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

	//�������г�ʼ��һ���յı�ʯ������С
	m_JewelsBox.resize(m_row);
	for (auto &vec : m_JewelsBox)
		vec.resize(m_col);

	//������ʯ
	for (int i = 0; i < 8; i++)
	{
		for (int j = 0; j < 8; j++)
		{
			//�������ָ��
			m_JewelsBox[i][j] = addAJewel(i, j); 
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

	log("touch coor: x=%d,y=%d type=%d", m_JewelsBox[x][y]->getX(), m_JewelsBox[x][y]->getY(), m_JewelsBox[x][y]->getType());

	//�õ���ǰѡ�еı�ʯ������ʾѡ��
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

	//������ѡ��ʯָ��Ϊ��
	m_jewelSelected = nullptr;

	m_touchJewel = m_JewelsBox[touchX][touchY];

	//������ʯ�����Լ�ָ��
	swapJewls(m_startJewel, m_touchJewel, &m_JewelsBox[startX][startY], &m_JewelsBox[touchX][touchY]);

	schedule(schedule_selector(JewelsGrid::onCrush), 0, 0, 0.3);
}

void JewelsGrid::onTouchEnded(Touch* pTouch, Event*)
{
	m_jewelSelected = nullptr;
}

bool JewelsGrid::canCrush()
{
	m_crushJewelBox.clear(); //�����������

	int count = 0; //������
	Jewel *JewelBegin = nullptr; //��ߵ�һ����ʯ
	Jewel *JewelNext = nullptr;

	//����ÿһ��
	for (int i = 0; i < m_col; i++) 
	{
		for (int j = 0; j < m_row - 1;)
		{
			count = 1;
			JewelBegin = m_JewelsBox[i][j];
			JewelNext = m_JewelsBox[i][j + 1];
			 //�����ͬ����
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

	//����ÿһ��
	for (int i = 0; i < m_row; i++) 
	{
		for (int j = 0; j < m_col - 1;)
		{
			count = 1;
			JewelBegin = m_JewelsBox[j][i];
			JewelNext = m_JewelsBox[j+1][i];
			 //�����ͬ����
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

		//�����������
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
		//��ʼ�ƶ�ʱ��ͣ����
		Director::getInstance()->getEventDispatcher()->pauseEventListenersForTarget(this);
	});
	auto move = MoveTo::create(.2f, Vec2(jewel->getX() * GRID_WIDTH, jewel->getY() * GRID_WIDTH));
	auto call2 = CallFunc::create([this]()
	{
		//�ƶ����������
		Director::getInstance()->getEventDispatcher()->resumeEventListenersForTarget(this);
	});
	jewel->runAction(Sequence::create(call1, move, call2, nullptr));
}

void JewelsGrid::swapJewls(Jewel *jewelA, Jewel *jewelB, Jewel** AInBoxADD, Jewel** BInBoxADD)
{
	//1.������ʯ����
	//2.������ʯ�����ڵı�ʯָ��
	//3.��ʯ�ƶ����µ�λ��
	auto tempX = jewelA->getX();
	jewelA->setX(jewelB->getX());
	jewelB->setX(tempX);

	auto tempY = jewelA->getY();
	jewelA->setY(jewelB->getY());
	jewelB->setY(tempY);

	auto temp = *AInBoxADD;
	*AInBoxADD = *BInBoxADD;
	*BInBoxADD = temp;

	//�ú��������ƶ����������
	moveJewelToNewPos(jewelA);
	moveJewelToNewPos(jewelB);
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