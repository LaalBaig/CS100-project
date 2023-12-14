/*
*********************************
CS100 Project
*********************************


Greetings!!
the following project is a barebones implementation of raycasting in c++ and sfml
please follow along the comments i've added for convenience

1. there are a few features that I have not implemented due to technical difficulties,

2.//documentation for the SFML library: https://www.sfml-dev.org/documentation/2.6.1/
------------------------------------------------------------------------------------------------

3. //article used to understand raycasting https://lodev.org/cgtutor/raycasting.html
------------------------------------------------------------------------------------------------
this article does not really provide any code, so this was used to understand the algorithm, (DDA, the most fundamental graphics algorithm)

*/
#include <SFML/Graphics.hpp>
#include <iostream>
#include <math.h>
using namespace sf;
using namespace std;

RenderWindow window(VideoMode(500, 500), "miniMap", Style::Close | Style::Resize);
RenderWindow window2(VideoMode(800, 450), "the Game!!", Style::Close | Style::Resize);
RectangleShape character(Vector2f(10.0f, 10.0f));
RectangleShape pointer(Vector2f(20.0f, 2.0f));
RectangleShape FPScharacter(Vector2f(300, 250));
Texture burger;

// some comments for future reference incase i forget myself what I am doing
void keyPressRegister(RectangleShape &a, float &v);
void initialise(); // intialises base position of character
void miniMap();    // renders map
void DisplayController();
float degreeToRadian(float degreeAngle);
float RayCalcH(float playerPostionX, float playerPositionY, float angle); //
float RayCalcV(float playerPostionX, float playerPositionY, float angle);
float RayCalc(float playerPostionX, float playerPositionY, float angle);
float mod(float n);
void CastRay();                                            // good to go (might change rectangles to vertex lines)
int currentCelldist(float coordinate);                     // subject to change
int currentCellCalc(float xCoordinate, float yCoordinate); // subject to chcange
void CastRay3d(float distance);                            // good to go
float rayCollision(float x, float y, float angle);         // will change
float velocity = 0.04f;
// var newTile // used to determine the tile type

