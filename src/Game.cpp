#include "Game.h"
#include "Physics.hpp"

#include <iostream>
#include <random>

Game::Game(const std::string& config)
{
	init(config);
}

void Game::init(const std::string& path)
{
	m_window.create(sf::VideoMode({ m_windowConfig.uW, m_windowConfig.uH }), "Mega Mario");
	m_window.setFramerateLimit(m_windowConfig.FR);

	ImGui::SFML::Init(m_window);

	ImGui::GetStyle().ScaleAllSizes(2.0f);
	ImGui::GetIO().FontGlobalScale = 2.0f;

	// int SR, CR, FR, FG, FB, OR, OG, OB, OT, V; float S
	m_playerConfig = { 4, 4, 255, 0, 0, 255, 0, 0, 1, 12, 1.0f };
	// int SR, CR, FR, FG, FB, OR, OG, OB, OT, V, L; float S
	m_bulletConfig = { 6, 6, 250, 250, 250, 250, 250, 250, 2, 12, 100, 15.0f };
	// int SR, CR, OR, OG, OB, OT, VMIN, VMAX, L, SI; float SMIN, SMAX
	m_enemyConfig = { 40, 40, 250, 10, 10, 3, 3, 7, 60, 120, 2.0f, 6.0f };

	spawnPlayer();
	spawnEnemy();
	for (int i = 0; i < m_numRays; i++)
	{
		auto line = m_entities.addEntity("playerLine");
		line->add<CLine>(Vec2f(0, 0), Vec2f(0, 0)); // dummy points, will be updated
	}

}

std::shared_ptr<Entity> Game::player()
{
	auto& player = m_entities.getEntities("player");
	assert(player.size() == 1);
	return player.front();
}

void Game::run()
{
	while (m_running)
	{
		m_entities.update();

		ImGui::SFML::Update(m_window, m_deltaClock.restart());

		if (!m_paused)
		{
			sEnemySpawner();
			sMovement();
			sLifespan();
			sCollision();
		}
		sUserInput();
		sGUI();
		sRender();

		m_currentFrame++;
	}
	m_window.close();
}

void Game::spawnPlayer()
{
	auto player = m_entities.addEntity("player");

	player->add<CTransform>(Vec2f(m_windowConfig.fW, m_windowConfig.fH) / 2,
		Vec2f(0.0f, 0.0f), 0.0f);
	player->add<CShape>(m_playerConfig.SR, m_playerConfig.V,
		sf::Color(m_playerConfig.FR, m_playerConfig.FG, m_playerConfig.FB),
		sf::Color(m_playerConfig.OR, m_playerConfig.OG, m_playerConfig.OB),
		m_playerConfig.OT);
	player->add<CInput>();
}

void Game::spawnEnemy()
{
	{
		Vec2f tl(0, 0), bl(m_windowConfig.fW, 0), tr(0, m_windowConfig.fH), br(m_windowConfig.fW, m_windowConfig.fH);
		auto l1 = m_entities.addEntity("line"); l1->add<CLine>(tl, tr);
		auto l2 = m_entities.addEntity("line"); l2->add<CLine>(tr, br);
		auto l3 = m_entities.addEntity("line"); l3->add<CLine>(br, bl);
		auto l4 = m_entities.addEntity("line"); l4->add<CLine>(bl, tl);
	}

	// Quad 1 - Skewed trapezoid
	{
		Vec2f tl(100, 100), tr(300, 80), br(280, 250), bl(120, 270);
		auto l1 = m_entities.addEntity("line"); l1->add<CLine>(tl, tr);
		auto l2 = m_entities.addEntity("line"); l2->add<CLine>(tr, br);
		auto l3 = m_entities.addEntity("line"); l3->add<CLine>(br, bl);
		auto l4 = m_entities.addEntity("line"); l4->add<CLine>(bl, tl);
	}

	// Quad 2 - Irregular with diagonal skew
	{
		Vec2f tl(450, 100), tr(650, 120), br(600, 280), bl(470, 260);
		auto l1 = m_entities.addEntity("line"); l1->add<CLine>(tl, tr);
		auto l2 = m_entities.addEntity("line"); l2->add<CLine>(tr, br);
		auto l3 = m_entities.addEntity("line"); l3->add<CLine>(br, bl);
		auto l4 = m_entities.addEntity("line"); l4->add<CLine>(bl, tl);
	}

	// Quad 3 - Long and narrow diamond shape
	{
		Vec2f tl(800, 150), tr(950, 100), br(1000, 250), bl(850, 300);
		auto l1 = m_entities.addEntity("line"); l1->add<CLine>(tl, tr);
		auto l2 = m_entities.addEntity("line"); l2->add<CLine>(tr, br);
		auto l3 = m_entities.addEntity("line"); l3->add<CLine>(br, bl);
		auto l4 = m_entities.addEntity("line"); l4->add<CLine>(bl, tl);
	}

	// Quad 4 - Irregular lower left
	{
		Vec2f tl(150, 500), tr(330, 490), br(310, 700), bl(100, 720);
		auto l1 = m_entities.addEntity("line"); l1->add<CLine>(tl, tr);
		auto l2 = m_entities.addEntity("line"); l2->add<CLine>(tr, br);
		auto l3 = m_entities.addEntity("line"); l3->add<CLine>(br, bl);
		auto l4 = m_entities.addEntity("line"); l4->add<CLine>(bl, tl);
	}

	// Quad 5 - Irregular lower right
	{
		Vec2f tl(1100, 500), tr(1300, 530), br(1250, 720), bl(1080, 700);
		auto l1 = m_entities.addEntity("line"); l1->add<CLine>(tl, tr);
		auto l2 = m_entities.addEntity("line"); l2->add<CLine>(tr, br);
		auto l3 = m_entities.addEntity("line"); l3->add<CLine>(br, bl);
		auto l4 = m_entities.addEntity("line"); l4->add<CLine>(bl, tl);
	}

}

