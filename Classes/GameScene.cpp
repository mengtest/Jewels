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
	//Ԥ���ر���ͼ
	TextureCache::getInstance()->addImage("bground1.png");
	TextureCache::getInstance()->addImage("bground2.png");
	TextureCache::getInstance()->addImage("bground3.png");
	TextureCache::getInstance()->addImage("bground4.png");

	auto visibleSize = Director::getInstance()->getVisibleSize();

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

	//��ʼ���������ݣ������ԭ�������½�
	auto grid = JewelsGrid::create(8, 8);
	addChild(grid);
	grid->setPosition(0, visibleSize.height - grid->getRow() * GRID_WIDTH);

	return true;
}