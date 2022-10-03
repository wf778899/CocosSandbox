#pragma once

#include "cocos2d.h"


class CoinSpawner : public cocos2d::Node
{
public:
    static CoinSpawner* create(const std::string& imagePath, cocos2d::Size spawnerSize);
    
    ~CoinSpawner() noexcept override;
    
    void AddCoin();
    
protected:
    bool onTouchBegan       (cocos2d::Touch *touch, cocos2d::Event *unused_event);
    void onTouchMoved       (cocos2d::Touch *touch, cocos2d::Event *unused_event);
    void onTouchEnded       (cocos2d::Touch *touch, cocos2d::Event *unused_event);
    void onTouchCancelled   (cocos2d::Touch *touch, cocos2d::Event *unused_event);

private:
    CoinSpawner(const std::string& imagePath, cocos2d::Size spawnerSize);
    
    void InitClipper();
    
    cocos2d::Sprite*                m_coin = nullptr;
    cocos2d::ClippingNode*          m_clipper = nullptr;     // Is used to spawn the money from nowhere ;)
    cocos2d::Size                   m_spawnerSize;
    cocos2d::Point                  m_coinInitialPos;
    cocos2d::Point                  m_coinFallingFromPos;
    cocos2d::Vec2                   m_offsetForMoving;
    std::string                     m_coinImage;
    bool                            m_isCoinOnInitialPos = false;
    float                           m_coinSpeed;            // Speed the coin'll return with to initial point (resolution depended - points/second)
    static const float              s_coinSpeedNorm;        // Speed the coin'll return with to initial point (normalized - main window's diagonal per second)
    static const float              s_coinFallingTime;      // Period (sec) the coin will falling down to the initial position after spawning
    static const float              s_coinOffsetY;          // Coin's Y-offset (points) in relation to the bottom side of the Spawner's window
    static const cocos2d::Point     s_coinAnchorPoint;
};
