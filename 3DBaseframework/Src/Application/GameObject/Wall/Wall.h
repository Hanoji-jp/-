#pragma once

class Wall :public KdGameObject
{
public:

	Wall() {}
	~Wall() {}

	void Init()override;
	void DrawLit()override;

private:

	std::shared_ptr<KdModelData> m_spModel;

	Math::Vector3 m_pos;

};