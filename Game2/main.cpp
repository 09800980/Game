#define _CRT_SECURE_NO_WARNINGS
#include <SFML/Graphics.hpp>
#include <cstdlib>
#include <windows.h>
#include <list>
#include <chrono>

using namespace sf;
using namespace std;

//Rozmiary Okna
#define ScreenWidth 1280
#define ScreenHeight 720
#define WallSize 96

//Globalny obiekt i zmienna Zegara 
Clock GlobalClock;
float GlobalTime = 0.0;

//Klasa Bazowa dla obiektów którzy muszą być wyświetlone
class GameSprite
{
private:
    Texture _Texture;
protected:
    Sprite _Sprite;
    int _PixelWidth, _PixelHeight;
    IntRect _FrameSprite;
public:
    //Konstruktor obiektu który tworzy sprajty dla późniejszego odrysowania
    GameSprite(String LoadFromFile, Vector2f Position, int PixelWidth = 0, int PixelHeight = 0)
    {
        if (!_Texture.loadFromFile(LoadFromFile)) 
            printf("%s Don`t Open\n", LoadFromFile.toAnsiString().c_str());
        _Sprite.setTexture(_Texture);
        if (PixelWidth != 0 && PixelHeight != 0)
        {
            _PixelHeight = PixelHeight;
            _PixelWidth = PixelWidth;
        }
        else {
            _PixelWidth = _Sprite.getLocalBounds().width;
            _PixelHeight = _Sprite.getLocalBounds().height;
        }
        _FrameSprite = IntRect(0, 0, _PixelWidth, _PixelHeight);
        _Sprite.setTextureRect(_FrameSprite);
        _Sprite.setOrigin(_Sprite.getLocalBounds().width / 2, _Sprite.getLocalBounds().height / 2);
        _Sprite.setPosition(Position);
    }

    //Metoda Ustawiająca skale objekta
    void SetScale(Vector2f Scale)
    {
        _Sprite.setScale(Scale);
    }

    //Pobieranie Obiektu dla odrysowania go na ekranie
    Sprite GetGameSprite()
    {
        return _Sprite;
    }
};

//Klasa dla Sprajtów z animacją
class Animated : public GameSprite
{
private:
    const Time _FrameTime = seconds(0.1f);
protected:
    int _top, _left;
public:
    //Konstruktor dla dla inicjalizacji zegara
    Animated(String LoadFromFile, Vector2f Position, int PixelWidth = 0, int PixelHeight = 0) :GameSprite(LoadFromFile, Position, PixelWidth, PixelHeight)
    {
        _top = 0;
        _left = 0;
        _PixelHeight = PixelHeight;
        _PixelWidth = PixelWidth;
    }

    //Metoda zmiany kadrów
    void Animate(int _top, int _left = -1)
    {
        chrono::high_resolution_clock::time_point CurrentFrameTime = chrono::high_resolution_clock::now();
        static chrono::high_resolution_clock::time_point PreviousFrameTime = chrono::high_resolution_clock::now();
        chrono::duration<float> TimeSinceLastFrame = CurrentFrameTime - PreviousFrameTime;
        static int left = 0;
        static int top = 0;
        top = _top;
        if (_left != -1)
            left = _left;
        CurrentFrameTime = chrono::high_resolution_clock::now();
        TimeSinceLastFrame = CurrentFrameTime - PreviousFrameTime;
        _FrameSprite.left = left * _PixelWidth;
        _FrameSprite.top = top * _PixelHeight;
        if (TimeSinceLastFrame.count() >= _FrameTime.asSeconds())
        {
            left = (left + 1) % 4;
            PreviousFrameTime = CurrentFrameTime;
        }

        _Sprite.setTextureRect(_FrameSprite);
    }
};

//Klasa wroga
class Enemy : public Animated
{
private:
    const float _MoveSpeed = 90.0;
public:
    //Konstruktor wroga
    Enemy(String LoadFromFile, Vector2f Position) :Animated(LoadFromFile, Position, 16, 16)
    {
        SetScale(Vector2f(5.f, 5.f));
    }
    
