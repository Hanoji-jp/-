#pragma once

class Clear : public KdGameObject
{
public:
	Clear(){}
	~Clear(){}

	void Init()		override;
	void DrawSprite() override;

private:

	KdTexture m_Tex;
};