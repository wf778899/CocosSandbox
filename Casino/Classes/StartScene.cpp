#include "StartScene.hpp"
#include "CoinSpawner.hpp"
#include "Scammer.hpp"


USING_NS_CC;

const cocos2d::Size  StartScene::s_scammerSlotSize  = {70.0f,  55.0f};
const cocos2d::Size  StartScene::s_coinSpawnerSize  = {50.0f,  50.0f};
const cocos2d::Vec2  StartScene::s_scammerPos       = {250.0f, 50.0f};
const int            StartScene::s_labelFontSize    = 18;

Scene* StartScene::createScene()
{
    auto scene = Scene::create();
    StartScene* layer = StartScene::create();
    scene->addChild(layer);
    return scene;
}

bool StartScene::init()
{
    SpriteFrameCache* cache = SpriteFrameCache::getInstance();
    cache->addSpriteFramesWithFile("res/sprites.plist");
    
    if(!Layer::init())
        return false;
    
    if(!createScammer())
        return false;
    
    if(!createCoinSpawner())
        return false;
    
    createLabel();
    
    subscribeToEvents();
    
    return true;
}

bool StartScene::createScammer()
{
    std::vector<std::string> slotImages = {"slot_apple_40x46.png", "slot_banana_54x42.png", "slot_cherry_42x42.png", "slot_orange_50x40.png", "slot_strawberry_50x44.png"};
    std::vector<std::string> slotNames = {"Apple", "Banana", "Cherry", "Orange", "Strawberry"};
    m_scammer = Scammer::create(slotImages, slotNames, s_scammerSlotSize);
    
    if(!m_scammer->init())
        return false;

    m_scammer->setAnchorPoint(Vec2::ANCHOR_BOTTOM_LEFT);
    m_scammer->setPosition(s_scammerPos);
    addChild(m_scammer);
    return true;
}

bool StartScene::createCoinSpawner()
{
    CoinSpawner* coinSpawner = CoinSpawner::create("coin23x26.png", s_coinSpawnerSize);
    
    if(!coinSpawner->init())
        return false;
    
    float posX = m_scammer->getPositionX() - (coinSpawner->getContentSize().width * 2.0f);
    float posY = m_scammer->getPositionY() + m_scammer->getContentSize().height/2 - coinSpawner->getContentSize().height/2;
    
    coinSpawner->setAnchorPoint(Vec2::ANCHOR_BOTTOM_LEFT);
    coinSpawner->setPosition({ posX, posY });
    addChild(coinSpawner);
    
    return true;
}

void StartScene::createLabel()
{
    float posX = Director::getInstance()->getWinSize().width/2;
    float posY = m_scammer->getPositionY() + m_scammer->getContentSize().height + (s_labelFontSize * 0.5f);
    
    m_stakeDisplay = Label::createWithSystemFont("", "arial", s_labelFontSize);
    m_stakeDisplay->setAnchorPoint(Vec2::ANCHOR_MIDDLE_BOTTOM);
    m_stakeDisplay->setPosition({ posX, posY });
    addChild(m_stakeDisplay);
}

void StartScene::subscribeToEvents()
{
    EventListenerCustom* customListener1 = EventListenerCustom::create("slots_rotation_finished_event", CC_CALLBACK_1(StartScene::onSlotsRotationFinish, this));
    EventListenerCustom* customListener2 = EventListenerCustom::create("slots_rotation_begin_event", CC_CALLBACK_1(StartScene::onSlotsRotationBegin, this));
    getEventDispatcher()->addEventListenerWithFixedPriority(customListener1, 1);
    getEventDispatcher()->addEventListenerWithFixedPriority(customListener2, 1);
}

void StartScene::onSlotsRotationFinish(cocos2d::EventCustom *event)
{
    const char* prizeName = static_cast<const char*>(event->getUserData());
    m_stakeDisplay->setString("Congrats! Your stake is: " + std::string(prizeName));
}
