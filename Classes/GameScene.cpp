#include "GameScene.h"
#include "JewelsGrid.h"

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

	//预加载宝石的边框
	TextureCache::getInstance()->addImage("selection.png");
	//预加载宝石
	TextureCache::getInstance()->addImage("jewel1.png");
	TextureCache::getInstance()->addImage("jewel2.png");
	TextureCache::getInstance()->addImage("jewel3.png");
	TextureCache::getInstance()->addImage("jewel4.png");
	TextureCache::getInstance()->addImage("jewel5.png");
	TextureCache::getInstance()->addImage("jewel6.png");
	TextureCache::getInstance()->addImage("jewel7.png");

	//加载背景图
	auto bg = Sprite::create("bground1.png");
	bg->setAnchorPoint(Vec2(0, 0));
	addChild(bg);

	//网格背景
	auto board = Sprite::create("board.png");
	board->setAnchorPoint(Vec2(0, 1));
	board->setPosition(Vec2(0, visibleSize.height));
	board->setOpacity(80);  //满分255
	addChild(board);

	//初始化网格数据
	auto grid = JewelsGrid::create(8, 8);
	addChild(grid);
	grid->setPosition(0, visibleSize.height - board->getContentSize().height);

	//测试用：重新开始按钮
	/*
	auto restarItem = MenuItemFont::create("restart", CC_CALLBACK_0(GameScene::initGrid, this));
	restarItem->setPositionY(-100);
	auto menu = Menu::create(restarItem, nullptr);
	addChild(menu);
	*/

	return true;
}