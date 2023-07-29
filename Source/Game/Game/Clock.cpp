#include "Clock.h"
#include <iostream>
#include "Player.h"

void Clock::Update(float dt) {
    Actor::Update(dt);
}

void Clock::OnCollision(Actor* other)
{
    if (other->m_tag == "Player") {
        m_destroyed = true;
        dynamic_cast<Player*>(other)->AddBonsuTime(m_time);
    }

}
