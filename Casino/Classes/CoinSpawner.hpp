#pragma once

#include "cocos2d.h"


class CoinSpawner : public cocos2d::Node
{
public:
    static CoinSpawner* create(const std::string& imagePath, cocos2d::Size spawnerSize);
    
    virtual bool init() override;
    
private:
    CoinSpawner(const std::string& imagePath, cocos2d::Size spawnerSize);
    
    void drawFrame();
    void initClipper();
    void addCoin();
    void subscribeToEvents();

    bool onTouchBegan           (cocos2d::Touch *touch, cocos2d::Event *unused_event);
    void onTouchMoved           (cocos2d::Touch *touch, cocos2d::Event *unused_event);
    void onTouchEnded           (cocos2d::Touch *touch, cocos2d::Event *unused_event);
    void onTouchCancelled       (cocos2d::Touch *touch, cocos2d::Event *unused_event) {}
    void onCoinHit              (cocos2d::EventCustom *unused_event);
    void onCoinMissed           (cocos2d::EventCustom *unusedEvent);
    void onSlotsRotationFinish  (cocos2d::EventCustom *unusedEvent);

private:
    cocos2d::Sprite*                m_coin;
    cocos2d::ClippingNode*          m_clipper;              // Is used to spawn the money from nowhere ;)
    cocos2d::Point                  m_coinInitialPos;
    cocos2d::Point                  m_coinFallingFromPos;
    cocos2d::Vec2                   m_offsetForMoving;
    std::string                     m_coinImage;
    bool                            m_isCoinOnInitialPos;
    bool                            m_isDraggingEnabled;
    float                           m_coinSpeed;            // Speed the coin'll return with to initial point (resolution depended - points/second)
    
    static const float              s_coinSpeedNorm;        // Speed the coin'll return with to initial point (normalized - main window's diagonal per second)
    static const float              s_coinFallingTime;      // Period (sec) the coin will falling down to the initial position after spawning
    static const float              s_coinRotationPeriod;   // Period (sec) the coin caught will do full turnover around itself
    static const float              s_coinOffsetY;          // Coin's Y-offset (points) in relation to the bottom side of the Spawner's window
    static cocos2d::Point           s_coinDropLocation;     // data buffer for "aboutCoinDropped" custom event emitted by CoinSpawner (drop's location: x and y)
};
