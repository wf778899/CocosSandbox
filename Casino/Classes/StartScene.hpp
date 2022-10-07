#pragma once
#include "cocos2d.h"

class Scammer;
class StartScene : public cocos2d::Layer
{
public:
    static cocos2d::Scene* createScene();

    bool init() override;
    
private:
    bool createScammer          ();
    bool createCoinSpawner      ();
    void createLabel            ();
    void subscribeToEvents      ();
    void onSlotsRotationBegin   (cocos2d::EventCustom *unusedEvent) { m_stakeDisplay->setString(""); }
    void onSlotsRotationFinish  (cocos2d::EventCustom *event);
    
    CREATE_FUNC(StartScene);
    
private:
    cocos2d::Label*             m_stakeDisplay  = nullptr;
    Scammer*                    m_scammer       = nullptr;
    
    static const cocos2d::Size  s_scammerSlotSize;
    static const cocos2d::Size  s_coinSpawnerSize;
    static const cocos2d::Vec2  s_scammerPos;
    static const int            s_labelFontSize;
};

