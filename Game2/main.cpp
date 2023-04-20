#define _CRT_SECURE_NO_WARNINGS
#include <SFML/Graphics.hpp>
#include <cstdlib>
#include <windows.h>
#include <list>
#include <chrono>

using namespace sf;
using namespace std;

chrono::high_resolution_clock::time_point previousFrameTime = chrono::high_resolution_clock::now();
chrono::duration<float> timeSinceLastFrame;

//Rozmiary Okna
#define ScreenWidth 1280
#define ScreenHeight 720
#define WallSize 96

//Globalny obiekt i zmienna Zegara 
Clock _Clock;
float _time = 0.0;

//Funkcja wyliczająca kierunek poruszania przez wektor bazowy
Vector2f CalculateDirection(Vector2f _From, Vector2f _To)
{
    Vector2f _Direction = Vector2f(_From - _To);
    float _Distance = static_cast<float>(sqrt(pow(_To.x - _From.x, 2) + pow(_To.y - _From.y, 2)));
    _Direction /= _Distance;
    return _Direction;
}

//Klasa Bazowa dla obiektów którzy muszą być wyświetlone
class GameSprite
{
protected:
    Texture _Texture;
    Sprite _Sprite;
    float _Width, _Height;
    IntRect _FrameSprite;
public:
    //Konstruktor obiektu która tworzy sprajty dla późniejszego odrysowania
    GameSprite(String _LoadFromFile, Vector2f _Position, Vector2f _Scale = Vector2f(0.f, 0.f))
    {
        if (!_Texture.loadFromFile(_LoadFromFile)) printf("%s Don`t Open\n", _LoadFromFile.toAnsiString().c_str());
        _Sprite.setTexture(_Texture);
        if (_Scale != Vector2f(0.f, 0.f))
            _Sprite.setScale(_Scale);
        _Width = _Sprite.getLocalBounds().width;
        _Height = _Sprite.getLocalBounds().height;
        _Sprite.setOrigin(_Sprite.getLocalBounds().width / 2, _Sprite.getLocalBounds().height / 2);
        _Sprite.setPosition(_Position);
    }

    GameSprite(String _LoadFromFile, Vector2f _Position, int _width, int _height, Vector2f _Scale = Vector2f(0.f, 0.f))
    {
        if (!_Texture.loadFromFile(_LoadFromFile)) printf("%s Don`t Open\n", _LoadFromFile.toAnsiString().c_str());
        _Sprite.setTexture(_Texture);
        if (_Scale != Vector2f(0.f, 0.f))
            _Sprite.setScale(_Scale);

        this->_FrameSprite.left = 0;
        this->_FrameSprite.top = 0;
        this->_FrameSprite.width = 32;
        this->_FrameSprite.height = 32;
        _Sprite.setTextureRect(_FrameSprite);

        _Width = _Sprite.getLocalBounds().width;
        _Height = _Sprite.getLocalBounds().height;
        _Sprite.setOrigin(_Sprite.getLocalBounds().width / 2, _Sprite.getLocalBounds().height / 2);
        _Sprite.setPosition(_Position);
    }

    //Pobieranie Obiektu dla odrysowania go na ekranie
    Sprite GetGameSprite()
    {
        return _Sprite;
    }
};

//Klasa wroga
class Enemy :public GameSprite
{
private:
    const float _MoveSpeed = 100.0;
    Vector2f _Direction;
    bool _IsMove;
public:
    //Konstruktor wroga
    Enemy(String _LoadFromFile, Vector2f _Position, int _width, int _height, Vector2f _Scale) :GameSprite(_LoadFromFile, _Position, _width, _height, _Scale)
    {
        _IsMove = false;
    }
    
