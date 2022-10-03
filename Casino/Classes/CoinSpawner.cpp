#include "CoinSpawner.hpp"

USING_NS_CC;

const float CoinSpawner::s_coinSpeedNorm = 1.0f;                // Speed the coin'll return with to initial point (normalized - main window's diagonal per second)
const float CoinSpawner::s_coinFallingTime = 0.7f;              // Period (sec) the coin will falling down to the initial position after spawning
const float CoinSpawner::s_coinOffsetY = 5.0f;                  // Coin's Y-offset (px) in relation to the bottom side of the Spawner's window
const Point CoinSpawner::s_coinAnchorPoint = {0.5f, 0.0f};      // Don't touch it!


CoinSpawner* CoinSpawner::create(const std::string& imagePath, Size spawnerSize)
{
    CoinSpawner *coinSpawner = new CoinSpawner(imagePath, spawnerSize);
    coinSpawner->autorelease();
    return coinSpawner;
}


CoinSpawner::CoinSpawner(const std::string& imagePath, Size spawnerSize)
    : m_spawnerSize(spawnerSize)
    , m_coinInitialPos(spawnerSize.width/2, s_coinOffsetY)
    , m_coinFallingFromPos(spawnerSize.width/2, spawnerSize.height)
    , m_coinImage(imagePath)
{
    Size size = Director::getInstance()->getWinSize();
    m_coinSpeed = Point(0.0f, 0.0f).distance(Point(size)) * s_coinSpeedNorm;
    
    // Draw the frame around our CoinSpawner
    DrawNode* frame = DrawNode::create();
    frame->drawRect(getPosition(), m_spawnerSize, Color4F::GREEN);
    addChild(frame);
    
    // Init the clipper
    InitClipper();
    
    // Put the coin into the spawner window
    AddCoin();
    
    // Subscribe to events
    EventListenerTouchOneByOne* touchListener = EventListenerTouchOneByOne::create();
    touchListener->onTouchBegan = CC_CALLBACK_2(CoinSpawner::onTouchBegan, this);
    touchListener->onTouchMoved = CC_CALLBACK_2(CoinSpawner::onTouchMoved, this);
    touchListener->onTouchEnded = CC_CALLBACK_2(CoinSpawner::onTouchEnded, this);
    touchListener->onTouchCancelled = CC_CALLBACK_2(CoinSpawner::onTouchCancelled, this);
    
    getEventDispatcher()->addEventListenerWithSceneGraphPriority(touchListener, this);
    
}


CoinSpawner::~CoinSpawner() noexcept
{
    CCLOG("CoinSpawner destructed");
}


void CoinSpawner::AddCoin()
{
    m_coin = Sprite::create(m_coinImage);
    m_coin->setAnchorPoint(s_coinAnchorPoint);
    m_coin->setPosition(m_coinFallingFromPos);
    
    m_clipper->addChild(m_coin);
    
    Sequence* seq = Sequence::create(EaseBounceOut::create(MoveTo::create(s_coinFallingTime, m_coinInitialPos)),
                                     CallFunc::create([this]
                                        {
                                            m_clipper->removeChild(m_coin);
                                            m_clipper->getParent()->addChild(m_coin);
                                            m_isCoinOnInitialPos = true;
                                        }),
                                     NULL);
    m_coin->runAction(seq);
}


bool CoinSpawner::onTouchBegan(Touch *touch, Event *unused_event)
{
    // Before the "coin is touched" detection - carrying over the touch location from absolute window space to CoinSpawner's space.
    Point touchInCoinSpace( touch->getLocation().x - getPositionX(),
                            touch->getLocation().y - getPositionY());
    
    if(m_isCoinOnInitialPos && m_coin->getBoundingBox().containsPoint(touchInCoinSpace))
    {
        m_offsetForMoving.x = touchInCoinSpace.x - m_coinInitialPos.x;
        m_offsetForMoving.y = touchInCoinSpace.y - m_coinInitialPos.y;
        
        m_coin->stopAllActions();
        m_coin->runAction(RepeatForever::create(RotateBy::create(0.5f, {0.0f, 360.0f, 0.0f})));
        CCLOG("Coin grabbed!");
        return true;
    }
    return false;
}


void CoinSpawner::onTouchMoved(Touch *touch, Event *unused_event)
{
    Size winSize = Director::getInstance()->getWinSize();
    Vec2 touchPos = touch->getLocation();
    
    touchPos.clamp({0.0f, 0.0f}, {winSize.width, winSize.height});
    touchPos.x -= (getPositionX() + m_offsetForMoving.x);
    touchPos.y -= (getPositionY() + m_offsetForMoving.y);
    
    m_coin->setPosition(touchPos);
    m_isCoinOnInitialPos = false;
    
    CCLOG("TouchMoving detected: X=%f, Y=%f", touchPos.x, touchPos.y);}


void CoinSpawner::onTouchEnded(Touch *touch, Event *unused_event)
{
    m_coin->stopAllActions();
    m_coin->setRotation3D({0.0f,0.0f,0.0f});
    
    // If striked to m_scammer
    if(false)
    {
        
    }
    // If missed to m_scammer
    else
    {
        float time = m_coinInitialPos.distance(m_coin->getPosition()) / m_coinSpeed;
        
        Sequence* seq = Sequence::create( MoveTo::create(time, m_coinInitialPos),
                                          CallFunc::create([this] { m_isCoinOnInitialPos = true; }),
                                          nullptr);
        m_coin->runAction(seq);
    }
    
    CCLOG("Coin released!");
}


void CoinSpawner::onTouchCancelled(Touch *touch, Event *unused_event)
{
    CCLOG("Coin released!");
}


void CoinSpawner::InitClipper()
{
    DrawNode* stencil = DrawNode::create();
    stencil->drawSolidRect(getPosition(), m_spawnerSize, Color4F::GREEN);
    
    m_clipper = ClippingNode::create();
    m_clipper->setPosition(getPosition());
    m_clipper->setStencil(stencil);
    m_clipper->setInverted(false);
    
    Node::addChild(m_clipper);
}

