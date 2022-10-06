#pragma once

#include "cocos2d.h"

class StartScene : public cocos2d::Layer
{
public:
    static cocos2d::Scene* createScene();

    virtual bool init() override;
    
private:
    bool createScammer();
    bool createCoinSpawner();
    void createLabel();
    void subscribeToEvents();
    
    void onSlotsRotationBegin   (cocos2d::EventCustom *unusedEvent) { m_stakeDisplay->setString(""); }
    void onSlotsRotationFinish  (cocos2d::EventCustom *event);
    
    CREATE_FUNC(StartScene);
    
private:
    cocos2d::Label* m_stakeDisplay;
};

