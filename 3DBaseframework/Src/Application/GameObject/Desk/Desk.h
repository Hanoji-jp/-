#pragma once

class Desk :public KdGameObject
{
public:

	Desk() {}
	~Desk() {}

	void Init()override;
	void DrawLit()override;

private:

	std::shared_ptr<KdModelData> m_spModel;

	Math::Vector3 m_pos;

};