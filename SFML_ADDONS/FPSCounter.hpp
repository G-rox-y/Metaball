#pragma once

#include <SFML/Graphics.hpp>
#include <queue>

//this is an FPS counter for sfml projects
//every frame update the counter and pass it the time that has passed (in seconds)
//when you want to pull the FPS count just do .qtext()
//when constructing you determine the size of query
//smaller size = more precision but more violent changes
//bigger size = less precision but more stability
class FPSCounter{
    std::deque<int> queue;
    int query;
    int query_size;
    bool paused;
    sf::Text theText;
public:
    void reset();


    void pause();


    void update(float time);


    sf::Text text();


    void setFont(sf::Font &font);


    FPSCounter(int size);
};