int mymap[10][10] =
    {
        {1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
        {1, 0, 0, 0, 0, 0, 0, 0, 0, 1},
        {1, 0, 0, 0, 1, 1, 1, 0, 0, 1},
        {1, 0, 0, 0, 0, 0, 0, 0, 0, 1},
        {1, 0, 0, 0, 0, 0, 0, 0, 0, 1},
        {1, 0, 0, 0, 0, 0, 0, 0, 0, 1},
        {1, 0, 1, 1, 0, 0, 0, 0, 0, 1},
        {1, 0, 0, 0, 0, 0, 1, 0, 0, 1},
        {1, 0, 0, 0, 0, 0, 1, 0, 0, 1},
        {1, 1, 1, 1, 1, 1, 1, 1, 1, 1}};
// may use numbers other than 1 to represent other tiles

int main()
{
    initialise();
    while (window.isOpen())
    {
        Event event;
        while (window.pollEvent(event))
        {
            if (event.type == Event::Closed)
                window.close();
        }

        DisplayController();
        keyPressRegister(character, velocity);
        // collisionDetection();
        // rendering uses two buffers. you render stuff on the back buffer and when we call display(), the buffers are swapped, leading to
        // the front display now being rendered upon.
    }
    return 0;
}

void CastRay3d(float distance, int offset)
{
    int d = distance;
    RectangleShape wall(Vector2f(13.33, 4000 / distance));
    wall.setOrigin(Vector2f(13.33 / 2, 2000 / distance));
    wall.setFillColor(Color(255, 0, 255, 4000 / d + 150)); // trying to implement depth by tweaking the alpha channel
    wall.setPosition(Vector2f(400 + offset * 13.33, 275));
    wall.setScale(Vector2f(1, 6));
    window2.draw(wall);
}

float dist(float x1, float y1, float x2, float y2)
{
    return sqrt((x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1));
}
int currentCellCalc(float coordinate)
{
    int position = floor(coordinate);
    return (position - position % 50) / 50;
}

int currentCelldist(float coordinate)
{
    int position = floor(coordinate);
    return (position % 50);
}

float normalisedCellDist(float coordinate)
{
    return currentCelldist(coordinate) / 50;
}

float degreeToRadian(float degreeAngle)
{
    return (3.1415926535 * degreeAngle) / 180;
}

void keyPressRegister(RectangleShape &a, float &v)
{
    float angle;
    if (Keyboard::isKeyPressed(Keyboard::Key::A))
    {
        pointer.rotate(-0.04f);
        a.setRotation(pointer.getRotation());
    }
    if (Keyboard::isKeyPressed(Keyboard::Key::D))
    {
        pointer.rotate(0.04f);
        a.setRotation(pointer.getRotation());
    }
    if (mymap[currentCellCalc(character.getPosition().y)][currentCellCalc(character.getPosition().x)] == 0)
    {
        if (Keyboard::isKeyPressed(Keyboard::Key::W))
        {
            angle = degreeToRadian(pointer.getRotation());
            character.move(velocity * (cosf(angle)), velocity * (sinf(angle)));
        }
    }

    if (Keyboard::isKeyPressed(Keyboard::Key::S))
    {

        angle = degreeToRadian(pointer.getRotation());
        character.move(-velocity * (cosf(angle)), -velocity * (sinf(angle)));
    }
}

void CastRay(float x, float y, float angle)
{
    float length;
    for (int i = -30; i <= 30; i++)
    {
        length = rayCollision(x, y, angle + i);
        RectangleShape ray(Vector2f(length, 2.0f));
        ray.setFillColor(Color::Red);
        ray.setPosition(character.getPosition().x, character.getPosition().y);
        ray.setRotation(angle + i);
        window.draw(ray);
        CastRay3d(length, i);
    }
}
// this doesnt work perfectly for non integer coordinates; working on it
// this function was made using the help of gpt
float rayCollision(float x, float y, float angle)
{
    float radians = degreeToRadian(angle);
    float stepSize = 1.0;
    float dx = stepSize * sinf(radians);
    float dy = stepSize * cosf(radians);

    float rayCordX = x;
    float rayCordY = y;

    for (float step = 0; step < 500; step += stepSize)
    {
        int cellX = floor(rayCordX / 50);
        int cellY = floor(rayCordY / 50);

        if (cellX < 0 || cellX >= 10 || cellY < 0 || cellY >= 10)
        {
            return 1000000; // No collision (out of bounds)
        }

        if (mymap[cellX][cellY] == 1)
        {
            return dist(x, y, rayCordX, rayCordY); // distance return hoga
        }
        // DDA iterative step
        rayCordX += dx;
        rayCordY += dy;
    }

    return 1000000; // No collision
}
// for the functions below id like to discuss their shortcomings
float RayCalcH(float playerPostionX, float playerPositionY, float angle)
{

    Vector2f rayCoordinate(playerPostionX, playerPositionY);
    int floorX, floorY;
    float stepX, stepY;
    float gradient = mod(tanf(degreeToRadian(angle)));
    float initialStepX;
    // quadrants are flipped on the x-axis such that quadrant 1 is 360 to 270 degrees
    // horizontal lines intersections
    // quadrant 1 and 2 (looking up---->

    // looking left or right
    if (angle == 180)
    {
        stepY = 0;
        return 10000;
    }
    if (angle == 360)
    {
        stepY = 0;
        return 10000;
    }

    // looking up or down
    if (angle == 270)
    {
        rayCoordinate.y -= currentCelldist(playerPositionY);
        floorX = floor(rayCoordinate.x);
        floorY = floor(rayCoordinate.y);
        while (mymap[floorY / 50 - 1][floorX / 50] != 1 && floorY / 50 - 1 > 0)
        {
            floorY -= 50;
            rayCoordinate.y -= 50;
        }
        return dist(playerPostionX, playerPositionY, rayCoordinate.x, rayCoordinate.y);
    }
    else if (angle == 90)
    {
        rayCoordinate.y -= 50 - currentCelldist(playerPositionY);
        floorX = floor(rayCoordinate.x);
        floorY = floor(rayCoordinate.y);
        while (mymap[(floorY + 1)][floorX / 50] != 1 && floorY / 50 + 1 < 10)
        {
            floorY += 50;
            rayCoordinate.y += 50;
        }
        return dist(playerPostionX, playerPositionY, rayCoordinate.x, rayCoordinate.y);
    }

    // lookking up
    //--------------------------------------------------------------------------------------------

    // upleft
    if (angle > 180 && angle < 270)
    {
        stepX = currentCelldist(playerPositionY) / gradient;
        rayCoordinate.x -= stepX;
        if (rayCoordinate.x < 0)
            return 10000;
        rayCoordinate.y -= currentCelldist(playerPositionY);
        floorX = floor(rayCoordinate.x);
        floorY = floor(rayCoordinate.y);

        while (mymap[floorY / 50 - 1][floorX / 50] != 1)
        {
            stepX = (50) / tanf(degreeToRadian(angle));
            floorY -= 50;
            rayCoordinate.x -= stepX;
            rayCoordinate.y -= 50;
        }
        return dist(playerPostionX, playerPositionY, rayCoordinate.x, rayCoordinate.y);
    }

    // upright
    if (angle > 270 && angle < 360)
    {
        stepX = currentCelldist(playerPositionY) / gradient;
        rayCoordinate.x += stepX;
        if (rayCoordinate.x > 500)
            return 10000;
        rayCoordinate.y -= currentCelldist(playerPositionY);
        floorX = floor(rayCoordinate.x);
        floorY = floor(rayCoordinate.y);
        stepX = (50) / mod(tanf(degreeToRadian(angle)));
        while (mymap[floorY / 50 - 1][floorX / 50] != 1)
        {
            floorY -= 50;
            rayCoordinate.x += stepX;
            rayCoordinate.y -= 50;
        }

        return dist(playerPostionX, playerPositionY, rayCoordinate.x, rayCoordinate.y);
    }
    //--------------------------------------------------------------------------------------------

    // lookking down
    //--------------------------------------------------------------------------------------------
    // downleft
    if (angle > 90 && angle < 180)
    {
        stepX = (50 - currentCelldist(playerPositionY)) / gradient;
        rayCoordinate.x -= stepX;
        if (rayCoordinate.x < 0)
            return 10000;
        rayCoordinate.y += currentCelldist(playerPositionY);
        floorX = floor(rayCoordinate.x);
        floorY = floor(rayCoordinate.y);
        while (mymap[floorY / 50][floorX / 50] != 1)
        {
            stepX = (50) / tanf(degreeToRadian(angle));
            floorY += 50;
            rayCoordinate.x -= stepX;
            rayCoordinate.y += 50;
        }
        return dist(playerPostionX, playerPositionY, rayCoordinate.x, rayCoordinate.y);
    }

    if (angle > 0 && angle < 90)
    {
        stepX = (50 - currentCelldist(playerPositionY)) / gradient;
        rayCoordinate.x += stepX;
        if (rayCoordinate.x > 500)
            return 10000;
        rayCoordinate.y += currentCelldist(playerPositionY);
        floorX = floor(rayCoordinate.x);
        floorY = floor(rayCoordinate.y);
        stepX = (50) / mod(tanf(degreeToRadian(angle)));
        while (mymap[floorY / 50][floorX / 50] != 1)
        {
            floorY += 50;
            rayCoordinate.x += stepX;
            rayCoordinate.y += 50;
        }

        return dist(playerPostionX, playerPositionY, rayCoordinate.x, rayCoordinate.y);
    }
    // downright
    //--------------------------------------------------------------------------------------------

    return dist(playerPostionX, playerPositionY, rayCoordinate.x, rayCoordinate.y);
}
float RayCalcV(float playerPostionX, float playerPositionY, float angle)
{

    Vector2f rayCoordinate(playerPostionX, playerPositionY);
    int floorX, floorY;
    float stepX, stepY;
    float gradient = mod(tanf(degreeToRadian(angle)));
    float initialStepX;
    // quadrants are flipped on the x-axis such that quadrant 1 is 360 to 270 degrees
    // horizontal lines intersections
    // quadrant 1 and 2 (looking up---->

    // looking left or right
    if (angle == 180)
    {
        rayCoordinate.x -= currentCelldist(playerPositionY);
        floorX = floor(rayCoordinate.x);
        floorY = floor(rayCoordinate.y);
        while (mymap[floorY / 50][floorX / 50 - 1] != 1)
        {
            floorX -= 50;
            rayCoordinate.x -= 50;
        }
        return dist(playerPostionX, playerPositionY, rayCoordinate.x, rayCoordinate.y);
    }
    if (angle == 360)
    {
        rayCoordinate.x += currentCelldist(playerPositionY);
        floorX = floor(rayCoordinate.x);
        floorY = floor(rayCoordinate.y);
        while (mymap[floorY / 50][floorX / 50] != 1 && floorY / 50 - 1 > 0)
        {
            floorX += 50;
            rayCoordinate.x += 50;
        }
        return dist(playerPostionX, playerPositionY, rayCoordinate.x, rayCoordinate.y);
    }

    // looking up or down
    if (angle == 270)
    {
        return 10000;
    }
    else if (angle == 90)
    {
        return 10000;
    }

    // lookking up
    //--------------------------------------------------------------------------------------------

    // upleft
    if (angle > 180 && angle < 270)
    {
        stepX = currentCelldist(playerPositionY) / gradient;
        rayCoordinate.x -= stepX;
        if (rayCoordinate.x < 0)
            return 10000;
        rayCoordinate.y -= currentCelldist(playerPositionY);
        floorX = floor(rayCoordinate.x);
        floorY = floor(rayCoordinate.y);

        while (mymap[floorY / 50][floorX / 50 - 1] != 1)
        {
            stepX = (50) / tanf(degreeToRadian(angle));
            floorY -= 50;
            rayCoordinate.x -= stepX;
            rayCoordinate.y -= 50;
        }
        return dist(playerPostionX, playerPositionY, rayCoordinate.x, rayCoordinate.y);
    }

    // upright
    if (angle > 270 && angle < 360)
    {
        stepX = currentCelldist(playerPositionY) / gradient;
        rayCoordinate.x += stepX;
        if (rayCoordinate.x > 500)
            return 10000;
        rayCoordinate.y -= currentCelldist(playerPositionY);
        floorX = floor(rayCoordinate.x);
        floorY = floor(rayCoordinate.y);
        stepX = (50) / mod(tanf(degreeToRadian(angle)));
        while (mymap[floorY / 50][floorX / 50] != 1)
        {
            floorY -= 50;
            rayCoordinate.x += stepX;
            rayCoordinate.y -= 50;
        }

        return dist(playerPostionX, playerPositionY, rayCoordinate.x, rayCoordinate.y);
    }
    //--------------------------------------------------------------------------------------------

    // lookking down
    //--------------------------------------------------------------------------------------------
    // downleft
    if (angle > 90 && angle < 180)
    {
        stepX = currentCelldist(playerPositionY) / gradient;
        rayCoordinate.x -= stepX;
        if (rayCoordinate.x < 0)
            return 10000;
        rayCoordinate.y += currentCelldist(playerPositionY);
        floorX = floor(rayCoordinate.x);
        floorY = floor(rayCoordinate.y);
        while (mymap[floorY / 50][floorX / 50] != 1)
        {
            stepX = (50) / tanf(degreeToRadian(angle));
            floorY += 50;
            rayCoordinate.x -= stepX;
            rayCoordinate.y += 50;
        }
        return dist(playerPostionX, playerPositionY, rayCoordinate.x, rayCoordinate.y);
    }

    if (angle > 0 && angle < 90)
    {
        stepX = currentCelldist(playerPositionY) / gradient;
        rayCoordinate.x += stepX;
        if (rayCoordinate.x > 500)
            return 10000;
        rayCoordinate.y += currentCelldist(playerPositionY);
        floorX = floor(rayCoordinate.x);
        floorY = floor(rayCoordinate.y);
        stepX = (50) / mod(tanf(degreeToRadian(angle)));
        while (mymap[floorY / 50][floorX / 50] != 1)
        {
            floorY += 50;
            rayCoordinate.x += stepX;
            rayCoordinate.y += 50;
        }

        return dist(playerPostionX, playerPositionY, rayCoordinate.x, rayCoordinate.y);
    }
    // downright
    //--------------------------------------------------------------------------------------------

    return dist(playerPostionX, playerPositionY, rayCoordinate.x, rayCoordinate.y);
}

void initialise() // initialises
{
    burger.loadFromFile("borger.png");
    FPScharacter.setTexture(&burger);
    FPScharacter.setPosition(250, 250);
    window.setPosition(Vector2i(0, 0));
    window2.setPosition(Vector2i(500, 0));
    pointer.setOrigin(0.0f, 1.0f);
    pointer.setRotation(180);
    character.setPosition(220.0f, 220.0f);
    character.setFillColor(Color::Red);
    pointer.setFillColor(Color::Red);
    character.setOrigin(5.0f, 5.0f);
}
float RayCalc(float playerPostionX, float playerPositionY, float angle)
{
    if (RayCalcH(playerPostionX, playerPositionY, angle) < RayCalcV(playerPostionX, playerPositionY, angle))
    {
        return RayCalcH(playerPostionX, playerPositionY, angle);
    }
    else
    {
        return RayCalcV(playerPostionX, playerPositionY, angle);
    }
}

void miniMap()
{

    for (int y = 0; y < 10; y++)
    {
        for (int x = 0; x < 10; x++)
        {
            float xCoordinate = x * 50;
            float yCoordinate = y * 50;
            if (mymap[y][x] == 1)
            {
                RectangleShape tile(Vector2f(49.0f, 49.0f));
                tile.setPosition(xCoordinate, yCoordinate);
                window.draw(tile);
            }
            else
            {
                RectangleShape tile(Vector2f(49.0f, 49.0f));
                tile.setPosition(xCoordinate, yCoordinate);
                tile.setFillColor(Color::Cyan);
                window.draw(tile);
            }
        }
    }
}

void DisplayController()
{
    window.clear();
    window2.clear();
    miniMap();
    float x = character.getPosition().x;
    float y = character.getPosition().y;
    float angle = pointer.getRotation();
    window.draw(character);
    window.draw(pointer);
    pointer.setPosition(x, y);
    CastRay(x, y, angle);
    window2.draw(FPScharacter);
    window.display();
    window2.display();
}

float mod(float n)
{
    if (n < 0)
        return -1 * n;
    else
        return n;
}
