#include "StartScene.hpp"
#include "CoinSpawner.hpp"
#include "Scammer.hpp"

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
    
    std::vector<std::string> slotImages = {"res/slot_apple_40x46.png", "res/slot_banana_54x42.png", "res/slot_cherry_42x42.png", "res/slot_orange_50x40.png", "res/slot_strawberry_50x44.png"};
    std::vector<std::string> slotNames = {"Apple", "Banana", "Cherry", "Orange", "Strawberry"};
    //std::vector<std::string> slotImages = {"res/green_rect_32x32.png", "res/green_rect_32x32.png", "res/green_rect_32x32.png", "res/green_rect_32x32.png", "res/green_rect_32x32.png"};
    
    // Create'n'Setup the Scammer
    Scammer* scammer = Scammer::create(slotImages, slotNames);
    scammer->init();
    scammer->setAnchorPoint(Vec2::ANCHOR_BOTTOM_LEFT);
    scammer->setPosition({ 250.0f, 100.0f });
    addChild(scammer);
    
    // Create'n'Setup the CoinSpawner
    CoinSpawner* coinSpawner = CoinSpawner::create("res/coin23x26.png", {50.0f, 50.0f});
    coinSpawner->init();
    scammer->setAnchorPoint(Vec2::ANCHOR_BOTTOM_LEFT);
    coinSpawner->setPosition({ 150.0f, 140.0f });
    addChild(coinSpawner);
    
    return true;
}

