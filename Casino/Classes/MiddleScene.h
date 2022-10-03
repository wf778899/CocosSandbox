#ifndef __MIDDLE_SCENE_H__
#define __MIDDLE_SCENE_H__

#include "cocos2d.h"


class MiddleScene : public cocos2d::Layer
{
public:
    static cocos2d::Scene* createScene();

    virtual bool init() override;
    
    CREATE_FUNC(MiddleScene);
    
};

#endif // __MIDDLE_SCENE_H__
