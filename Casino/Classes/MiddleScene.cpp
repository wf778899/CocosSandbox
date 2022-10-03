#include "MiddleScene.h"

USING_NS_CC;


Scene* MiddleScene::createScene()
{
    auto scene = Scene::create();
    auto layer = MiddleScene::create();
    scene->addChild(layer);
    return scene;
}


// on "init" you need to initialize your instance
bool MiddleScene::init()
{
    if ( !Layer::init() ) {
        return false;
    }
    
    Size size = Director::getInstance()->getWinSize();
    
    Label* label = Label::createWithSystemFont("Middle", "Arial", 32);
    label->setPosition(size/2);
    this->addChild(label);
    
    
    return true;
}