    //Metoda poruszania wroga
    void Move(Vector2f _Direction)
    {
        chrono::high_resolution_clock::time_point EnemyCurrentFrameTime = chrono::high_resolution_clock::now();
        static chrono::high_resolution_clock::time_point EnemyPreviousFrameTime = previousFrameTime;
        chrono::duration<float> EnemyTimeSinceLastFrame = EnemyCurrentFrameTime - EnemyPreviousFrameTime;
        const Time _FrameTime = seconds(0.1f);
        static int _left = 0;
        static int _top = 0;

        _FrameSprite.left = _left*32;
        _FrameSprite.top = _top;

        if (_Direction.y < 0 && abs(_Direction.y) > abs(_Direction.x))
        {
            _top = 7 * 32;
        }
        if (_Direction.x < 0 && abs(_Direction.x) > abs(_Direction.y))
        {
            _top = 5 * 32;
        }
        if (_Direction.y > 0 && abs(_Direction.y) > abs(_Direction.x))
        {
            _top = 4 * 32;
        }
        if (_Direction.x > 0 && abs(_Direction.x) > abs(_Direction.y))
        {
            _top = 6 * 32;
        }

        if (EnemyTimeSinceLastFrame.count() >= _FrameTime.asSeconds())
        {
            _left = (_left + 1) % 4;
            EnemyPreviousFrameTime = EnemyCurrentFrameTime;
        }

        _Sprite.setTextureRect(_FrameSprite);

        this->_Direction = _Direction;
        printf("%d\n", _left);

        _Sprite.move(this->_Direction*_time*_MoveSpeed);
    }

