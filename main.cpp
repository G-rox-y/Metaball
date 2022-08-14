#include <iostream>
#include <random>
#include <vector>
#include <string>
#include <cmath>
#include <queue>

#include <SFML/Graphics.hpp>
#include <FPSCounter.hpp>

const int W = 920, H = 640;
const float PI = acos(-1);
float rezScale = 0.15f;
float threshold = 0.0025f;


//rng setup
//kasnije kad budes gotov, generaliziraj klasu da je mozes reciklirat kad pozelis
//aka napravi da se distributionovi automatski deklariraju i to (jer je ovo bas ew)
class CleanRNG{
    std::random_device dev;
    std::mt19937 rng;
    std::uniform_real_distribution<float> X;
    std::uniform_real_distribution<float> Y;
    std::uniform_real_distribution<float> Q;
    std::uniform_real_distribution<float> Ang;
    std::uniform_real_distribution<float> Vel;
public:
    float randomX(){ return X(rng); }
    float randomY(){ return Y(rng); }
    float randomVel(){ return Vel(rng); }
    float randomAng(){ return Ang(rng); }
    float randomQ(){ return Q(rng); }

    CleanRNG(){
        rng.seed(dev());
        X.param(std::uniform_real_distribution<float>::param_type(W/4, W*3/4));
        Y.param(std::uniform_real_distribution<float>::param_type(H/4, H*3/4));
        Q.param(std::uniform_real_distribution<float>::param_type(1.f, 15.f));
        Ang.param(std::uniform_real_distribution<float>::param_type(0.f, 360.f));
        Vel.param(std::uniform_real_distribution<float>::param_type(0.2f, 0.8f));
    }
} rngGen;


class Point{
    //speed is expressed in units per tick
    //direction is expressed by degree angle relative to the X axis
    float velocity, direction;
public:
    float X,Y;

    void update_position(){
        float yChange = sin(direction*2*PI/360) * velocity;
        float xChange = cos(direction*2*PI/360) * velocity;

        if (yChange + Y > 0 && yChange + Y < H) Y += yChange;
        else{
            direction *= -1;
            if (yChange + Y <= 0) Y = abs(yChange+Y);
            else Y = 2*H-yChange-Y;
        }
        if (xChange + X > 0 && xChange + X < W) X += xChange;
        else{
            direction = 180.0f - direction;
            if (xChange + X <= 0) X = abs(xChange+X);
            else X = 2*W-xChange-X;
        }
    }

    Point(){
        X = rngGen.randomX();
        Y = rngGen.randomY();
        velocity = rngGen.randomVel();
        direction = rngGen.randomAng();
    }
};


class MetaBall : public Point{
    float Q; //Q as charge since i will use EM-like fomrulas
public:
    float get_force_at(float d){ return Q/pow(d, 2); }

    MetaBall(){ Q = rngGen.randomQ(); }
};


struct Line{ std::pair<float, float> p1, p2; };


class GlobalFields{
public:
    int metaball_amm;
    std::vector<MetaBall> metaballs;
    std::vector<Line> lines;
    std::vector<std::vector<float> > pointMap;

    float meta_value(float x, float y){
        float rX = x/rezScale, rY = y/rezScale;
        float answ = 0.f;
        for(int k = 0; k < metaball_amm; k++){
            float d = sqrt(pow(metaballs[k].X-rX, 2)+pow(metaballs[k].Y-rY, 2));
            answ += metaballs[k].get_force_at(d);
        }
        return answ;
    }

    void reset(){
        lines.clear();
        return;
    }

    void update_metaballs(){
        for(int i = 0; i < metaball_amm; i++)
            metaballs[i].update_position();
        return;
    }

    void add_line(std::pair<float, float> p1, std::pair<float, float> p2){
        Line l; l.p1 = p1; l.p2 = p2;
        lines.emplace_back(l);
        return;
    }

    GlobalFields(int amm) : metaball_amm(amm){
        metaballs.resize(metaball_amm);
        pointMap.resize(W*rezScale, std::vector<float>(H*rezScale));
    }

}; GlobalFields glF(10);


// this is a bake to make marching squares logic simpler to write
// neighbours array consists of 4 elements, each one being a square edge
// the first two values are the values of the edge (X, Y)
// the next two values are relative values of clockwise neighbour
// the last two values are relative values of counterclockwise neighbour
int ngb[4][6] = {
    {0, 0, 1, 0, 0, 1},
    {1, 0, 1, 1, 0, 0},
    {0, 1, 0, 0, 1, 1},
    {1, 1, 0, 1, 1, 0}
};

