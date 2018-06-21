#pragma once

#include <xyginext/core/State.hpp>
#include <xyginext/core/MessageBus.hpp>
#include <xyginext/ecs/Scene.hpp>

class PlayingState : public xy::State
{
public:
    PlayingState(xy::StateStack&, xy::State::Context&, bool singlePlayer);
    
    bool handleEvent(const sf::Event &evt) override;
    
    void handleMessage(const xy::Message &) override;
    
    bool update(float dt) override;
    
    void draw() override;
    
    xy::StateID stateID() const override;
    
private:
    xy::Scene m_scene;
    xy::FontResource m_fonts;
    xy::TextureResource m_textures;
    bool m_singlePlayer = true;
};