    //Metoda aktualizacji wroga
    void Update(Vector2f _Direction)
    {
        Move(_Direction);
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
    //Konstruktor który pobiera ustawia kierunek strzału
    Bullet(String _LoadFromFile, Vector2f _Position, Vector2f _Direction, Vector2f _Scale) :GameSprite(_LoadFromFile, _Position, _Scale)
    {
        this->_Direction = _Direction;
    }

    //Metoda która przemieszca pocisk i odrysowuje go na ekranie
    void Update(RenderWindow& _window)
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
    bool _IsMove;
public:
    //Konstruktor Bohatera
    Player(String _LoadFromFile, Vector2f _Position, int _width, int _height, Vector2f _Scale) :GameSprite(_LoadFromFile, _Position, _width, _height, _Scale)
    {
        _IsMove = false;
    }

    //Funkcja porusznia się gracza
    void Move()
    {
        chrono::high_resolution_clock::time_point HeroCurrentFrameTime = chrono::high_resolution_clock::now();
        static chrono::high_resolution_clock::time_point HeroPreviousFrameTime = previousFrameTime;
        chrono::duration<float> HeroTimeSinceLastFrame = HeroCurrentFrameTime - HeroPreviousFrameTime;
        const Time _FrameTime = seconds(0.1f);
        static int _left = 0;
        static int _top = 0;
        if (!_IsMove)
        {
            _left = 0;
        }
        _FrameSprite.left = _left*32;
        _FrameSprite.top = _top;
        Vector2f _Position = _Sprite.getPosition();
        if (Keyboard::isKeyPressed(Keyboard::W) || Keyboard::isKeyPressed(Keyboard::A) || Keyboard::isKeyPressed(Keyboard::S) || Keyboard::isKeyPressed(Keyboard::D))
        {
            _IsMove = true;
            if (Keyboard::isKeyPressed(Keyboard::W) && _Position.y > WallSize)
            {
                _top = 7 * 32;
                GameSprite::_Sprite.move(0, -_MoveSpeed * _time);
            }
            if (Keyboard::isKeyPressed(Keyboard::A) && _Position.x > WallSize + _Width / 2)
            {
                _top = 5 * 32;
                GameSprite::_Sprite.move(-_MoveSpeed * _time, 0);
            }
            if (Keyboard::isKeyPressed(Keyboard::S) && _Position.y < ScreenHeight - WallSize - _Width / 2)
            {
                _top = 4 * 32;
                GameSprite::_Sprite.move(0, _MoveSpeed * _time);
            }
            if (Keyboard::isKeyPressed(Keyboard::D) && _Position.x < ScreenWidth - WallSize - _Width / 2)
            {
                _top = 6 * 32;
                GameSprite::_Sprite.move(_MoveSpeed * _time, 0);
            }
        }
        else _IsMove = false;
        if (HeroTimeSinceLastFrame.count() >= _FrameTime.asSeconds() && _IsMove)
        {
            _left = (_left + 1) % 4;
            HeroPreviousFrameTime = HeroCurrentFrameTime;
        }
        _Sprite.setTextureRect(_FrameSprite);
    }
};

int main()
{
    //Konsola dla komunikatów błędów
    AllocConsole();
    (void)freopen("CONOUT$", "w", stdout);
    printf("Console has open\n");

    //Właściwości Okna
    RenderWindow window(VideoMode(ScreenWidth, ScreenHeight), "Prosta Gra Przygodowa");
    window.setFramerateLimit(60);

    //Tworzenie Obiektów i dodawanie do nich textur
    GameSprite Floor("Textures/Room/floor.png", Vector2f(ScreenWidth / 2, ScreenHeight / 2));
    Player Hero("Textures/Hero/Hero.png", Vector2f(ScreenWidth / 2, ScreenHeight / 2), 32, 32, Vector2f(4.5f, 4.5f));
    Enemy Skeleton("Textures/Enemy/Skeleton.png", Vector2f(150, 150), 32, 32, Vector2f(4.5f, 4.5f));

    //Wektor który przechowuje w sobie wszystkie pociski
    list<Bullet> Bullets;

    //Wektor który przechowuje ściany 
    list<GameSprite> Walls;
    Walls.emplace_back("Textures/Room/left_wall.png", Vector2f(WallSize / 2, ScreenHeight / 2));
    Walls.emplace_back("Textures/Room/right_wall.png", Vector2f(ScreenWidth - WallSize / 2, ScreenHeight / 2));
    Walls.emplace_back("Textures/Room/top_wall.png", Vector2f(ScreenWidth / 2, WallSize / 2));
    Walls.emplace_back("Textures/Room/bottom_wall.png", Vector2f(ScreenWidth / 2, ScreenHeight - WallSize / 2));

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
            //Warynek sprawdzający odpuszcenie klawiszy klawiszy
            if (event.type == Event::MouseButtonReleased)
            {
                //Warynek sprawdzajacy czy odpuszczony klawisz był lewym przyciskiem MYSZY
                if (event.mouseButton.button == Mouse::Left)
                {
                    Bullets.emplace_back("Textures/Hero/Bullet.png", Hero.GetGameSprite().getPosition(), CalculateDirection(window.mapPixelToCoords(_Mouse.getPosition(window)), Hero.GetGameSprite().getPosition()), Vector2f(0.8f, 0.8f));
                }
            }
        }

        //Wywołanie funkcji poruszania gracza
        Hero.Move();
        Skeleton.Update(CalculateDirection(Hero.GetGameSprite().getPosition(), Skeleton.GetGameSprite().getPosition()));

        //Wyświetlanie wszystkich obiektów w oknie gry
        window.clear();
        window.draw(Floor.GetGameSprite());
        for (auto& WallsIt : Walls)
        {
            window.draw(WallsIt.GetGameSprite());
        }
        window.draw(Skeleton.GetGameSprite());
        window.draw(Hero.GetGameSprite());
        for(auto BulletsIt = Bullets.begin(); BulletsIt != Bullets.end();)
        {
            BulletsIt->Update(window);
            bool _IsDelete = false;
            for (auto& WallsIt : Walls)
            {
                if (WallsIt.GetGameSprite().getGlobalBounds().intersects(BulletsIt->GetGameSprite().getGlobalBounds()))
                {
                    BulletsIt = Bullets.erase(BulletsIt);
                    _IsDelete = true;
                    break;
                }
            }
            if (!_IsDelete)
                ++BulletsIt;
        }
        window.display();
    }
    return 0;
}