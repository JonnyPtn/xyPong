#include "MainMenuState.hpp"
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
        PLAY_BUTTON = 2,
        QUIT_BUTTON = 3,
        VOL_BUTTON = 4
    };
}

MainMenuState::MainMenuState(xy::StateStack& ss, xy::State::Context& ctx) :
xy::State(ss, ctx),
m_scene(ctx.appInstance.getMessageBus(), "assets/MainMenu.xyscn")
{
    // using the editor for this scene
    m_scene.addSystem<xy::EditorSystem>(ctx.appInstance.getMessageBus());
    
    // Scene has been loaded, but resources are uninitialised
    // Load assets to initialise them
    m_fonts.get("assets/kenney_fontpackage/Fonts/Kenney Blocks.ttf");
    m_fonts.get("assets/kenney_fontpackage/Fonts/Kenney Future Narrow.ttf");
    m_textures.get("assets/gameicons/Spritesheet/sheet_white2x.png");
    
    // quit button
    auto q = m_scene.getEntity(QUIT_BUTTON);
    auto& u = q.getComponent<xy::UIHitBox>();
    u.callbacks[xy::UIHitBox::MouseDown] =
    m_scene.getSystem<xy::UISystem>().addMouseButtonCallback([](xy::Entity, sf::Uint64 flags)
        {
            if (flags & xy::UISystem::LeftMouse)
            {
                xy::App::getActiveInstance()->quit();
            }
        });
    
    // Play button
    auto p = m_scene.getEntity(PLAY_BUTTON);
    p.getComponent<xy::UIHitBox>().callbacks[xy::UIHitBox::MouseDown] =
    m_scene.getSystem<xy::UISystem>().addMouseButtonCallback([this](xy::Entity, sf::Uint64 flags)
        {
            if (flags & xy::UISystem::LeftMouse)
            {
                requestStackPop();
                requestStackPush(States::SINGLE_PLAYER);
            }
        });
    
    // Volume button
    auto v = m_scene.getEntity(VOL_BUTTON);
    v.getComponent<xy::UIHitBox>().callbacks[xy::UIHitBox::MouseDown] =
    m_scene.getSystem<xy::UISystem>().addMouseButtonCallback([](xy::Entity e, sf::Uint64 flags)
        {
            if (flags & xy::UISystem::LeftMouse)
            {
                if (xy::AudioMixer::getMasterVolume() == 0.f)
                {
                    xy::AudioMixer::setMasterVolume(1.f);
                    e.getComponent<xy::SpriteAnimation>().play(0);
                }
                else
                {
                    xy::AudioMixer::setMasterVolume(0.f);
                    e.getComponent<xy::SpriteAnimation>().play(1);
                }
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
    q.getComponent<xy::UIHitBox>().callbacks[xy::UIHitBox::MouseEnter] = greyoutFunc;
    p.getComponent<xy::UIHitBox>().callbacks[xy::UIHitBox::MouseEnter] = greyoutFunc;
    v.getComponent<xy::UIHitBox>().callbacks[xy::UIHitBox::MouseEnter] = greyoutFunc;
    
    q.getComponent<xy::UIHitBox>().callbacks[xy::UIHitBox::MouseExit] = whiteoutFunc;
    p.getComponent<xy::UIHitBox>().callbacks[xy::UIHitBox::MouseExit] = whiteoutFunc;
    v.getComponent<xy::UIHitBox>().callbacks[xy::UIHitBox::MouseExit] = whiteoutFunc;
}

bool MainMenuState::handleEvent(const sf::Event &evt)
{
    m_scene.getSystem<xy::UISystem>().handleEvent(evt);
    m_scene.forwardEvent(evt);
    return true;
}

void MainMenuState::handleMessage(const xy::Message & msg)
{
    m_scene.forwardMessage(msg);
}

bool MainMenuState::update(float dt)
{
    m_scene.update(dt);
    return false;
}

void MainMenuState::draw()
{
    auto rt = xy::App::getActiveInstance()->getRenderTarget();
    rt->draw(m_scene);
}

xy::StateID MainMenuState::stateID() const
{
    return States::MAIN_MENU;
}
