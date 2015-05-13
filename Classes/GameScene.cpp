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

	//Ԥ���ر�ʯ�ı߿�
	TextureCache::getInstance()->addImage("selection.png");
	//Ԥ���ر�ʯ
	TextureCache::getInstance()->addImage("jewel1.png");
	TextureCache::getInstance()->addImage("jewel2.png");
	TextureCache::getInstance()->addImage("jewel3.png");
	TextureCache::getInstance()->addImage("jewel4.png");
	TextureCache::getInstance()->addImage("jewel5.png");
	TextureCache::getInstance()->addImage("jewel6.png");
	TextureCache::getInstance()->addImage("jewel7.png");

	//���ر���ͼ
	auto bg = Sprite::create("bground1.png");
	bg->setAnchorPoint(Vec2(0, 0));
	addChild(bg);

	//���񱳾�
	auto board = Sprite::create("board.png");
	board->setAnchorPoint(Vec2(0, 1));
	board->setPosition(Vec2(0, visibleSize.height));
	board->setOpacity(80);  //����255
	addChild(board);

	//��ʼ����������
	auto grid = JewelsGrid::create(8, 8);
	addChild(grid);
	grid->setPosition(0, visibleSize.height - board->getContentSize().height);

	//�����ã����¿�ʼ��ť
	/*
	auto restarItem = MenuItemFont::create("restart", CC_CALLBACK_0(GameScene::initGrid, this));
	restarItem->setPositionY(-100);
	auto menu = Menu::create(restarItem, nullptr);
	addChild(menu);
	*/

	return true;
}