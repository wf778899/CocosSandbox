#include "Scammer.hpp"
#include "math/MathUtil.h"

USING_NS_CC;

const int   Scammer::s_visibleSlots = 3;                    // how much slots will you see
const int   Scammer::s_winningSlotNumber = 1;               // slot's number taking into account after rotation (0 - bottom visible slot). Shouldn't be >= s_visibleSlots
const int   Scammer::s_rotationSpeed = 1250;                // slots rotation speed (points per sec)
const float Scammer::s_rotationSlowdownResponse = 1.0f;     // time (sec) the rotation will smoothley slow down. The greater value - the longer rotation.
const float Scammer::s_slowDownFactorFrom = 1.0f;           // rotation speed is a function of smoothing between 'slowDownFactorFrom' and 'slowDownFactorTo' values.
const float Scammer::s_slowDownFactorTo = 0.01f;
char Scammer::s_prizeName[128];                             // data buffer for "slots_rotation_finished_event" custom event emitted by Scammer


Scammer* Scammer::create(const std::vector<std::string> &slotImagePaths, const std::vector<std::string> &slotNames, Size slotSize)
{
    float slotGap = sqrtf(powf(slotSize.width, 2) + powf(slotSize.height, 2))/10;
    Scammer *scammer = new Scammer(slotImagePaths, slotNames, slotSize, slotGap);
    
    Size scammerSize;
    scammerSize.width = slotSize.width + (slotGap * 2);
    scammerSize.height = slotGap + ((slotSize.height + slotGap) * s_visibleSlots);

    scammer->setContentSize(scammerSize);
    scammer->autorelease();
    return scammer;
}


bool Scammer::init()
{
    if(!Node::init())
        return false;
    
    initClipper();
    
    addSlots();             // Add the slots from top to bottom (starting from invisible area above visible frame)
    
    drawFrames();
    
    subscribeToEvents();

    scheduleUpdate();
    
    return true;
}


Scammer::Scammer(const std::vector<std::string> &slotImagePaths, const std::vector<std::string> &slotNames, Size slotSize, float slotGap)
    : m_clipper(nullptr)
    , m_slotImages(slotImagePaths)
    , m_slotNames(slotNames)
    , m_slotSize(slotSize)
    , m_slotsTopY(0.0f)
    , m_slotGap(slotGap)
    , m_doRotate(false)
{
    assert(s_winningSlotNumber <= s_visibleSlots);
    assert(s_visibleSlots <= m_slotImages.size());
    assert(m_slotImages.size() == m_slotNames.size());
}


void Scammer::drawFrames()
{
    DrawNode* frame = DrawNode::create();
    frame->drawRect(getPosition(), getContentSize(), Color4F::BLUE);
    addChild(frame);
    
    float aimFrameOrigX = m_slotGap/2;
    float aimFrameOrigY = m_slotGap/2 + (m_slotSize.height + m_slotGap) * s_winningSlotNumber;
    float aimFrameDestX = aimFrameOrigX + m_slotSize.width + m_slotGap;
    float aimFrameDestY = aimFrameOrigY + m_slotSize.height + m_slotGap;
    
    DrawNode* aimFrame = DrawNode::create();
    aimFrame->drawRect( {aimFrameOrigX, aimFrameOrigY}, {aimFrameDestX, aimFrameDestY}, Color4F::RED);
    addChild(aimFrame);
}


void Scammer::initClipper()
{
    DrawNode* stencil = DrawNode::create();
    stencil->drawSolidRect(getPosition(), getContentSize(), Color4F::GREEN);
    
    m_clipper = ClippingNode::create();
    m_clipper->setPosition(getPosition());
    m_clipper->setStencil(stencil);
    m_clipper->setInverted(false);
    
    addChild(m_clipper);
}


void Scammer::addSlots()
{
    m_slotsTopY = getContentSize().height + (m_slotImages.size() - s_visibleSlots) * (m_slotSize.height + m_slotGap) - m_slotGap - m_slotSize.height/2;
    
    float slotPosY = m_slotGap + m_slotSize.height/2;
    
    for(int i = 0; i < m_slotImages.size(); ++i)
    {
        Sprite* slot = Sprite::createWithSpriteFrameName(m_slotImages[i]);
        slot->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
        slot->setPosition({m_slotGap + m_slotSize.width/2, slotPosY});
        m_clipper->addChild(slot);

        slotPosY += (m_slotSize.height + m_slotGap);
    }
}


void Scammer::subscribeToEvents()
{
    EventListenerCustom* customListener = EventListenerCustom::create("coin_dropped_event", CC_CALLBACK_1(Scammer::onCoinDropped, this));
    getEventDispatcher()->addEventListenerWithFixedPriority(customListener, 1);
}


void Scammer::onCoinDropped(EventCustom* event)
{
    Point* dropPos = static_cast<Point*>(event->getUserData());
    
    if(getBoundingBox().containsPoint(*dropPos))
    {
        getEventDispatcher()->dispatchCustomEvent("slots_rotation_begin_event");
        m_doRotate = true;
    }
    else
    {
        getEventDispatcher()->dispatchCustomEvent("coin_missed_event");
    }
}


void Scammer::update(float dt)
{
    if(m_doRotate)
    {
        static float slowDownFactor = s_slowDownFactorFrom;
        static int winnedSlot = s_winningSlotNumber;
        static float randSpeedBoost = RandomHelper::random_real<float>(0.0f, s_rotationSpeed/10);

        MathUtil::smooth(&slowDownFactor, s_slowDownFactorTo, dt, s_rotationSlowdownResponse);
        
        float step = (s_rotationSpeed + randSpeedBoost) * dt * slowDownFactor;
        
        for(Node* slot : m_clipper->getChildren())
        {
            float newPosY = slot->getPositionY() - step;
            
            if((newPosY + m_slotSize.height/2) < 0)
            {
                newPosY = (m_slotsTopY + newPosY) + m_slotSize.height/2;
                
                if(++winnedSlot == m_slotNames.size())
                {
                    winnedSlot = 0;
                }
                if(slowDownFactor <= (s_slowDownFactorTo*5))
                {
                    m_doRotate = false;
                }
            }
            slot->setPositionY(newPosY);
        }
        // rotation is over, report about the stake
        if(!m_doRotate)
        {
            slowDownFactor = s_slowDownFactorFrom;
            randSpeedBoost = RandomHelper::random_real<float>(0, s_rotationSpeed/10);
            
            EventCustom event("slots_rotation_finished_event");
            strcpy(s_prizeName, m_slotNames[winnedSlot].c_str());
            event.setUserData(s_prizeName);
            getEventDispatcher()->dispatchEvent(&event);
        }
    }
}
