#ifndef __START_SCENE_H__
#define __START_SCENE_H__

#include "cocos2d.h"


class StartScene : public cocos2d::Layer
{
public:
    static cocos2d::Scene* createScene();

    virtual bool init() override;
    
    CREATE_FUNC(StartScene);
    
protected:
    //bool onTouchBegan(cocos2d::Touch *touch, cocos2d::Event *unused_event) override;
    //void onTouchMoved(cocos2d::Touch *touch, cocos2d::Event *unused_event) override;
    //void onTouchEnded(cocos2d::Touch *touch, cocos2d::Event *unused_event) override;
    //void onTouchCancelled(cocos2d::Touch *touch, cocos2d::Event *unused_event) override;
    
private:
    void CreateScammer();
    //void CreateCoin();
    
    //cocos2d::Sprite*            m_scammer = nullptr;
    //cocos2d::Sprite*            m_coin = nullptr;
    //bool                        m_isCoinGrabbed = false;
    //cocos2d::Vec2               m_coinInitialPos;
    //float                       m_coinReturnSpeed;          // points per second
    //static const cocos2d::Vec2  s_coinInitialPosNorm;       // Coin's position in normalized space 0...1f
    //static const float          s_coinReturnSpeedNorm;      // Speed the coin'll return with to initial point (normalized - window's diagonal per second)
};

#endif // __START_SCENE_H__