std::pair<float, float> lineApprox(float V1, float V2, float X1, float X2, float Y1, float Y2){
    float scale = (V1-threshold) / (V2-V1);
    float dx = (X1-X2) * scale;
    float dy = (Y1-Y2) * scale;
    float newX = (X1+dx) * (1/rezScale);
    float newY = (Y1+dy) * (1/rezScale);
    return std::make_pair(newX, newY);
}

void square_marching(){
    using namespace std;
    int rezX = W*rezScale, rezY = H*rezScale;

    glF.reset();

    for(int i = 0; i < rezX; i++)
        for(int j = 0; j < rezY; j++)
            glF.pointMap[i][j] = glF.meta_value(i, j);
    
    //march trough squares and generate lines
    vector<pair<float, float> > pl;
    for(int x0 = 0; x0 < rezX-1; x0++){
        for(int y0 = 0; y0 < rezY-1; y0++){
            for(int k = 0; k < 4; k++){
                int x1 = x0+ngb[k][0], y1 = y0+ngb[k][1];
                if (glF.pointMap[x1][y1] > threshold){
                    int x2 = x0+ngb[k][2], y2 = y0+ngb[k][3];
                    if(glF.pointMap[x2][y2] <= threshold)
                        pl.emplace_back(lineApprox(
                            glF.pointMap[x1][y1], 
                            glF.pointMap[x2][y2], 
                            x1, x2, y1, y2));
                    int x3 = x0+ngb[k][4], y3 = y0+ngb[k][5];
                    if(glF.pointMap[x3][y3] <= threshold)
                        pl.emplace_back(lineApprox(
                            glF.pointMap[x1][y1], 
                            glF.pointMap[x3][y3], 
                            x1, x3, y1, y3));
                }
            }
            
            if (!pl.size()) continue;

            if (pl.size()==2) glF.add_line(pl[0], pl[1]);
            else if (pl.size()==4){
                float x5 = x0 + 0.5f, y5 = y0 + 0.5f;
                float p5 = glF.meta_value(x5, y5);
                if (p5 > threshold){
                    glF.add_line(pl[1], pl[2]);
                    glF.add_line(pl[0], pl[3]);
                }
                else{
                    glF.add_line(pl[0], pl[1]);
                    glF.add_line(pl[2], pl[3]);
                }
            }
            pl.clear();
        }
    }
    return;
}


int main(){
    sf::ContextSettings settings;
    settings.antialiasingLevel = 8;
    sf::VideoMode desktop = sf::VideoMode::getDesktopMode();
    sf::RenderWindow window(sf::VideoMode(W, H, desktop.bitsPerPixel), "Metaballs", sf::Style::Default, settings);
    // window.setFramerateLimit(60);

    sf::View view(sf::Vector2f(W/2, H/2), sf::Vector2f(W, H));
    window.setView(view);

    sf::Clock fpsClock;
    sf::Font font; font.loadFromFile("OpenSans-Light.ttf");
    FPSCounter fps(10); fps.setFont(font);

    bool isPaused = false;

    while (window.isOpen()){
        sf::Event event;
        while (window.pollEvent(event)){
            if (event.type == sf::Event::Closed) window.close();
            if(event.type == sf::Event::KeyReleased){
                if (event.key.code == sf::Keyboard::P){
                    isPaused = !isPaused;
                    fps.pause();
                }
            }
        }

    	//frame updates (fix this to time intervals)
        if (!isPaused){
            glF.update_metaballs();
            square_marching();
        }

        window.clear();
        
        //window.draw metaballs
        sf::CircleShape pt(3.f);
        pt.setFillColor(sf::Color::Green);
        for(int i = 0; i < glF.metaball_amm; i++){
            pt.setPosition(glF.metaballs[i].X-1.5f, glF.metaballs[i].Y-1.5f);
            window.draw(pt);
        }

        for(int i = 0; i < glF.lines.size(); i++){
            float p1x = glF.lines[i].p1.first, p1y = glF.lines[i].p1.second;
            float p2x = glF.lines[i].p2.first, p2y = glF.lines[i].p2.second;
            
            sf::Vertex line[] = {
                sf::Vertex(sf::Vector2f(p1x, p1y)), 
                sf::Vertex(sf::Vector2f(p2x, p2y))
            };
            window.draw(line, 2, sf::Lines);
        }

        //fps counter
        fps.update(fpsClock.getElapsedTime().asSeconds());
        fpsClock.restart();
        window.draw(fps.text());

        window.display();
    }

    return 0;
}