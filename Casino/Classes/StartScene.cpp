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
    
    SpriteFrameCache* cache = SpriteFrameCache::getInstance();
    cache->addSpriteFramesWithFile("res/sprites.plist");
    
    std::vector<std::string> slotImages = {"slot_apple_40x46.png", "slot_banana_54x42.png", "slot_cherry_42x42.png", "slot_orange_50x40.png", "slot_strawberry_50x44.png"};
    std::vector<std::string> slotNames = {"Apple", "Banana", "Cherry", "Orange", "Strawberry"};

    // Create'n'Setup the Scammer
    Scammer* scammer = Scammer::create(slotImages, slotNames, {70.0f, 55.0f});
    scammer->init();
    scammer->setAnchorPoint(Vec2::ANCHOR_BOTTOM_LEFT);
    scammer->setPosition({ 250.0f, 50.0f });
    addChild(scammer);
    
    // Create'n'Setup the CoinSpawner
    CoinSpawner* coinSpawner = CoinSpawner::create("coin23x26.png", {50.0f, 50.0f});
    coinSpawner->init();
    scammer->setAnchorPoint(Vec2::ANCHOR_BOTTOM_LEFT);
    coinSpawner->setPosition({ 150.0f, 140.0f });
    addChild(coinSpawner);
    
    // Create the texbox to display the prize
    m_stakeDisplay = Label::createWithSystemFont("", "arial", 18);
    m_stakeDisplay->setAnchorPoint(Vec2::ANCHOR_MIDDLE_BOTTOM);
    m_stakeDisplay->setPosition({Director::getInstance()->getWinSize().width/2, 270.0f});
    addChild(m_stakeDisplay);
    
    EventListenerCustom* customListener1 = EventListenerCustom::create("slots_rotation_finished_event", CC_CALLBACK_1(StartScene::onSlotsRotationFinish, this));
    EventListenerCustom* customListener2 = EventListenerCustom::create("slots_rotation_begin_event", CC_CALLBACK_1(StartScene::onSlotsRotationBegin, this));

    getEventDispatcher()->addEventListenerWithFixedPriority(customListener1, 1);
    getEventDispatcher()->addEventListenerWithFixedPriority(customListener2, 1);
    
    return true;
}


void StartScene::onSlotsRotationFinish(cocos2d::EventCustom *event)
{
    const char* prizeName = static_cast<const char*>(event->getUserData());
    m_stakeDisplay->setString("Congrats! Your stake is: " + std::string(prizeName));
}