    //Metoda poruszania wroga
    void Move(Vector2f _Direction)
    {
        if (_Direction.y < 0 && abs(_Direction.y) > abs(_Direction.x))
        {
            _top = 3;
        }
        if (_Direction.x < 0 && abs(_Direction.x) > abs(_Direction.y))
        {
            _top = 1;
        }
        if (_Direction.y > 0 && abs(_Direction.y) > abs(_Direction.x))
        {
            _top = 0;
        }
        if (_Direction.x > 0 && abs(_Direction.x) > abs(_Direction.y))
        {
            _top = 2;
        }
        _Sprite.move(_Direction* GlobalTime* _MoveSpeed);
        
        Animate(_top);
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
    Bullet(String LoadFromFile, Vector2f Position, Vector2f Direction) :GameSprite(LoadFromFile, Position)
    {
        _Direction = Direction;
        SetScale(Vector2f(0.8f, 0.8f));
    }

    //Metoda która przemieszca pocisk i odrysowuje go na ekranie
    void Update(RenderWindow& window)
    {
        _Sprite.move(_Direction * _BulletSpeed * GlobalTime);
        window.draw(_Sprite);
    }
};

//Klasa Gracza
class Player : public Animated
{
private:
    Vector2f _Move;
    const float _MoveSpeed = 150.0;
    bool _IsMove;
public:
    //Konstruktor Bohatera
    Player(String LoadFromFile, Vector2f Position) :Animated(LoadFromFile, Position, 16, 16)
    {
        _IsMove = false;
        SetScale(Vector2f(5.f, 5.f));
    }

