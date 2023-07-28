#include "Enemy.h"
#include "Player.h"
#include "DrivingGame.h"
#include "Framework/Scene.h"
#include "Framework/Emitter.h"
#include "Renderer/Renderer.h"
#include <iostream>

void Enemy::Update(float dt) {
    Actor::Update(dt);

    Player* player = m_scene->GetActor<Player>();

    if (player) 
    {
        float playerDistance = m_transform.position.Distance(player->m_transform.position);

        m_currentSpeed = kiko::Mag(m_velocity.x, m_velocity.y);

        // BRAKE
        if (playerDistance < 10) {
            if (m_drive > -1) m_drive -= m_brakePower;
        }
        // GAS
        else if (playerDistance > 300 && m_currentSpeed < 100) {
            if (m_drive < 1) m_drive += m_acceleration;
        }
        // Cruising
        else {
            if (m_drive < 0.001 && m_drive > -0.001) m_drive = 0;
            else if (m_drive > 0) m_drive -= 0.005;
            else if (m_drive < 0) m_drive += 0.005;
        }
        kiko::vec2 forward = kiko::vec2{ 0, -1 }.Rotate(m_transform.rotation);

        float angle = kiko::Vector2::Angle(forward.Normalized(), (player->m_transform.position - m_transform.position).Normalized());
        float degreesAngle = kiko::RadToDeg(angle);

        //std::cout << "Player Distance: " << playerDistance << "\n"
         //   << "angle: " << angle << "\n"
         //   << "degrees angle: " << degreesAngle << "\n";
        //std::cout << m_drive << "\n";

        if ((degreesAngle > 30 && degreesAngle < 150) && m_rotate > -1) m_rotate -= 0.05f;

        else if ((degreesAngle > 240 && degreesAngle < 330) && m_rotate < 1) m_rotate += 0.05f;

        else if (m_currentSpeed) {
            // Bring rotate back to zero
            if (m_rotate < 0.001 && m_rotate > -0.001) m_rotate = 0;
            else if (m_rotate > 0) m_rotate -= 0.01f;
            else if (m_rotate < 0) m_rotate += 0.01f;
        }
        

        // Steer Rotations
        if (m_currentSpeed != 0) m_transform.rotation += (m_rotate * ((m_currentSpeed / 100) * m_drive)) * m_turnRate * kiko::g_time.GetDeltaTime();

        // Move accordingly
        AddForce(forward * m_enginePower * m_drive * kiko::g_time.GetDeltaTime());

        // Wrapping
        m_transform.position.x = kiko::Wrap(m_transform.position.x, kiko::g_renderer.GetWidth());
        m_transform.position.y = kiko::Wrap(m_transform.position.y, kiko::g_renderer.GetHeight());
    }
}

void Enemy::OnCollision(Actor* other)
{
    if (other->m_tag == "Player") {
        m_game->AddPoints(100);

        // Apply force from other car
        AddForce(other->GetVelocity() / 2);

        m_health--;
        if (m_health <= 0) {
            // Death Particles
            kiko::EmitterData data;
            data.burst = true;
            data.burstCount = 300;
            data.spawnRate = 10000;
            data.angle = 0;
            data.angleRange = kiko::Pi;
            data.lifetimeMin = 0.5f;
            data.lifetimeMax = 1.0f;
            data.speedMin = 300;
            data.speedMax = 600;
            data.damping = 0.2f;
            data.color = kiko::Color{ 1, 0, 0, 1 };
            kiko::Transform transform{ { m_transform.position }, m_transform.rotation, 1 };
            auto emitter = std::make_unique<kiko::Emitter>(transform, data);
            emitter->m_lifespan = 0.5f;
            m_scene->Add(std::move(emitter));


            m_destroyed = true;

        }
    }
}
