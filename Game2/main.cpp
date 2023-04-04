#include <SFML/Graphics.hpp>
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
        _Position = _FPosition;
        _Texture.loadFromFile(_LoadFromFile);
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

    //Przesunięcie objektu
    void Move(Vector2f _Move)
    {
        _Sprite.move(_Move);
    }

    //Ustawienie pozycji objekta
    void SetPositionGameSprite(Vector2f _Position)
    {
        _Sprite.setPosition(_Position);
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
    //Konstruktor który wylicza kierunek strzału
    Bullet(String _LoadFromFile, Vector2f _Position, Vector2f _MousePosition) :GameSprite(_LoadFromFile, _Position)
    {
        _Direction = Vector2f(_MousePosition - _Position);
        float _Distance = static_cast<float>(sqrt(pow(_MousePosition.x - _Position.x, 2) + pow(_MousePosition.y - _Position.y, 2)));
        _Direction /= _Distance;
    }
    void Update()
    {
        _Sprite.move(_Direction * _BulletSpeed * _time);
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
        _Position = GameSprite::_Sprite.getPosition();
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

    vector<Bullet> Bullets;

    Mouse _Mouse;

    while (window.isOpen())
    {
        Event event;

        //Zegar który pomaga w poruszniu obiektów
        _time = _Clock.getElapsedTime().asSeconds();
        _Clock.restart();

        while (window.pollEvent(event))
        {
            if (event.type == Event::Closed)
                window.close();
            if (event.type == Event::MouseButtonReleased)
            {
                if (event.mouseButton.button == Mouse::Left)
                {
                    Bullet NewBullet = Bullet("Textures/Hero/Bullet.png", Hero.GetGameSprite().getPosition(), window.mapPixelToCoords(_Mouse.getPosition(window)));
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
        for (auto& bullet : Bullets)
        {
            window.draw(bullet.GetGameSprite());
            bullet.Update();
        }
        window.display();
    }

    return 0;
}