    //Funkcja poruszania się gracza
    void Move()
    {
        Vector2f _Position = _Sprite.getPosition();
        if (Keyboard::isKeyPressed(Keyboard::W) || Keyboard::isKeyPressed(Keyboard::A) || Keyboard::isKeyPressed(Keyboard::S) || Keyboard::isKeyPressed(Keyboard::D))
        {
            _IsMove = true;
            if (Keyboard::isKeyPressed(Keyboard::A) && _Position.x > WallSize + _PixelWidth / 2)
            {
                _top = 1;
                GameSprite::_Sprite.move(-_MoveSpeed * GlobalTime, 0);
            }
            if (Keyboard::isKeyPressed(Keyboard::D) && _Position.x < ScreenWidth - WallSize - _PixelWidth / 2)
            {
                _top = 2;
                GameSprite::_Sprite.move(_MoveSpeed * GlobalTime, 0);
            }
            if (Keyboard::isKeyPressed(Keyboard::S) && _Position.y < ScreenHeight - WallSize - _PixelWidth / 2)
            {
                _top = 0;
                GameSprite::_Sprite.move(0, _MoveSpeed * GlobalTime);
            }
            if (Keyboard::isKeyPressed(Keyboard::W) && _Position.y > WallSize)
            {
                _top = 3;
                GameSprite::_Sprite.move(0, -_MoveSpeed * GlobalTime);
            }
        }
        else 
            _IsMove = false;

        if (!_IsMove)
            _left = -1;
        else
            this->Animate(_top, _left);
    }
};

//Funkcja która tworzy nowych wrogów
void SpawnEnemy(list<Enemy>* Enemies, String LoadFromFile, Player Hero)
{
    while (true)
    {
        int x = rand() % (ScreenWidth - WallSize * 3) + WallSize, y = rand() % (ScreenHeight - WallSize * 3) + WallSize;
        if (x > Hero.GetGameSprite().getPosition().x + 220 || x < Hero.GetGameSprite().getPosition().x - 220)
        {
            if (y > Hero.GetGameSprite().getPosition().y + 220 || y < Hero.GetGameSprite().getPosition().y - 220)
            {
                printf("%d\t%d\n", x, y);
                Enemies->emplace_back(LoadFromFile, Vector2f(x,y));
                return;
            }
            else {
                y = rand() % (ScreenHeight - WallSize * 2) + WallSize;
            }
        }
        else {
            x = rand() % (ScreenWidth - WallSize * 2) + WallSize;
        }
    }
}

//Funkcja wyliczająca kierunek poruszania przez wektor bazowy
Vector2f CalculateDirection(Vector2f To, Vector2f From)
{
    Vector2f Direction = Vector2f(To - From);
    float Distance = static_cast<float>(sqrt(pow(To.x - From.x, 2) + pow(To.y - From.y, 2)));
    Direction /= Distance;
    return Direction;
}

int main()
{
    srand(time(NULL));
    //Konsola dla komunikatów błędów
    AllocConsole();
    (void)freopen("CONOUT$", "w", stdout);
    printf("Console has open\n");

    //Właściwości Okna
    RenderWindow window(VideoMode(ScreenWidth, ScreenHeight), "Prosta Gra Przygodowa");
    window.setFramerateLimit(60);

    Start:
    //Tworzenie Obiektów i dodawanie do nich textur
    GameSprite Floor("Textures/Room/floor.png", Vector2f(ScreenWidth / 2, ScreenHeight / 2));
    Player Hero("Textures/Hero/Hero.png", Vector2f(ScreenWidth / 2, ScreenHeight / 2));

    //Lista która przechowuje w sobie wszystkie pociski
    list<Bullet> Bullets;

    //Lista która przechowuje wrogów
    list<Enemy> Enemies;

    //Lista która przechowuje ściany 
    list<GameSprite> Walls;
    Walls.emplace_back("Textures/Room/left_wall.png", Vector2f(WallSize / 2, ScreenHeight / 2));
    Walls.emplace_back("Textures/Room/right_wall.png", Vector2f(ScreenWidth - WallSize / 2, ScreenHeight / 2));
    Walls.emplace_back("Textures/Room/top_wall.png", Vector2f(ScreenWidth / 2, WallSize / 2));
    Walls.emplace_back("Textures/Room/bottom_wall.png", Vector2f(ScreenWidth / 2, ScreenHeight - WallSize / 2));

    //Klasa Myszki (Narazie potrzebna tylko dla strzałow)
    Mouse _Mouse;

    bool IsSpawn = false;
    bool IsShot = false;

    //Pętla okna
    while (window.isOpen())
    {
        Event event;

        //Zegar który pomaga w poruszniu obiektów bardziej płynnie
        GlobalTime = GlobalClock.getElapsedTime().asSeconds();
        GlobalClock.restart();

        while (window.pollEvent(event))
        {
            if (event.type == Event::Closed)
                window.close();
            //Warynek sprawdzający odpuszcenie klawiszy klawiszy
            if (event.type == Event::MouseButtonReleased)
            {
                //Warynek sprawdzajacy czy odpuszczony klawisz był lewym przyciskiem MYSZY
                if (event.mouseButton.button == Mouse::Left)
                    IsShot = true;
                if (event.mouseButton.button == Mouse::Right)
                    IsSpawn = true;
            }
        }

        if (IsSpawn)
        {
            IsSpawn = false;
            SpawnEnemy(&Enemies, "Textures/Enemy/Skeleton.png", Hero);
        }
        if (IsShot)
        {
            IsShot = false;
            Bullets.emplace_back("Textures/Hero/Bullet.png", Hero.GetGameSprite().getPosition(), CalculateDirection(window.mapPixelToCoords(_Mouse.getPosition(window)), Hero.GetGameSprite().getPosition()));
        }

        //Wywołanie funkcji poruszania gracza
        Hero.Move();

        //Wyświetlanie wszystkich obiektów w oknie gry
        window.clear();
        window.draw(Floor.GetGameSprite());
        for(auto& WallsIt : Walls)
        {
            window.draw(WallsIt.GetGameSprite());
        }
        for(auto& EnemiesIt : Enemies)
        {
            EnemiesIt.Move(CalculateDirection(Hero.GetGameSprite().getPosition(), EnemiesIt.GetGameSprite().getPosition()));
            window.draw(EnemiesIt.GetGameSprite());
        }
        window.draw(Hero.GetGameSprite());
        for(auto BulletsIt = Bullets.begin(); BulletsIt != Bullets.end();)
        {
            BulletsIt->Update(window);
            bool _IsBulletDelete = false;
            bool _IsEnemyDelete = false;
            for (auto& WallsIt : Walls)
            {
                if(WallsIt.GetGameSprite().getGlobalBounds().intersects(BulletsIt->GetGameSprite().getGlobalBounds()))
                {
                    BulletsIt = Bullets.erase(BulletsIt);
                    _IsBulletDelete = true;
                    break;
                }
            }
            if (BulletsIt == Bullets.end()) break;
            for(auto EnemiesIt = Enemies.begin(); EnemiesIt != Enemies.end();)
            {
                if (EnemiesIt->GetGameSprite().getGlobalBounds().intersects(BulletsIt->GetGameSprite().getGlobalBounds()))
                {
                    BulletsIt = Bullets.erase(BulletsIt);
                    EnemiesIt = Enemies.erase(EnemiesIt);
                    _IsBulletDelete = true;
                    _IsEnemyDelete = true;
                    break;
                }
                if (!_IsEnemyDelete)
                    ++EnemiesIt;
            }
            if(!_IsBulletDelete)
                ++BulletsIt;
        }
        for (auto& EnemiesIt : Enemies)
        {
            if (EnemiesIt.GetGameSprite().getGlobalBounds().intersects(Hero.GetGameSprite().getGlobalBounds()))
            {
                GameSprite GameOver("Textures/Menu/GameOver.jpg", Vector2f(ScreenWidth/2, ScreenHeight/2));
                bool Restart = false;
                while(!Restart)
                {
                    window.draw(GameOver.GetGameSprite());
                    window.display();
                    while (window.pollEvent(event))
                    {
                        if (event.type == sf::Event::KeyPressed)
                        {
                            if (event.key.code == sf::Keyboard::R)
                            {
                                Restart = true;
                            }
                        }
                    }
                }
                goto Start;
            }
        }
        window.display();
    }
    return 0;
}