void Game::spawnSmallEnemies(std::shared_ptr<Entity> entity)
{

}

void Game::spawnBullet(std::shared_ptr<Entity> entity, const Vec2f& target)
{

}

void Game::spawnSpecialWeapon(std::shared_ptr<Entity> entity, const Vec2f& target)
{

}

void Game::sMovement()
{
	auto& playerTransform = player()->get<CTransform>();
	auto& playerInput = player()->get<CInput>();

	playerTransform.pos = playerInput.mousePos;
}

void Game::sLifespan()
{

}

void Game::sCollision()
{
	if (!player()->get<CInput>().mouseMoved)
	{
		return;
	}
	int i = 0;
	for (auto& playerLine : m_entities.getEntities("playerLine"))
	{
		float minT = 1000;
		Vec2f minPoint = player()->get<CTransform>().pos;

		for (auto& entity : m_entities.getEntities())
		{
			if (entity->tag() == "playerLine" || !entity->has<CLine>()) continue;

			constexpr float DEG2RAD = 3.14159265f / 180.0f;
			Vec2f rayDir = Vec2f(std::cos(i * 360.0f * DEG2RAD / m_numRays), std::sin(i * 360.0f * DEG2RAD / m_numRays));
			auto& line = entity->get<CLine>().line;

			Intersect intersect = Physics::LineIntersect(
				player()->get<CTransform>().pos,
				player()->get<CTransform>().pos + rayDir,
				line.front().position, line.back().position
			);

			if (intersect.intersected && intersect.t < minT)
			{
				minT = intersect.t;
				minPoint = intersect.point;
			}
		}

		auto& line = playerLine->get<CLine>().line;
		line[0].position = player()->get<CTransform>().pos;
		line[1].position = minPoint;
		line[0].color = sf::Color::Red;
		line[1].color = sf::Color::Red;

		++i;
	}

}

void Game::sEnemySpawner()
{
}

void Game::sGUI()
{
}

void Game::sRender()
{
	m_window.clear();

	for (auto& entity : m_entities.getEntities())
	{
		if (entity->has<CTransform>() && entity->has<CShape>())
		{
			auto& entityTransform = entity->get<CTransform>();
			auto& entityShape = entity->get<CShape>();

			entityShape.circle.setPosition(entityTransform.pos);
			entityShape.circle.setRotation(sf::degrees(entityTransform.angle));

			m_window.draw(entityShape.circle);
		}

		if (entity->has<CLine>())
		{
			auto& line = entity->get<CLine>().line;
			m_window.draw(line.data(), line.size(), sf::PrimitiveType::Lines);
		}
	}
	m_window.draw(m_text);

	ImGui::SFML::Render(m_window);

	m_window.display();
}

void Game::sUserInput()
{
	while (const std::optional event = m_window.pollEvent())
	{
		ImGui::SFML::ProcessEvent(m_window, *event);
		
		if (event->is<sf::Event::Closed>())
		{
			m_running = false;
		}
		
		auto& playerInput = player()->get<CInput>();
		if (const auto* mouseMoved = event->getIf<sf::Event::MouseMoved>())
		{
			playerInput.mouseMoved = true;
			playerInput.mousePos = Vec2f(mouseMoved->position.x, mouseMoved->position.y);
		}
		else
		{
			playerInput.mouseMoved = false;
		}

		if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>())
		{
			switch (keyPressed->scancode)
			{
			case sf::Keyboard::Scancode::Escape:
				setPaused(!m_paused);
				break;
			case sf::Keyboard::Scancode::W:
				playerInput.up = true;
				break;
			case sf::Keyboard::Scancode::A:
				playerInput.left = true;
				break;
			case sf::Keyboard::Scancode::S:
				playerInput.down = true;
				break;
			case sf::Keyboard::Scancode::D:
				playerInput.right = true;
				break;
			default: break;
			}
		}

		if (const auto* keyReleased = event->getIf<sf::Event::KeyReleased>())
		{
			switch (keyReleased->scancode)
			{
			case sf::Keyboard::Scancode::Escape:
				break;
			case sf::Keyboard::Scancode::W:
				playerInput.up = false;
				break;
			case sf::Keyboard::Scancode::A:
				playerInput.left = false;
				break;
			case sf::Keyboard::Scancode::S:
				playerInput.down = false;
				break;
			case sf::Keyboard::Scancode::D:
				playerInput.right = false;
				break;
			default: break;
			}
		}
	}
}

void Game::setPaused(bool paused)
{
	m_paused = paused;
}