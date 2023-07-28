#include "DrivingGame.h"
#include "Player.h"
#include "Enemy.h"

#include "Framework/Scene.h"
#include "Framework/Emitter.h"

#include "Audio/AudioSystem.h"
#include "Input/InputSystem.h"

#include "Renderer/Renderer.h"
#include "Renderer/Text.h"
#include "Renderer/ModelManager.h"



bool DrivingGame::Init()
{
	// Create font / text objects
	m_font = std::make_shared<kiko::Font>("Coalition_v2..ttf", 24); // Yes it has two dots thats not a mis-type
	auto titleFont = std::make_shared<kiko::Font>("Coalition_v2..ttf", 48);

	m_healthText = std::make_unique<kiko::Text>(m_font);
	m_healthText->Create(kiko::g_renderer, "(Your health will be here, DONT CRASH!)", kiko::Color{ 1, 1, 1, 1});

	m_titleText = std::make_unique<kiko::Text>(titleFont);
	m_titleText->Create(kiko::g_renderer, "TIME DRIFTER", kiko::Color{ 1, 1, 1, 1});

	m_startPromptText = std::make_unique<kiko::Text>(m_font);
	m_startPromptText->Create(kiko::g_renderer, "Press [Space] to Start", kiko::Color{ 1, 1, 1, 1});

	m_deathText = std::make_unique<kiko::Text>(m_font);
	m_deathText->Create(kiko::g_renderer, "YOU ARE DEAD", kiko::Color{ 1, 0, 0, 1});

	m_timerText = std::make_unique<kiko::Text>(m_font);
	m_timerText->Create(kiko::g_renderer, "Time Survived: 0", kiko::Color{ 1, 1, 1, 1});

	// Load audio
	kiko::g_audioSystem.AddAudio("music", "TDOST1.wav");
	kiko::g_audioSystem.AddAudio("hit1", "Hit1.wav");
	kiko::g_audioSystem.AddAudio("hit2", "Hit2.wav");
	kiko::g_audioSystem.AddAudio("hit3", "Hit3.wav");
	kiko::g_audioSystem.PlayOneShot("music", true);

	// Scene
	m_scene = std::make_unique<kiko::Scene>();



	return true;
}

void DrivingGame::Shutdown(){}

void DrivingGame::Update(float dt)
{
	switch (m_state)
	{
	case eState::Title:
		if (kiko::g_inputSystem.GetKeyDown(SDL_SCANCODE_SPACE))
		{
			m_state = eState::StartGame;
		}
		break;
	case eState::StartGame:
		m_score = 0;
		m_state = eState::StartLevel;
		break;
	case eState::StartLevel:
		m_scene->RemoveAll();
	{
		std::unique_ptr<Enemy> enemy = std::make_unique<Enemy>(1200, 0.01, 1, kiko::DegToRad(kiko::randomf(20.0f, 40.0f)), kiko::Transform { { kiko::randomf(kiko::g_renderer.GetWidth()), kiko::randomf(kiko::g_renderer.GetHeight()) }, kiko::randomf(kiko::TwoPi), 9 }, kiko::g_modelManager.Get("EnemyCar.txt"));
		enemy->m_tag = "Enemy";
		enemy->m_game = this;
		m_scene->Add(std::move(enemy));

		std::unique_ptr<Player> player = std::make_unique<Player>(1000, 0.01, 0.005, kiko::DegToRad(25), kiko::Transform{ {500, 300}, 0, 10 }, kiko::g_modelManager.Get("Car.txt"));
		player->m_tag = "Player";
		player->SetDamping(0.95f);
		player->m_game = this;
		m_scene->Add(std::move(player));
	}
		m_state = eState::Game;
		break;
		
	case eState::Game:
		{
			m_spawnTimer += dt;
			if (m_spawnTimer >= m_spawnTime) {
				m_spawnTimer = 0;
				std::unique_ptr<Enemy> enemy = std::make_unique<Enemy>(1200, 0.01, 1, kiko::DegToRad(kiko::randomf(20.0f, 40.0f)), kiko::Transform { { kiko::randomf(kiko::g_renderer.GetWidth()), kiko::randomf(kiko::g_renderer.GetHeight()) }, kiko::randomf(kiko::TwoPi), 9 }, kiko::g_modelManager.Get("EnemyCar.txt"));
				enemy->m_tag = "Enemy";
				enemy->m_game = this;
				m_scene->Add(std::move(enemy));
			}
		}

		break;
	case eState::PlayerDeadStart:
		m_stateTimer = 3;
		if (m_lives == 0) m_state = eState::GameOver;
		else {
			m_state = eState::PlayerDead;
			m_lives--;
		}
		break;
	case eState::PlayerDead:
		m_stateTimer -= dt;
		if (m_stateTimer <= 0) {
			m_state = eState::StartLevel;
		}
		
		break;
	case eState::GameOver:
		m_stateTimer -= dt;
		if (m_stateTimer <= 0) {
			m_scene->RemoveAll();
			m_state = eState::Title;
		}
		
		break;
	default:
		break;
	}

	Player* player = m_scene->GetActor<Player>();
	if (player) {
		m_healthText->Create(kiko::g_renderer, std::to_string((int)player->GetHealth()), { 1,1,1,1 });
		m_timerText->Create(kiko::g_renderer, "Time Survived: " + std::to_string(m_gameTime), {1,1,1,1});
		m_gameTime += dt;
	}
	

	m_scene->Update(dt);
}

void DrivingGame::Draw(kiko::Renderer& renderer)
{
	if (m_state == eState::Title) {
		m_titleText->Draw(renderer, 40, renderer.GetHeight() / 2);
		m_startPromptText->Draw(renderer, 40, renderer.GetHeight() / 2 - 40);
	}
	else if (m_state == eState::PlayerDead) {
		m_deathText->Draw(renderer, renderer.GetWidth()/2, renderer.GetHeight()/2);
	}
	
	if (m_state != eState::Title) {
		m_timerText->Draw(renderer, 40, renderer.GetHeight()-50);
	}

	m_healthText->Draw(renderer, 40, 40);
	m_scene->Draw(renderer);
}
