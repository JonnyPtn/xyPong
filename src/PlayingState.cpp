#include "PlayingState.hpp"
#include "States.hpp"

#include <xyginext/ecs/systems/UISystem.hpp>

#include <xyginext/ecs/components/UIHitBox.hpp>
#include <xyginext/ecs/components/Text.hpp>
#include <xyginext/ecs/components/Drawable.hpp>
#include <xyginext/ecs/components/Transform.hpp>
#include <xyginext/ecs/components/Sprite.hpp>
#include <xyginext/ecs/components/SpriteAnimation.hpp>

#include <xyginext/core/Editor.hpp>

#include <SFML/Window/Mouse.hpp>

// Entity ID's from the editor.
namespace
{
    enum Entities
    {
        PLAYER_ONE = 4,
        PLAYER_TWO = 5,
        SCORE_ONE = 2,
        SCORE_TWO = 3
    };
}

PlayingState::PlayingState(xy::StateStack& ss, xy::State::Context& ctx, bool singlePlayer) :
xy::State(ss, ctx),
m_scene(ctx.appInstance.getMessageBus(), "assets/Game.xyscn"),
m_singlePlayer(singlePlayer)
{
    m_scene.addSystem<xy::EditorSystem>(ctx.appInstance.getMessageBus());
    
    m_fonts.get("assets/kenney_fontpackage/Fonts/Kenney Blocks.ttf");
}

bool PlayingState::handleEvent(const sf::Event &evt)
{
    m_scene.forwardEvent(evt);
    return true;
}

void PlayingState::handleMessage(const xy::Message & msg)
{
    m_scene.forwardMessage(msg);
}

bool PlayingState::update(float dt)
{
    auto playerOne = m_scene.getEntity(PLAYER_ONE);
    auto playerTwo = m_scene.getEntity(PLAYER_TWO);
    
    const auto paddleSpeed = 500.f;
    
    // Handle Input
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::W))
    {
        playerOne.getComponent<xy::Transform>().move(0.f, -paddleSpeed * dt);
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::S))
    {
        playerOne.getComponent<xy::Transform>().move(0.f, paddleSpeed * dt);
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
    {
        playerTwo.getComponent<xy::Transform>().move(0.f, -paddleSpeed * dt);
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
    {
        playerTwo.getComponent<xy::Transform>().move(0.f, paddleSpeed * dt);
    }
    
    // Make sure players don't go off screen
    auto limitPlayerEntity = [this](xy::Entity player)
    {
        auto localBounds = player.getComponent<xy::Drawable>().getLocalBounds();
        auto worldTransform = player.getComponent<xy::Transform>().getWorldTransform();
        auto globalBounds = worldTransform.transformRect(localBounds);
        if (globalBounds.top < 0)
        {
            player.getComponent<xy::Transform>().move(0.f, - globalBounds.top);
        }
        else if (globalBounds.top + globalBounds.height > getContext().defaultView.getSize().y)
        {
            auto offset = globalBounds.top + globalBounds.height - getContext().defaultView.getSize().y;
            player.getComponent<xy::Transform>().move(0.f, -offset);
        }
    };
    limitPlayerEntity(playerOne);
    limitPlayerEntity(playerTwo);
    
    m_scene.update(dt);
    
    return false;
}

void PlayingState::draw()
{
    auto rt = xy::App::getActiveInstance()->getRenderTarget();
    rt->draw(m_scene);
}

xy::StateID PlayingState::stateID() const
{
    return m_singlePlayer ? States::SINGLE_PLAYER : States::TWO_PLAYER;
}
