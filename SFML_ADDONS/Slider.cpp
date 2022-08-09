#include <utility>
#include <SFML/Graphics.hpp>
#include <SFML_ADDONS/Slider.hpp>


void Slider::rotationSet(float angle = 0.f)
    : rotation(angle){}


void Slider::orientationSet(bool orientation)
    : sliderType(orientation)
{
    if (xDim < yDim) this->sliderType = false;
    else this->sliderType = true;
}


Slider::Slider(float X, float Y, float XD, float YD)
    : x(X), y(Y), xDim(XD), yDim(YD)
{ 
    rotationReset();
}

Slider::Slider(std::pair<float, float> p1, std::pair<float, float> p2) 
    : x(std::min(p1.first, p2.first)), y(std::min(p1.second, p2.second))
{
    this->xDim = std::max(p1.first, p2.first)-this->x;
    this->yDim = std::max(p1.second, p2.second)-this->y;
    rotationReset();
}