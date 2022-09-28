#include "HelloWorldScene.h"

USING_NS_CC;

Scene* HelloWorld::createScene()
{
    auto scene = Scene::create();
    auto layer = HelloWorld::create();
    scene->addChild(layer);
    return scene;
}


// on "init" you need to initialize your instance
bool HelloWorld::init()
{
    if(!Layer::init()) {
        return false;
    }
    
    Size size = Director::getInstance()->getWinSize();
    SpriteFrameCache *spritesCache = SpriteFrameCache::getInstance();

    // Cache the animation frames
    spritesCache->addSpriteFramesWithFile("res/atlases/spriggans.plist");
    Sprite* sprite = Sprite::createWithSpriteFrameName("spriggan_red_0.png");
    sprite->setPosition(size/2);
    this->addChild(sprite);
    
    // Fill animation frames from the cache
    Rect bounds({0.0f, 0.0f}, sprite->getContentSize());
    Vector<AnimationFrame*> frames;
    ValueMap info;
    
    for(int i = 1; i < 8; ++i) {
        SpriteFrame *spriteFrame = spritesCache->getSpriteFrameByName(StringUtils::format("spriggan_red_%d.png", i));
        frames.pushBack(AnimationFrame::create(spriteFrame, 0.2f, info));
    }
    for(int i = 6; i > 1; --i) {
        SpriteFrame *spriteFrame = spritesCache->getSpriteFrameByName(StringUtils::format("spriggan_red_%d.png", i));
        frames.pushBack(AnimationFrame::create(spriteFrame, 0.6f, info));
    }
    
    // Set up the animation
    Animation* animation = Animation::create(frames, 0.1f);
    animation->setRestoreOriginalFrame(true);
    animation->setLoops(5);
    
    // Prepare the actions for sequence
    Animate* animate = Animate::create(animation);
    JumpBy* jumpBy = JumpBy::create(1.5f, {100.0f, 50.0f}, 40.0f, 4);
    DelayTime* delay = DelayTime::create(0.8f);
    EaseInOut* easeMoveR = EaseInOut::create(MoveBy::create(1.0f, {(size.width/2) - (bounds.size.width/2), 0.0f}), 3.0f);
    EaseInOut* easeMoveL = EaseInOut::create(MoveBy::create(2.0f, {(bounds.size.width) - size.width, 0.0f}), 3.0f);
    EaseBackInOut* easeMoveUp = EaseBackInOut::create(MoveTo::create(2.0f, {size.width/2, size.height - (bounds.size.height/2)}));
    EaseBounceOut* easeMoveDown = EaseBounceOut::create(MoveTo::create(0.5f, {size.width/2, bounds.size.height/2}));
    
    // Adds the batch of sprites (and remove it as well in the end) to the sender's parent
    CallFuncN* func = CallFuncN::create([size](Node* sender)
    {
        // Add batch to the parent
        auto parent = sender->getParent();
        SpriteBatchNode* batch = SpriteBatchNode::create("res/spriggan.png");
        parent->addChild(batch);

        // Forms the heap of actions to add the plenty of sprites into the batch
        Vector<FiniteTimeAction*> actions;
        
        for(int i = 0; i < 100; ++i)
        {
            // just adds one more sprite to the batch
            actions.pushBack(CallFunc::create([=]()
                                              {
                                                  Sprite* s = Sprite::createWithTexture(batch->getTexture());
                                                  s->setPosition(CCRANDOM_0_1() * size.width, CCRANDOM_0_1() * size.height);
                                                  batch->addChild(s);
                                              }));
            actions.pushBack(DelayTime::create(0.05f));
        }
        
        // Last action will remove the batch and sender from the main layer and adds 3D-Modal
        actions.pushBack(CallFuncN::create([=](Node* _sender)
                                           {
                                                _sender->removeChild(batch);
                                                _sender->removeChild(sender);
            
                                                Sprite3D* sprite3d = Sprite3D::create("res/girl.c3b");
                                                sprite3d->setPosition(size.width, 10);
                                                _sender->addChild(sprite3d);
            
                                                Animation3D* anim3d = Animation3D::create("res/girl.c3b");
                                                Animate3D* animate3d = Animate3D::create(anim3d);
                                                RepeatForever* repeat = RepeatForever::create(animate3d);
                                                sprite3d->runAction(repeat);
            
                                                MoveBy* move1 = MoveBy::create(5.0f, {-size.width, 0});
                                                MoveBy* move2 = MoveBy::create(5.0f, {size.width, 0});
                                                Sequence* seq = Sequence::create(move1, move2, NULL);
                                                sprite3d->runAction(RepeatForever::create(seq));
                                            }));
        parent->runAction(Sequence::create(actions));
        
    });
    
    // Form and run the sequence
    Sequence* todo = Sequence::create(easeMoveUp, delay, easeMoveDown, easeMoveR, easeMoveL, easeMoveR, jumpBy, animate, jumpBy->reverse(), func, NULL);
    sprite->runAction(todo);
    
    return true;
}
