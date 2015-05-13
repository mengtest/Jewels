#include "GameScene.h"
#include "Jewel.h"

Scene* GameScene::createScene()
{
	auto scene = Scene::create();
	auto layer = GameScene::create();
	scene->addChild(layer);
	return scene;
}

bool GameScene::init()
{
	Layer::init();

	auto visibleSize = Director::getInstance()->getVisibleSize();

	//���ر���ͼ
	auto bg = Sprite::create("bground1.png");
	bg->setAnchorPoint(Vec2(0, 0));
	addChild(bg);

	//��������
	auto board = Sprite::create("board.png");
	board->setAnchorPoint(Vec2(0, 1));
	board->setPosition(Vec2(0, visibleSize.height));
	board->setOpacity(80);  //����255
	addChild(board);

	//��ʼ��������ӱ�ʯ����һ�γ�ʼ��ǰҪ��ն�ά����
	memset(m_jewels, 0, sizeof(m_jewels));
	initGrid();

	//�����ã����¿�ʼ��ť
	auto restarItem = MenuItemFont::create("restart", CC_CALLBACK_0(GameScene::initGrid, this));
	restarItem->setPositionY(-100);
	auto menu = Menu::create(restarItem, nullptr);
	addChild(menu);

	return true;
}

void GameScene::initGrid()
{
	clearAllJewels();

	//������ʯ,��������Ⱦ��
	for (int i = 0; i < 8; i++)
		for (int j = 0; j < 8; j++)
		{
			//�������ָ��
			m_jewels[j][i] = addAJewel(j, i); //��j�����У�i�����У����j��X�ᣬi��Y��
		}
}

Jewel* GameScene::addAJewel(int x, int y)
{
	//���������ʯ���ͣ������ʯ���Ϸ������´���ֱ���Ϸ�Ϊֹ������������Ϸ����λ�ã�������Ⱦ��
	Jewel* jewel = nullptr;
	while(1)
	{
		jewel = Jewel::createByType(random(1, 7));
		
		if (isJewelLegal(jewel, x, y))
			break;

		jewel->removeFromParent();
	}	

	jewel->setPos(x, y);
	addChild(jewel);

	log("add a jewel!---type:%d---x:%d---y:%d", jewel->getType(), x, y);

	return jewel;
}

bool GameScene::isJewelLegal(Jewel* jewel, int x, int y)
{
	if (!jewel) return false;

	//������ϱȽ��Ƿ�����������ϵ�������Ͻ�Ϊԭ�㣬x��y��Ϊ��
	//���x�������y����С�ڵ���1����ô�ñ�ʯ�ڴ���϶��ǺϷ��ģ���Ҫ����ͬʱ�Ϸ�����
	bool isBackLegal = true;
	bool isUpLegal = true;

	if (x > 1)
	{
		//���ֱ�Ƚ���λ�������ʯ���Ͳ�һ������ô���غϷ�
		if (jewel->getType() == m_jewels[x-1][y]->getType() && 
			jewel->getType() == m_jewels[x-2][y]->getType() &&
			m_jewels[x-2][y]->getType() == m_jewels[x-1][y]->getType())
		{
			isBackLegal = false;
		}
		else
			isBackLegal = true;
	}
	
	if (y > 1)
	{
		//���Ϸֱ�Ƚ���λ�������ʯ���Ͳ�һ������ô���غϷ�
		if (jewel->getType() == m_jewels[x][y-1]->getType() && 
			jewel->getType() == m_jewels[x][y-2]->getType() &&
			m_jewels[x][y-1]->getType() == m_jewels[x][y-2]->getType())
		{
			isUpLegal = false;
		}
		else
			isUpLegal = true;
	}

	return isBackLegal && isUpLegal;
}

void GameScene::clearAllJewels()
{
	for (int i = 0; i < 8; i++)
		for (int j = 0; j < 8; j++)
		{
			if (m_jewels[i][j])
				m_jewels[i][j]->removeFromParent();
		}

	memset(m_jewels, 0, sizeof(m_jewels));
}