#include "Jewel.h"

Jewel* Jewel::createByType(int type)
{
	auto jewel = new Jewel();

	if (jewel && jewel->init(type))
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

bool Jewel::init(int type)
{
	Sprite::init();

	this->m_type = type;
	
	//������Դ����ʼ����������ԴӦ������Ϸ�������������ϵ�
	char name[100] = {0};
	sprintf(name, "jewel%d.png", m_type);
	this->initWithTexture(TextureCache::getInstance()->getTextureForKey(name));

	this->setAnchorPoint(Vec2(0, 0)); //���½�Ϊê��

	//����߿򣬲��Ҳ��ɼ�
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