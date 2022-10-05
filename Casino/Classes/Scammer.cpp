#include "Scammer.hpp"
#include "math/MathUtil.h"

USING_NS_CC;

const float Scammer::s_distanceBetweenSlots = 10.0f;        // vertical distance between the slots (points)
const Size  Scammer::s_slotSize = {40.0f, 30.0f};           // slots dimentions
const int   Scammer::s_visibleSlots = 3;                    // how much slots will you see
const int   Scammer::s_winningSlotNumber = 1;               // slot's number taking into account after rotation (0 - bottom visible slot). Shouldn't be >= s_visibleSlots
const int   Scammer::s_rotationSpeed = 1250;                // slots rotation speed (points per sec)
const float Scammer::s_rotationSlowdownResponse = 1.0f;     // time (sec) the rotation will smoothley slow down. The greater value - the longer rotation.
const float Scammer::s_slowDownFactorFrom = 1.0f;           // rotation speed is a function of smoothing between 'slowDownFactorFrom' and 'slowDownFactorTo' values.
const float Scammer::s_slowDownFactorTo = 0.01f;


Scammer* Scammer::create(const std::vector<std::string> &slotImagePaths, const std::vector<std::string> &slotNames)
{
    Scammer *scammer = new Scammer(slotImagePaths, slotNames);
    
    Size scammerSize;
    scammerSize.width = s_slotSize.width + (s_distanceBetweenSlots * 2);
    scammerSize.height = s_distanceBetweenSlots + ((s_slotSize.height + s_distanceBetweenSlots) * s_visibleSlots);

    scammer->setContentSize(scammerSize);
    scammer->autorelease();
    return scammer;
}


bool Scammer::init()
{
    if(!Node::init())
        return false;
    
    drawFrame();            // draw the blue frame around our Scammer

    initClipper();
    
    addSlots();             // Add the slots from top to bottom (starting from invisible area above visible frame)
    
    subscribeToEvents();

    scheduleUpdate();
    
    return true;
}


Scammer::Scammer(const std::vector<std::string> &slotImagePaths, const std::vector<std::string> &slotNames)
    : m_clipper(nullptr)
    , m_slotImages(slotImagePaths)
    , m_slotNames(slotNames)
    , m_slotsTopY(0.0f)
    , m_doRotate(false)
{
    assert(s_winningSlotNumber <= s_visibleSlots);
    assert(s_visibleSlots <= m_slotImages.size());
    assert(m_slotImages.size() == m_slotNames.size());
}


void Scammer::drawFrame()
{
    DrawNode* frame = DrawNode::create();
    frame->drawRect(getPosition(), getContentSize(), Color4F::BLUE);
    addChild(frame);
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
    m_slotsTopY = getContentSize().height + (m_slotImages.size() - s_visibleSlots) * (s_slotSize.height + s_distanceBetweenSlots) - s_distanceBetweenSlots;
    
    float slotPosY = s_slotSize.height + s_distanceBetweenSlots;
    
    for(int i = 0; i < m_slotImages.size(); ++i)
    {
        Sprite* slot = Sprite::create(m_slotImages[i]);
        slot->setAnchorPoint(Vec2::ANCHOR_TOP_LEFT);
        slot->setContentSize(s_slotSize);
        slot->setPosition({s_distanceBetweenSlots, slotPosY});
        m_clipper->addChild(slot);

        slotPosY += (s_slotSize.height + s_distanceBetweenSlots);
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
        getEventDispatcher()->dispatchCustomEvent("coin_hit_event");
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
            
            if(newPosY < 0)
            {
                newPosY = m_slotsTopY + newPosY;
                
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
            randSpeedBoost = RandomHelper::random_real<float>(0.0f, s_rotationSpeed/10);
            
            getEventDispatcher()->dispatchCustomEvent("slots_rotation_finished_event");
            CCLOG("Your stake is: %s!", m_slotNames[winnedSlot].c_str());
        }
    }
}
