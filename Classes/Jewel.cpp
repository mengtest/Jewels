#include "Jewel.h"
#include "JewelsGrid.h"

Jewel* Jewel::createByType(int type, int x, int y)
{
	auto jewel = new Jewel();

	if (jewel && jewel->init(type, x, y))
	{
		jewel->autorelease();
		return jewel;
	}
	else
	{
		CC_SAFE_DELETE(jewel);
		return nullptr;
	}
}

bool Jewel::init(int type, int x, int y)
{
	Sprite::init();

	m_type = type;
	m_x = x;
	m_y = y;
	
	//根据资源名初始化纹理，该资源应该在游戏主场景里加载完毕的
	char name[100] = {0};
	sprintf(name, "jewel%d.png", m_type);
	this->initWithTexture(TextureCache::getInstance()->getTextureForKey(name));

	this->setAnchorPoint(Vec2(0, 0)); //左下角为锚点

	//加入边框，并且不可见
	m_selection = Sprite::createWithTexture(TextureCache::getInstance()->getTextureForKey("selection.png"));
	m_selection->setAnchorPoint(Vec2(0, 0));
	m_selection->setOpacity(0);
	addChild(m_selection);

	return true;
}

void Jewel::showSelection()
{
	auto fadein = FadeIn::create(0.2);
	m_selection->runAction(fadein);
}

void Jewel::hideSelection()
{
	auto fadeout = FadeOut::create(0.2);
	m_selection->runAction(fadeout);
}