#define _CRT_SECURE_NO_WARNINGS
#include <SFML/Graphics.hpp>
#include <windows.h>
#include <vector>

using namespace sf;
using namespace std;

#define ScreenWidth 1280
#define ScreenHeight 720
#define WallSize 96

Clock _Clock;
float _time = 0.0;

//Klasa Bazowa dla obiektów którzy muszą być wyświetlone
class GameSprite
{
protected:
    Texture _Texture;
    Sprite _Sprite;
    Vector2f _Position;
    float _Width, _Height;
public:
    //Konstruktor obiektu
    GameSprite(String _LoadFromFile, Vector2f _FPosition)
    {
        if (!_Texture.loadFromFile(_LoadFromFile)) printf("%s Don`t Open\n", _LoadFromFile.toAnsiString().c_str());
        _Position = _FPosition;
        _Sprite.setTexture(_Texture);
        _Width = _Sprite.getLocalBounds().width;
        _Height = _Sprite.getLocalBounds().height;
        _Sprite.setOrigin(_Sprite.getLocalBounds().width / 2, _Sprite.getLocalBounds().height / 2);
        _Sprite.setPosition(_FPosition);
    }

    //Pobieranie Obiektu dla odrysowania go na ekranie
    Sprite GetGameSprite()
    {
        return _Sprite;
    }
};

//Klasa Strału
class Bullet : public GameSprite
{ 
private:
    float _Damage = 10;
    const float _BulletSpeed = 1000.0;
    Vector2f _Direction;
public:
    //Pusty Konstruktor tylko dla ustawienia tekstur
    Bullet(String _LoadFromFile, Vector2f _Position) :GameSprite(_LoadFromFile, _Position)
    {}

    void SetDirection(Vector2f _FPosition, Vector2f _MousePosition)
    {
        _Sprite.setPosition(_FPosition);
        _Direction = Vector2f(_MousePosition - _FPosition);
        float _Distance = static_cast<float>(sqrt(pow(_MousePosition.x - _Position.x, 2) + pow(_MousePosition.y - _Position.y, 2)));
        _Direction /= _Distance;
    }

    //Metod który przemieszca pocisk
    void Update(RenderWindow &_window)
    {
        _Sprite.move(_Direction * _BulletSpeed * _time);
        _window.draw(_Sprite);
    }
};

//Klasa Gracza
class Player : public GameSprite
{
private:
    Vector2f _Move;
    const float _MoveSpeed = 150.0;
public:
    //Konstruktor Bohatera który pobiera właściwości konstruktora GameSprite
    Player(String _LoadFromFile, Vector2f _Position) :GameSprite(_LoadFromFile, _Position)
    {}

    //Funkcja porusznia się gracza
    void Move()
    {
        _Position = _Sprite.getPosition();
        if (Keyboard::isKeyPressed(Keyboard::W) && _Position.y > WallSize) GameSprite::_Sprite.move(0, -_MoveSpeed * _time);
        if (Keyboard::isKeyPressed(Keyboard::A) && _Position.x > WallSize + _Width / 2)
        {
            GameSprite::_Sprite.move(-_MoveSpeed * _time, 0);
            GameSprite::_Sprite.setScale(-1.f, 1.f);
        }
        if (Keyboard::isKeyPressed(Keyboard::S) && _Position.y < ScreenHeight - WallSize - (_Width / 2)) GameSprite::_Sprite.move(0, _MoveSpeed * _time);
        if (Keyboard::isKeyPressed(Keyboard::D) && _Position.x < ScreenWidth - WallSize - (_Width / 2))
        {
            GameSprite::_Sprite.move(_MoveSpeed * _time, 0);
            GameSprite::_Sprite.setScale(1.f, 1.f);
        }
    }
};

int main()
{

    //Konsola dla komunikatów błędów
    AllocConsole();
    freopen("CONOUT$", "w", stdout);
    printf("Console has open\n");

    //Właściwości Okna
    RenderWindow window(VideoMode(ScreenWidth, ScreenHeight), "Prosta Gra Przygodowa");
    window.setFramerateLimit(60);

    //Tworzenie Obiektów i dodawanie do nich textur
    GameSprite Floor("Textures/Room/floor.png", Vector2f(ScreenWidth / 2, ScreenHeight / 2));
    GameSprite LeftWall("Textures/Room/left_wall.png", Vector2f(WallSize / 2, ScreenHeight / 2));
    GameSprite RightWall("Textures/Room/right_wall.png", Vector2f(ScreenWidth - WallSize / 2, ScreenHeight / 2));
    GameSprite TopWall("Textures/Room/top_wall.png", Vector2f(ScreenWidth / 2, WallSize / 2));
    GameSprite BottomWall("Textures/Room/bottom_wall.png", Vector2f(ScreenWidth / 2, ScreenHeight - WallSize / 2));
    Player Hero("Textures/Hero/Hero.png", Vector2f(ScreenWidth / 2, ScreenHeight / 2));
    Bullet NewBullet = Bullet("Textures/Hero/Bullet.png", Hero.GetGameSprite().getPosition());

    //Wektor który chroni w sobie wszystkie pociski
    vector<Bullet> Bullets;

    //Klasa Myszki (Narazie potrzebna tylko dla strzałow)
    Mouse _Mouse;

    //Pętla okna
    while (window.isOpen())
    {
        Event event;

        //Zegar który pomaga w poruszniu obiektów bardziej płynnie
        _time = _Clock.getElapsedTime().asSeconds();
        _Clock.restart();

        while (window.pollEvent(event))
        {
            if (event.type == Event::Closed)
                window.close();
            //Warynek sprawdzający ODPUSZCZENIE klawiszy
            if (event.type == Event::MouseButtonReleased)
            {
                //Warynek sprawdzajacy czy odpuszczony klawisz był LEWYM przyciskiem MYSZY
                if (event.mouseButton.button == Mouse::Left)
                {
                    //Tworzenie nowego picisku i dodanie go dowektora
                    NewBullet.SetDirection(Hero.GetGameSprite().getPosition(), window.mapPixelToCoords(_Mouse.getPosition(window)));
                    Bullets.emplace_back(NewBullet);
                }
            }
        }
        
        //Wywołanie funkcji poruszania gracza
        Hero.Move();

        //Wyświetlanie wszystkich obiektów w oknie gry
        window.clear();
        window.draw(Floor.GetGameSprite());
        window.draw(LeftWall.GetGameSprite());
        window.draw(RightWall.GetGameSprite());
        window.draw(TopWall.GetGameSprite());
        window.draw(BottomWall.GetGameSprite());
        window.draw(Hero.GetGameSprite());
        //Pętla która wyświetla wszystkie pociski
        for (int i = 0; i < Bullets.size(); i++)
        {
            Bullets[i].Update(window);
        }
        window.display();
    }

    return 0;
}