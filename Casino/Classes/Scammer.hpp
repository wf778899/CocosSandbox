#pragma once

#include "cocos2d.h"


class Scammer : public cocos2d::Node
{
public:
    static Scammer* create(const std::vector<std::string> &slotImagePaths, const std::vector<std::string> &slotNames);
    
    virtual bool init() override;
    
private:
    Scammer(const std::vector<std::string> &slotImagePaths, const std::vector<std::string> &slotNames);
    
    void drawFrame();
    void initClipper();
    void addSlots();
    void subscribeToEvents();
    
    void onCoinDropped(cocos2d::EventCustom* event);
    
    virtual void update(float dt) override;
    
private:
    cocos2d::ClippingNode*          m_clipper;
    std::vector<std::string>        m_slotImages;
    std::vector<std::string>        m_slotNames;
    float                           m_slotsTopY;                    // top-left corner of the slots area (its invisible part, above the visible frame)
    bool                            m_doRotate;
    
    static const float              s_distanceBetweenSlots;         // vertical distance between the slots (points)
    static const cocos2d::Size      s_slotSize;                     // slots dimentions
    static const int                s_visibleSlots;                 // how much slots will you see
    static const int                s_winningSlotNumber;            // slot's number taking into account after rotation (0 - bottom visible slot). Shouldn't be >= s_visibleSlots
    static const int                s_rotationSpeed;                // slots rotation speed (points per sec)
    static const float              s_rotationSlowdownResponse;     // time (sec) the rotation will smoothley slow down. The greater value - the longer rotation.
    static const float              s_slowDownFactorFrom;           // rotation speed is a function of smoothing between 'slowDownFactorFrom' and 'slowDownFactorTo' values.
    static const float              s_slowDownFactorTo;
    
    static char                     s_prizeName[128];               // data buffer for "slots_rotation_finished_event" custom event emitted by Scammer
};
