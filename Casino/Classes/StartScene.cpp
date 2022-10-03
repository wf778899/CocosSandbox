#include "StartScene.h"
#include "CoinSpawner.hpp"
#include "ui/CocosGUI.h"

USING_NS_CC;

//Vec2  const StartScene::s_coinInitialPosNorm = {0.2, 0.5};
//float const StartScene::s_coinReturnSpeedNorm = 1.0f;


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
    
//    Size size = Director::getInstance()->getWinSize();
//    m_coinInitialPos = Point(size.width * s_coinInitialPosNorm.x, size.height * s_coinInitialPosNorm.y);
//    m_coinReturnSpeed = Point(0.0f, 0.0f).distance(Point(size)) * s_coinReturnSpeedNorm;
//
//    CreateCoin();
//
//    EventListenerTouchOneByOne* touchListener = EventListenerTouchOneByOne::create();
//    touchListener->onTouchBegan = CC_CALLBACK_2(StartScene::onTouchBegan, this);
//    touchListener->onTouchEnded = CC_CALLBACK_2(StartScene::onTouchEnded, this);
//    touchListener->onTouchMoved = CC_CALLBACK_2(StartScene::onTouchMoved, this);
//    this->getEventDispatcher()->addEventListenerWithSceneGraphPriority(touchListener, m_coin);

    return true;
}


//bool StartScene::onTouchBegan(cocos2d::Touch *touch, cocos2d::Event *unused_event)
//{
//    if(!m_isCoinGrabbed)
//    {
//        if(m_coin->getBoundingBox().containsPoint(touch->getLocation()))
//        {
//            m_coin->stopAllActions();
//            m_coin->runAction(RepeatForever::create(RotateBy::create(0.5f, {0.0f, 360.0f, 0.0f})));
//            m_isCoinGrabbed = true;
//            CCLOG("Coin grabbed!");
//        }
//    }
//    return true;
//}
//
//void StartScene::onTouchMoved(cocos2d::Touch *touch, cocos2d::Event *unused_event)
//{
//    if(m_isCoinGrabbed)
//    {
//        auto winSize = Director::getInstance()->getWinSize();
//        auto touchPos = touch->getLocation();
//        touchPos.x = touchPos.x < 0 ? 0 : (touchPos.x > winSize.width ? winSize.width : touchPos.x);
//        touchPos.y = touchPos.y < 0 ? 0 : (touchPos.y > winSize.height ? winSize.height : touchPos.y);
//
//        //touchPos.x -= m_coin->getPositionX();
//        //touchPos.y -= m_coin->getPositionY();
//
//        m_coin->setPosition(touchPos);
//        CCLOG("TouchMoving detected: X=%f, Y=%f", touchPos.x, touchPos.y);
//    }
//
//}
//
//void StartScene::onTouchEnded(cocos2d::Touch *touch, cocos2d::Event *unused_event)
//{
//    if(m_isCoinGrabbed)
//    {
//        m_coin->stopAllActions();
//        m_coin->setRotation3D({0.0f,0.0f,0.0f});
//
//        // If striked to m_scammer
//        if(false)
//        {
//
//        }
//        // If missed to m_scammer
//        else
//        {
//            float time = m_coinInitialPos.distance(m_coin->getPosition()) / m_coinReturnSpeed;
//            m_coin->runAction(MoveTo::create(time, m_coinInitialPos));
//        }
//
//        m_isCoinGrabbed = false;
//        CCLOG("Coin released!");
//    }
//}
//
//void StartScene::onTouchCancelled(cocos2d::Touch *touch, cocos2d::Event *unused_event)
//{
//    CCLOG("TouchCancelled detected");
//}

void StartScene::CreateScammer()
{
    
}

//void StartScene::CreateCoin()
//{
//    m_coin = Sprite::create("res/coin32x32.png");
//    m_coin->setPosition(m_coinInitialPos);
//    this->addChild(m_coin);
//}

