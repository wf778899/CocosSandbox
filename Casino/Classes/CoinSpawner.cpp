#include "CoinSpawner.hpp"


USING_NS_CC;

const float CoinSpawner::s_coinSpeedNorm        = 1.0f;     // Speed the coin'll return with to initial point (normalized - main window's diagonal per second)
const float CoinSpawner::s_coinFallingTime      = 0.7f;     // Period (sec) the coin will falling down to the initial position after spawning
const float CoinSpawner::s_coinRotationPeriod   = 0.5f;     // Period (sec) the coin caught will do full turnover around itself
const float CoinSpawner::s_coinOffsetY          = 5.0f;     // Coin's Y-offset (px) in relation to the bottom side of the Spawner's window
Point       CoinSpawner::s_coinDropLocation;                // Data buffer for "coin_dropped_event" custom event dispatched by CoinSpawner (drop's location: x and y)


CoinSpawner* CoinSpawner::create(const std::string& imagePath, Size spawnerSize)
{
    CoinSpawner *coinSpawner = new CoinSpawner(imagePath);
    coinSpawner->setContentSize(spawnerSize);
    coinSpawner->autorelease();
    return coinSpawner;
}

bool CoinSpawner::init()
{
    if(!Node::init())
        return false;
    
    if(!buildShaders())
        return false;
    
    drawFrame();            // Draw the green frame around our CoinSpawner
    
    addClipper();
    
    addCoin();              // Put the coin into the spawner window
    
    subscribeToEvents();

    return true;
}

CoinSpawner::~CoinSpawner() noexcept
{
    m_grayingProg->release();
}

CoinSpawner::CoinSpawner(const std::string& imagePath)
    : m_coin(nullptr)
    , m_clipper(nullptr)
    , m_grayingProg(nullptr)
    , m_offsetForMoving{}
    , m_coinImage(imagePath)
    , m_coinSpeed( Vec2::ZERO.distance( Point( Director::getInstance()->getWinSize())) * s_coinSpeedNorm)
    , m_isCoinOnInitialPos(false)
    , m_isDraggingEnabled(true)
{}


bool CoinSpawner::buildShaders()
{
    std::string fragSource = FileUtils::getInstance()->getStringFromFile("res/shaders/graying.frag");
    m_grayingProg = GLProgram::createWithByteArrays(ccPositionTextureColor_noMVP_vert, fragSource.c_str());
    
    if(!m_grayingProg)
        return false;
    
    m_grayingProg->bindAttribLocation(GLProgram::ATTRIBUTE_NAME_POSITION, GLProgram::VERTEX_ATTRIB_POSITION);
    m_grayingProg->bindAttribLocation(GLProgram::ATTRIBUTE_NAME_TEX_COORD, GLProgram::VERTEX_ATTRIB_TEX_COORD);
    m_grayingProg->link();
    
    if(!glGetError())
    {
        m_grayingProg->updateUniforms();
        if(!glGetError())
        {
            m_grayingProg->retain();
            return true;
        }
    }
    CHECK_GL_ERROR_DEBUG();
    return false;
}

void CoinSpawner::drawFrame()
{
    DrawNode* frame = DrawNode::create();
    frame->drawRect(getPosition(), getContentSize(), Color4F::GREEN);
    addChild(frame);
}

void CoinSpawner::addClipper()
{
    DrawNode* stencil = DrawNode::create();
    stencil->drawSolidRect(getPosition(), getContentSize(), Color4F::WHITE);

    m_clipper = ClippingNode::create();
    m_clipper->setPosition(getPosition());
    m_clipper->setStencil(stencil);
    m_clipper->setInverted(false);
    
    addChild(m_clipper);
}

void CoinSpawner::addCoin()
{
    m_coin = Sprite::createWithSpriteFrameName(m_coinImage);
    m_coin->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
    m_coin->setPosition(getContentSize().width/2, getContentSize().height + m_coin->getContentSize().height/2);     // this is the point the coin will falling from when created
    m_clipper->addChild(m_coin);
    
    grayCoin(!m_isDraggingEnabled);

    Sequence* seq = Sequence::create(EaseBounceOut::create(MoveTo::create(s_coinFallingTime, getCoinInitPos())),
                                     CallFunc::create([this]
                                        {
                                            m_clipper->removeChild(m_coin);
                                            this->addChild(m_coin);
                                            m_isCoinOnInitialPos = true;
                                        }), NULL);
    m_coin->runAction(seq);
}

