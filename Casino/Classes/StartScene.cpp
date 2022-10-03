#include "StartScene.hpp"
#include "CoinSpawner.hpp"
#include "ui/CocosGUI.h"

USING_NS_CC;

Scene* StartScene::createScene()
{
    auto scene = Scene::create();
    StartScene* layer = StartScene::create();
    scene->addChild(layer);
    return scene;
}


// on "init" you need to initialize your instance
bool StartScene::init()
{
    if (!Layer::init())
    {
        return false;
    }
    
    CoinSpawner* coinSpawner = CoinSpawner::create("res/coin32x32.png", {50.0f, 50.0f});
    coinSpawner->setPosition({100.0f, 100.0f});
    this->addChild(coinSpawner);

    return true;
}

void StartScene::CreateScammer()
{
    
}

