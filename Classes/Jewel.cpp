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
	
	//������Դ����ʼ������
	char name[100] = {0};
	sprintf(name, "jewel%d.png", m_type);
	this->initWithFile(name);

	this->setAnchorPoint(Vec2(0, 1)); //���Ͻ�Ϊê��

	return true;
}