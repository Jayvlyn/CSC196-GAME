#pragma once
#include "Framework/Actor.h"

class Player : public kiko::Actor {

public:

	Player(float enginePower, float acceleration, float brakePower, float turnRate, const kiko::Transform& transform, std::shared_ptr<kiko::Model> model) :
		kiko::Actor{ transform, model },
		m_enginePower{ enginePower },
		m_acceleration{ acceleration },
		m_brakePower{ brakePower},
		m_turnRate{ turnRate }
	{}

	void Update(float dt) override;
	void OnCollision(Actor* other) override;

	float GetHealth();

private:
	
	float m_enginePower = 0;
	float m_currentSpeed = 0;
	float m_acceleration = 0; 
	float m_brakePower = 0; 
	float m_turnRate = 0;
	float m_drive = 0;
	
	float m_rotate = 0;

	float m_health = 500;

	float m_timeScale = 1;

};
