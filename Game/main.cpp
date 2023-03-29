#include <SFML/Graphics.hpp>
#include <iostream>

using namespace sf;

//Klasa dla tworzenia obiektów i przyznacznia dla nich odrazu textur
class Object                                                                                                                  
{
protected:
    Texture _Texture;
    RectangleShape _RectangleShape;
public:
    //Konstruktor obiektu
    Object(String _LoadFromFile, Vector2f _Size, Vector2f _Position)
    {
        _Texture.loadFromFile(_LoadFromFile);
        _RectangleShape.setSize(_Size);
        _RectangleShape.setTexture(&_Texture);
        _RectangleShape.setPosition(_Position);
    }
    //Pobieranie Obiektu dla odrysowania go na ekranie
    RectangleShape GetObject()
    {
        return _RectangleShape;
    }
};

//Klasa Bohatera
class Player : public Object
{
private:
    Vector2f _Move, _Position;
    float _MoveSpeed = 150.0;
public:
    //Konstruktor Bohatera który pobiera właściwości konstruktora Object
    Player(String _LoadFromFile, Vector2f _Size, Vector2f _Position) :Object(_LoadFromFile, _Size, _Position)
    {}
    //Funkcja porusznia się gracza
    void Move(float time)
    {
        if (Keyboard::isKeyPressed(Keyboard::W)) Object::_RectangleShape.move(0, -_MoveSpeed * time);
        if (Keyboard::isKeyPressed(Keyboard::A)) Object::_RectangleShape.move(-_MoveSpeed * time, 0);
        if (Keyboard::isKeyPressed(Keyboard::S)) Object::_RectangleShape.move(0, _MoveSpeed * time);
        if (Keyboard::isKeyPressed(Keyboard::D)) Object::_RectangleShape.move(_MoveSpeed * time, 0);

        //Granicy pokoju dla bohatera
        _Position = Object::_RectangleShape.getPosition();
        if (_Position.x > 1096) Object::_RectangleShape.setPosition(1096, _Position.y);
        if (_Position.x < 96) Object::_RectangleShape.setPosition(96, _Position.y);
        if (_Position.y > 531) Object::_RectangleShape.setPosition(_Position.x, 531);
        if (_Position.y < 36) Object::_RectangleShape.setPosition(_Position.x, 36);

        if (_Position.x > 1096 && _Position.y > 531) Object::_RectangleShape.setPosition(1096, 531);
        if (_Position.x > 1096 && _Position.y < 36) Object::_RectangleShape.setPosition(1096, 36);
        if (_Position.x < 96 && _Position.y > 531) Object::_RectangleShape.setPosition(96, 531);
        if (_Position.x < 96 && _Position.y < 36) Object::_RectangleShape.setPosition(96, 36);
    }

    //Funkcja strzału
    void Shot(float time, Vector2f _MousePosition)
    {
    
    }
};

int main()
{
    //Właściwości Okna
    RenderWindow window(sf::VideoMode(1280, 720), "Prosta Gra Przygodowa");
    window.setFramerateLimit(60);
    Clock _Clock;

    //Tworzenie Obiektów i dodawanie do nich textur
    Object Floor("Textures/Room/floor.png", Vector2f(1280, 720), Vector2f(0, 0));
    Object LeftWall("Textures/Room/left_wall.png", Vector2f(96, 720), Vector2f(0, 0));
    Object RightWall("Textures/Room/right_wall.png", Vector2f(96, 720), Vector2f(1184, 0));
    Object TopWall("Textures/Room/top_wall.png", Vector2f(1280, 96), Vector2f(0, 0));
    Object BottomWall("Textures/Room/bottom_wall.png", Vector2f(1280, 96), Vector2f(0, 624));
    Player Hero("Textures/Hero/Hero.png", Vector2f(88, 93), Vector2f(640-44, 360-46));



    while (window.isOpen())
    {
        sf::Event event;

        //Zegar który pomaga w poruszniu obiektów
        float time = _Clock.getElapsedTime().asSeconds();
        _Clock.restart();

        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        //Wywołanie funkcji poruszania gracza
        Hero.Move(time);

        if (Mouse::isButtonPressed(Mouse::Left))
        {
            Mouse _Mouse;
            Hero.Shot(time, window.mapPixelToCoords(_Mouse.getPosition(window)));
        }

        //Wyświetlanie wszystkich obiektów w oknie gry
        window.clear();
        window.draw(Floor.GetObject());
        window.draw(LeftWall.GetObject());
        window.draw(RightWall.GetObject());
        window.draw(TopWall.GetObject());
        window.draw(BottomWall.GetObject());
        window.draw(Hero.GetObject());
        window.display();
    }

    return 0;
}