void CoinSpawner::subscribeToEvents()
{
    EventListenerTouchOneByOne* touchListener = EventListenerTouchOneByOne::create();
    touchListener->onTouchBegan = CC_CALLBACK_2(CoinSpawner::onTouchBegan, this);
    touchListener->onTouchMoved = CC_CALLBACK_2(CoinSpawner::onTouchMoved, this);
    touchListener->onTouchEnded = CC_CALLBACK_2(CoinSpawner::onTouchEnded, this);
    touchListener->onTouchCancelled = CC_CALLBACK_2(CoinSpawner::onTouchCancelled, this);
    
    EventListenerCustom* customListener1 = EventListenerCustom::create("slots_rotation_begin_event", CC_CALLBACK_1(CoinSpawner::onSlotsRotationBegin, this));
    EventListenerCustom* customListener3 = EventListenerCustom::create("slots_rotation_finished_event", CC_CALLBACK_1(CoinSpawner::onSlotsRotationFinish, this));
    EventListenerCustom* customListener2 = EventListenerCustom::create("coin_missed_event", CC_CALLBACK_1(CoinSpawner::onCoinMissed, this));
    
    getEventDispatcher()->addEventListenerWithSceneGraphPriority(touchListener, this);
    getEventDispatcher()->addEventListenerWithSceneGraphPriority(customListener1, this);
    getEventDispatcher()->addEventListenerWithSceneGraphPriority(customListener2, this);
    getEventDispatcher()->addEventListenerWithSceneGraphPriority(customListener3, this);
}

void CoinSpawner::grayCoin(bool gray)
{
    if(gray)
    {
        GLProgramState* progState = GLProgramState::getOrCreateWithGLProgram(m_grayingProg);
        m_coin->setGLProgramState(progState);
        progState->setUniformTexture("u_texture", m_coin->getTexture());
        m_coin->getGLProgram()->use();
    }
    else
    {
        GLProgramState* state = GLProgramState::getOrCreateWithGLProgramName(GLProgram::SHADER_NAME_POSITION_TEXTURE_COLOR_NO_MVP);
        m_coin->setGLProgramState(state);
        m_coin->getGLProgram()->use();
    }
}

bool CoinSpawner::onTouchBegan(Touch *touch, Event *unused_event)
{
    Point touchInCoinSpace( touch->getLocation().x-getPositionX(),
                            touch->getLocation().y-getPositionY());     // Before the "coin is touched" detection - carrying over touch location from window space to CoinSpawner's space.
    
    if(m_isDraggingEnabled && m_isCoinOnInitialPos && m_coin->getBoundingBox().containsPoint(touchInCoinSpace))
    {
        m_offsetForMoving.x = touchInCoinSpace.x - getCoinInitPos().x;
        m_offsetForMoving.y = touchInCoinSpace.y - getCoinInitPos().y;
        
        m_coin->stopAllActions();
        m_coin->runAction(RepeatForever::create(RotateBy::create(s_coinRotationPeriod, {0.0f, 360.0f, 0.0f})));     // Rotate near Y

        return true;
    }
    return false;
}

void CoinSpawner::onTouchMoved(Touch *touch, Event *unused_event)
{
    Size winSize = Director::getInstance()->getWinSize();
    Vec2 touchPos = touch->getLocation();
    
    touchPos.clamp(Vec2::ZERO, {winSize.width, winSize.height});
    touchPos.x -= (getPositionX() + m_offsetForMoving.x);
    touchPos.y -= (getPositionY() + m_offsetForMoving.y);
    
    m_coin->setPosition(touchPos);
    m_isCoinOnInitialPos = false;
}

void CoinSpawner::onTouchEnded(Touch *touch, Event *unused_event)
{
    m_coin->stopAllActions();
    m_coin->setRotation3D(Vec3::ZERO);
    
    EventCustom event("coin_dropped_event");
    s_coinDropLocation = touch->getLocation();
    event.setUserData(&s_coinDropLocation);
    getEventDispatcher()->dispatchEvent(&event);
}

void CoinSpawner::onSlotsRotationBegin(EventCustom *unused_event)
{
    Sequence* seq = Sequence::create( CallFunc::create([this] { removeChild(m_coin); }),
                                      CallFunc::create([this] { addCoin(); }),
                                      nullptr);
    runAction(seq);
    m_isDraggingEnabled = false;
}

void CoinSpawner::onSlotsRotationFinish(EventCustom *unusedEvent)
{
    m_isDraggingEnabled = true;
    grayCoin(false);
}

void CoinSpawner::onCoinMissed(EventCustom *unusedEvent)
{
    float time = getCoinInitPos().distance(m_coin->getPosition()) / m_coinSpeed;
    Sequence* seq = Sequence::create( MoveTo::create(time, getCoinInitPos()),
                                      CallFunc::create([this] { m_isCoinOnInitialPos = true; }),
                                      nullptr);
    m_coin->runAction(seq);
}
