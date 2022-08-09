#include <queue>
#include <SFML/Graphics.hpp>
#include <SFML_ADDONS/FPSCounter.hpp>


void FPSCounter::reset(){
    this->queue.clear();
    this->query = 0;
}

void FPSCounter::pause(){
    this->paused = !this->paused;
}

void FPSCounter::update(float time){
    if (this->paused) return;

    int fps = 1.0f/time;
    this->queue.push_back(fps);
    this->query += fps;

    if (this->queue.size() > this->query_size){
        this->query -= this->queue.front();
        this->queue.pop_front();
    }
}

sf::Text FPSCounter::text(){
    if (this->paused) this->theText.setString("Paused");
    else if (this->queue.size() < this->query_size) this->theText.setString("...");
    else this->theText.setString("FPS: " + std::to_string(this->query/this->query_size));
    return this->theText;
}

void FPSCounter::setFont(sf::Font &font){
    this->theText.setFont(font);
}

FPSCounter::FPSCounter(int size = 10) : query_size(size){
    this->query = 0;
    this->paused = false;
    this->theText.setFillColor(sf::Color::White);
    this->theText.setCharacterSize(24);
    this->theText.setPosition(sf::Vector2f(5, 5));
}