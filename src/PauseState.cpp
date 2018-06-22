#include "PauseState.hpp"
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
#include <SFML/Window/Event.hpp>

// Entity ID's from the editor.
namespace
{
    enum Entities
    {
        RESUME_BUTTON = 4,
        MAIN_MENU_BUTTON = 3,
    };
}

PauseState::PauseState(xy::StateStack& ss, xy::State::Context& ctx) :
xy::State(ss, ctx),
m_scene(ctx.appInstance.getMessageBus(), "assets/PauseScene.xyscn")
{
    m_scene.addSystem<xy::EditorSystem>(ctx.appInstance.getMessageBus());
    
    m_fonts.get("assets/kenney_fontpackage/Fonts/Kenney Pixel Square.ttf");
    m_fonts.get("assets/kenney_fontpackage/Fonts/Kenney Blocks.ttf");
    
    // resume button
    auto r = m_scene.getEntity(RESUME_BUTTON);
    auto& u = r.getComponent<xy::UIHitBox>();
    u.callbacks[xy::UIHitBox::MouseDown] =
    m_scene.getSystem<xy::UISystem>().addMouseButtonCallback([this](xy::Entity, sf::Uint64 flags)
             {
                 if (flags & xy::UISystem::LeftMouse)
                 {
                     requestStackPop();
                 }
             });
    
    // main menu button
    auto m = m_scene.getEntity(MAIN_MENU_BUTTON);
    m.getComponent<xy::UIHitBox>().callbacks[xy::UIHitBox::MouseDown] =
    m_scene.getSystem<xy::UISystem>().addMouseButtonCallback([this](xy::Entity, sf::Uint64 flags)
             {
                 if (flags & xy::UISystem::LeftMouse)
                 {
                     requestStackClear();
                     requestStackPush(States::MAIN_MENU);
                 }
             });
    
    // greyout buttons on hover
    auto greyoutFunc = m_scene.getSystem<xy::UISystem>().addMouseMoveCallback([](xy::Entity e, sf::Vector2f pos)
              {
                  if (e.hasComponent<xy::Text>())
                  {
                      auto col = e.getComponent<xy::Text>().getFillColour();
                      e.getComponent<xy::Text>().setFillColour({col.r, col.g, col.b, 128});
                  }
                  if (e.hasComponent<xy::Sprite>())
                  {
                      auto col = e.getComponent<xy::Sprite>().getColour();
                      e.getComponent<xy::Sprite>().setColour({col.r, col.g, col.b, 128});
                  }
              });
    
    auto whiteoutFunc = m_scene.getSystem<xy::UISystem>().addMouseMoveCallback([](xy::Entity e, sf::Vector2f pos)
               {
                   if (e.hasComponent<xy::Text>())
                   {
                       auto col = e.getComponent<xy::Text>().getFillColour();
                       e.getComponent<xy::Text>().setFillColour({col.r, col.g, col.b, 255});
                   }
                   if (e.hasComponent<xy::Sprite>())
                   {
                       auto col = e.getComponent<xy::Sprite>().getColour();
                       e.getComponent<xy::Sprite>().setColour({col.r, col.g, col.b, 255});
                   }
               });
    m.getComponent<xy::UIHitBox>().callbacks[xy::UIHitBox::MouseEnter] = greyoutFunc;
    r.getComponent<xy::UIHitBox>().callbacks[xy::UIHitBox::MouseEnter] = greyoutFunc;
    
    m.getComponent<xy::UIHitBox>().callbacks[xy::UIHitBox::MouseExit] = whiteoutFunc;
    r.getComponent<xy::UIHitBox>().callbacks[xy::UIHitBox::MouseExit] = whiteoutFunc;
}

bool PauseState::handleEvent(const sf::Event &evt)
{
    switch(evt.type)
    {
        case sf::Event::KeyPressed:
        {
            switch(evt.key.code)
            {
                case sf::Keyboard::P:
                case sf::Keyboard::Escape:
                requestStackPop();
                return true;
            }
        }
    }
    
    m_scene.forwardEvent(evt);
    return true;
}

void PauseState::handleMessage(const xy::Message & msg)
{
    m_scene.forwardMessage(msg);
}

bool PauseState::update(float dt)
{
    m_scene.update(dt);
    return false;
}

void PauseState::draw()
{
    auto rt = xy::App::getActiveInstance()->getRenderTarget();
    rt->draw(m_scene);
}

xy::StateID PauseState::stateID() const
{
    return States::PAUSED;
}
