#pragma once

#include <SFML/Graphics.hpp>
#include <utility>

class Slider{
private:
    float rotation;
    bool sliderType; //true if the slider is vertical, false if horizontal
    float x, y;
    float xDim, yDim;
public:
    
    void rotationSet(float angle);


    void orientationSet(bool orientation);


    Slider(float X, float Y, float XD, float YD);


    Slider(std::pair<float, float> p1, std::pair<float, float> p2);
};