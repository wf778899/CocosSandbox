#include "Coin.hpp"

USING_NS_CC;


Coin* Coin::create(const std::string& imagePath)
{
    Coin *coin = new Coin(imagePath);
    coin->autorelease();
    return coin;
}


Coin::Coin(const std::string& imagePath)
{
    m_coinImage = imagePath;
    
    AddCoin();
}


Coin::~Coin() noexcept
{
    CCLOG("Coin destructed");
}


void Coin::AddCoin()
{
    m_coin = Sprite::create(m_coinImage);
    m_coin->setPosition(this->getPosition().x/2, 100+this->getPosition().y/2);
    
    DrawNode* stencil = DrawNode::create();
    stencil->drawSolidCircle(this->getPosition()/2, 40.0f, 0.0f, 200, Color4F::GREEN);
    stencil->setBlendFunc(BlendFunc::ADDITIVE);
    
    ClippingNode* clipper = ClippingNode::create();
    clipper->setPosition(this->getPosition()/2);
    clipper->setStencil(stencil);
    clipper->setInverted(false);
    
    clipper->addChild(m_coin);
    Node::addChild(clipper);
    
    m_coin->runAction(DelayTime::create(3.0f));
    m_coin->runAction(EaseBounceOut::create(MoveTo::create(1.7f, this->getPosition())));
}
