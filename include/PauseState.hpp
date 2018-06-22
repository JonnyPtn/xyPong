#pragma once

#include <xyginext/core/State.hpp>
#include <xyginext/core/MessageBus.hpp>
#include <xyginext/ecs/Scene.hpp>

class PauseState : public xy::State
{
public:
    PauseState(xy::StateStack&, xy::State::Context&);
    
    bool handleEvent(const sf::Event &evt) override;
    
    void handleMessage(const xy::Message &) override;
    
    bool update(float dt) override;
    
    void draw() override;
    
    xy::StateID stateID() const override;
    
private:
    xy::Scene m_scene;
    xy::FontResource m_fonts;
    xy::TextureResource m_textures;
};
