#include "Scammer.hpp"
#include "math/MathUtil.h"


USING_NS_CC;

const int   Scammer::s_visibleSlots             = 3;        // how much slots will you see
const int   Scammer::s_winningSlotNumber        = 1;        // slot's number taking into account after rotation (0 - bottom visible slot). Shouldn't be >= s_visibleSlots
const int   Scammer::s_rotationSpeed            = 1250;     // slots rotation speed (points per sec)
const float Scammer::s_rotationSlowdownResponse = 1.0f;     // time (sec) the rotation will smoothley slow down. The greater value - the longer rotation.
const float Scammer::s_slowDownFactorFrom       = 1.0f;     // rotation speed is a function of smoothing between 'slowDownFactorFrom' and 'slowDownFactorTo' values.
const float Scammer::s_slowDownFactorTo         = 0.01f;
const float Scammer::s_slotGapFactor            = 0.14f;    // distance between the slots is proportional to the slot's diagonal and to this factor
char        Scammer::s_prizeName[128];                      // data buffer for "slots_rotation_finished_event" custom event emitted by Scammer

Scammer* Scammer::create(const std::vector<std::string> &slotImagePaths, const std::vector<std::string> &slotNames, Size slotSize)
{
    Scammer *scammer = new Scammer(slotImagePaths, slotNames, slotSize);
    scammer->autorelease();
    return scammer;
}

bool Scammer::init()
{
    if(!Node::init())
        return false;
    
    addClipper();
    
    addSlots();             // Add the slots from top to bottom (starting from invisible area above visible frame)
    
    drawFrames();
    
    subscribeToEvents();

    scheduleUpdate();
    
    return true;
}

Scammer::Scammer(const std::vector<std::string> &slotImagePaths, const std::vector<std::string> &slotNames, Size slotSize)
    : m_clipper(nullptr)
    , m_slotImages(slotImagePaths)
    , m_slotNames(slotNames)
    , m_slotSize(slotSize)
    , m_slotGap(sqrtf(powf(slotSize.width, 2) + powf(slotSize.height, 2))*s_slotGapFactor)
    , m_doRotate(false)
{
    assert(s_winningSlotNumber <= s_visibleSlots);
    assert(s_visibleSlots <= m_slotImages.size());
    assert(m_slotImages.size() == m_slotNames.size());
    
    setContentSize({ m_slotSize.width + (m_slotGap * 2),
                     m_slotGap + ((m_slotSize.height + m_slotGap) * s_visibleSlots)
                   });
}

void Scammer::addClipper()
{
    DrawNode* stencil = DrawNode::create();
    stencil->drawSolidRect(getPosition(), getContentSize(), Color4F::WHITE);
    
    m_clipper = ClippingNode::create();
    m_clipper->setPosition(getPosition());
    m_clipper->setStencil(stencil);
    m_clipper->setInverted(false);
    
    addChild(m_clipper);
}

void Scammer::addSlots()
{
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
        // 'slotsTopY' - the upper position (Y) the slot will be carried over to after moving down out of stencil visible area.
        static const float slotsTopY = getContentSize().height + (m_slotImages.size() - s_visibleSlots) * (m_slotSize.height + m_slotGap) - m_slotGap - m_slotSize.height/2;
        static float slowDownFactor = s_slowDownFactorFrom;
        static float randSpeedBoost = RandomHelper::random_real<float>(0, s_rotationSpeed/10);
        static int   winnedSlot = s_winningSlotNumber;

        MathUtil::smooth(&slowDownFactor, s_slowDownFactorTo, dt, s_rotationSlowdownResponse);
        
        float step = (s_rotationSpeed + randSpeedBoost) * dt * slowDownFactor;
        
        for(Node* slot : m_clipper->getChildren())
        {
            float newPosY = slot->getPositionY() - step;
            
            // When the slot has moved down out of visible area - move it up to simulate rotation
            if((newPosY + m_slotSize.height/2) < 0)
            {
                newPosY = (slotsTopY + newPosY) + m_slotSize.height/2;
                
                // Just cyclic shift of the winnedSlot's value from 0 to slots number
                if(++winnedSlot == m_slotNames.size())
                {
                    winnedSlot = 0;
                }
                // Rotation will be over some earlier than 'slowDownFactor' will reach its target value by 'smooth()'
                if(slowDownFactor <= (s_slowDownFactorTo*5))
                {
                    m_doRotate = false;
                }
            }
            slot->setPositionY(newPosY);
        }
        // rotation is over, report about our stake!
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
