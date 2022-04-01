#include "TitleState.hpp"
#include "SpriteNode.h"
#include "Game.hpp"

TitleState::TitleState(StateStack* stack, Context* context)
    : State(stack, context)
{
    mAllRitems.clear();
    mContext->game->ResetFrameResources();
    mContext->game->BuildMaterials();

    std::unique_ptr<SpriteNode> backgroundSprite = std::make_unique<SpriteNode>(this);
    backgroundSprite->SetMatGeoDrawName("StarWars_Title", "boxGeo", "box");
    backgroundSprite->setScale(60, 1.0, 50.0);
    backgroundSprite->setPosition(0, 0, 0);
    mSceneGraph->attachChild(std::move(backgroundSprite));
    
    mSceneGraph->build();
    mContext->game->BuildFrameResources(mAllRitems.size());
}

TitleState::~TitleState()
{
}

void TitleState::draw()
{
    mSceneGraph->draw();
}

bool TitleState::update(const GameTimer& gt)
{
    mSceneGraph->update(gt);

    return true;
}

bool TitleState::handleEvent(WPARAM btnState)
{
    // Any Key
    //
    requestStackPop();
    requestStackPush(States::Menu);
    return true;
}

bool TitleState::handleRealtimeInput()
{
    return true;
}
