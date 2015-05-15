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
	m_canJewelMoveRight = false;
	m_canJewelMoveUp = false;
	m_canJewelMoveDown = false;
	m_canJewelMoveLeft = false;

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
	m_canJewelMoveUp = true;
	m_canJewelMoveDown = true;
	m_canJewelMoveLeft = true;
	m_canJewelMoveRight = true;

	//��ȡ����������꣬ͨ������������ʯ
	auto pos = this->convertToNodeSpace(pTouch->getLocation());
	int x = pos.x / GRID_WIDTH;
	int y = pos.y / GRID_WIDTH;

	if (pos.x < 0 || pos.y < 0 || x > m_row - 1 || y > m_col - 1) //û�а����������棬��Ч
	{
		return false;
	}

	log("touch coor: x=%d,y=%d type=%d", m_JewelsBox[x][y]->getX(), m_JewelsBox[x][y]->getY(), m_JewelsBox[x][y]->getType());

	m_startTouchCoor = Vec2(x, y);

	//�õ���ǰѡ�еı�ʯ������ʾѡ��
	m_jewelSelected = m_JewelsBox[x][y];
	m_jewelSelected->showSelection();

	return true;
}

void JewelsGrid::onTouchMoved(Touch* pTouch, Event* pEvent)
{	
	//���û��ѡ��ʯ����ô����
	if (!m_jewelSelected)
		return;

	//��ʼ����ʱ������
	int startX = m_startTouchCoor.x;
	int startY = m_startTouchCoor.y;

	//�����������
	auto pos = this->convertToNodeSpace(pTouch->getLocation());
	int touchX = pos.x / GRID_WIDTH;
	int touchY = pos.y / GRID_WIDTH;

	//����touchX,touchY�Ƿ���磬�Ƿ�Ϳ�ʼ��������һ��
	if (pos.x < 0 || pos.y < 0 || touchX > m_row - 1 || touchX > m_col - 1 || Vec2(touchX, touchY) == m_startTouchCoor) 
	{
		return;
	}

	//�жϿ�ʼ�����봥�������Ƿ����һ����λ
	if (abs(startX - touchX) + abs(startY - touchY) != 1)
	{
		//log("touch pos not border");
		return;
	}

	auto startJewel = m_JewelsBox[startX][startY];
	auto touchJewel = m_JewelsBox[touchX][touchY];

	//������ʯ�����Լ�ָ��
	swapJewls(startJewel, touchJewel, &m_JewelsBox[startX][startY], &m_JewelsBox[touchX][touchY]);

	/*
	if (!canCrush())
	{
		//�����������
		m_jewelSelected = nullptr;
		swapJewls(startJewel, touchJewel, &m_JewelsBox[startX][startY], &m_JewelsBox[touchX][touchY]);
	}
	*/
}

void JewelsGrid::onTouchEnded(Touch* pTouch, Event*)
{
	if (m_jewelSelected)
	{
		m_jewelSelected->hideSelection(); //����ѡ��
	}
}

bool JewelsGrid::canCrush()
{
	return false;
}

void JewelsGrid::moveJewelToNewPos(Jewel* jewel)
{
	auto move = MoveTo::create(0.2, Vec2(jewel->getX() * GRID_WIDTH, jewel->getY() * GRID_WIDTH));
	jewel->runAction(move);
}

void JewelsGrid::swapJewls(Jewel *jewelA, Jewel *jewelB, Jewel** AInBoxADD, Jewel** BInBoxADD)
{
	//1.������ʯ����
	//2.��ʯ�ƶ����µ�λ��
	//3.������ʯ�����ڵı�ʯָ��
	auto tempX = jewelA->getX();
	jewelA->setX(jewelB->getX());
	jewelB->setX(tempX);

	auto tempY = jewelA->getY();
	jewelA->setY(jewelB->getY());
	jewelB->setY(tempY);

	moveJewelToNewPos(jewelA);
	moveJewelToNewPos(jewelB);

	auto temp = *AInBoxADD;
	*AInBoxADD = *BInBoxADD;
	*BInBoxADD = temp;
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