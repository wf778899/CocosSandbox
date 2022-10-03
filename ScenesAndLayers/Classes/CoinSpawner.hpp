#pragma once

#include "cocos2d.h"


class Coin : public cocos2d::Node
{
public:
    static Coin* create(const std::string& imagePath);
    
    ~Coin() noexcept override;
    
    void AddCoin();
    
protected:

    
private:
    Coin(const std::string& imagePath);
    
    cocos2d::Sprite* m_coin = nullptr;
    cocos2d::Size 
    std::string m_coinImage;
};
