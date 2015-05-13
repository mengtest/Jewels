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

	//加载背景图
	auto bg = Sprite::create("bground1.png");
	bg->setAnchorPoint(Vec2(0, 0));
	addChild(bg);

	//背景网格
	auto board = Sprite::create("board.png");
	board->setAnchorPoint(Vec2(0, 1));
	board->setPosition(Vec2(0, visibleSize.height));
	board->setOpacity(80);  //满分255
	addChild(board);

	//初始化网格，添加宝石，第一次初始化前要清空二维数组
	memset(m_jewels, 0, sizeof(m_jewels));
	initGrid();

	//测试用：重新开始按钮
	auto restarItem = MenuItemFont::create("restart", CC_CALLBACK_0(GameScene::initGrid, this));
	restarItem->setPositionY(-100);
	auto menu = Menu::create(restarItem, nullptr);
	addChild(menu);

	return true;
}

void GameScene::initGrid()
{
	clearAllJewels();

	//创建宝石,并加入渲染树
	for (int i = 0; i < 8; i++)
		for (int j = 0; j < 8; j++)
		{
			//保存对象指针
			m_jewels[j][i] = addAJewel(j, i); //用j代表列，i代表行，因此j是X轴，i是Y轴
		}
}

Jewel* GameScene::addAJewel(int x, int y)
{
	//随机创建宝石类型（如果宝石不合法，重新创建直到合法为止），设置其游戏像素位置，加入渲染树
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

	//向后向上比较是否三连，坐标系是以左上角为原点，x右y下为正
	//如果x坐标或者y坐标小于等于1，那么该宝石在此轴肯定是合法的，但要两轴同时合法才行
	bool isBackLegal = true;
	bool isUpLegal = true;

	if (x > 1)
	{
		//向后分别比较两位，如果宝石类型不一样，那么返回合法
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
		//向上分别比较两位，如果宝石类型不一样，那么返回合法
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