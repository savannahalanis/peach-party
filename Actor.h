#ifndef ACTOR_H_
#define ACTOR_H_

#include "GraphObject.h"

// Students:  Add code to this file, Actor.cpp, StudentWorld.h, and StudentWorld.cpp

// TODO: MAKE THINGS CONST

class StudentWorld;

class Actor : public GraphObject{
public:
    static const int invalid = -1;
    static const int waiting = 0;
    static const int walking = 1;
    
    Actor(int imageID, int startX, int startY, int sDir, int depth, double size, bool impactable, int mDir, StudentWorld* sWorld);
    virtual void doSomething() = 0;

    // accessors
    bool isAlive() const {return m_isAlive;}
    bool isImpactable() {return m_isImpactable;}
    // helps differentiate between sprite and moving direction
    int movingDirection() {return m_movingDirection;}
    StudentWorld* world() {return m_world;}
    int walkWaitStatus() {return m_walkWaitStatus;}
    int ticksToMove() {return m_ticksToMove;}

    // mutators
    int setMovingDirection(int dir);
    void changeDir(int futureXGrid, int futureYGrid);
    void walkNoForwardSquare();
    void walkMoveForward();
    void setWalkWaitStatus(int status) {m_walkWaitStatus = status;}
    void setTicksToMove(int t) {m_ticksToMove = t;}
    void changeTicksToMove(int t) {m_ticksToMove += t;}
    virtual void randomTeleport();
    // kills - lol
    void ruthlesslyObilerateAndDestroy() {m_isAlive = false;}
    

    // other
    Actor* overlappingEffectedActor(std::vector<Actor*> actors);
    bool isFork(bool& right, bool& up, bool& left, bool& down);
    void getGridValues (int& x, int& y);
    void getGridValues (int& x1, int& y1, int& x2, int& y2);

private:
    int m_movingDirection;
    bool m_isAlive;
    bool m_isImpactable;
    int m_walkWaitStatus;
    int m_ticksToMove;
    StudentWorld* m_world;
};

//////////////////////////////////////////////////////////////////
// MovingActors
//////////////////////////////////////////////////////////////////

class Vortex;

class PlayerAvatar: public Actor{
public:
    PlayerAvatar(int imageID, int startX, int startY, StudentWorld* world);
    
    // mutators
    void doSomething();
    void changeCoins(int c);
    void changeStars(int s);
    void resetCoins() {m_coins = 0;}
    void resetStars() {m_stars = 0;}
    void swapCoins(PlayerAvatar* other);
    void swapStars(PlayerAvatar* other);
    void makeLandableSquare() {m_isLandableSquare = true;}
    void makeUnlandableSquare() {m_isLandableSquare = false;}
    void makeLandableBaddie() {m_isLandableBaddie = true;}
    void makeUnlandableBaddie() {m_isLandableBaddie = false;}
    void recieveVortex() {m_hasVortex = true;}
    void walkMoveForward();
    bool walkForkAction();
    void swapTeleport(PlayerAvatar* other);
    
    // accessors
    int playerNum() { return m_playerNum; }
    int coins() { return m_coins; }
    int stars() { return m_stars; }
    bool hasVortex() {return m_hasVortex;}
    bool isLandableSquare() {return m_isLandableSquare;}
    bool isLandableBaddie () {return m_isLandableBaddie;}
    
private:
    int m_playerNum;
    int m_coins;
    int m_stars;
    bool m_hasVortex;
    bool m_isLandableSquare;
    bool m_isLandableBaddie;
};

class ActivatingObject: public Actor{
public:
    ActivatingObject(int imageID, int startX, int startY, int sDir, int depth, double size, bool impactable, int mDir, StudentWorld* world);
private:
    bool m_isVortexImpactable;
};

class ActivateOnPlayer: public ActivatingObject {
public:
    ActivateOnPlayer(int imageID, int startX, int startY, int sDir, int depth, double size, bool impactable, int mDir, PlayerAvatar* P1, PlayerAvatar* P2, StudentWorld* world);
    bool isOverlap(PlayerAvatar* affectedPlayer);

    // mutators
    void P1Activate() {m_P1isActive = true;}
    void P2Activate() {m_P2isActive = true;}
    void P1Deactivate() {m_P1isActive = false;}
    void P2Deactivate() {m_P2isActive = false;}

    // accessors
    PlayerAvatar* P1() {return m_P1;}
    PlayerAvatar* P2() {return m_P2;}
    bool P1isActive() {return m_P1isActive;}
    bool P2isActive() {return m_P2isActive;}

private:
    PlayerAvatar* m_P1;
    PlayerAvatar* m_P2;
    // ensures no repeating actions upon lingering 
    bool m_P1isActive;
    bool m_P2isActive;
};

class Baddie: public ActivateOnPlayer {
public:
    Baddie(int imageID, int startX, int startY, PlayerAvatar* p1, PlayerAvatar* p2, StudentWorld* world);
    int pauseCounter() {return m_pauseCounter;}
    void changePauseCounter(int p) {m_pauseCounter += p;}
    void setPauseCounter(int pc) {m_pauseCounter = pc;}
    void newRandomDir();
    void pauseCounterAction();
    virtual void walkingAction();
    void randomTeleport();
private:
    int m_pauseCounter;
};

class Bowser: public Baddie {
public:
    Bowser(int startX, int startY, PlayerAvatar* p1, PlayerAvatar* p2, StudentWorld* world);
    void doSomething();
    bool possibleLoseAllCoins(PlayerAvatar* affectedPlayer);
    void walkingAction();
};

class Boo: public Baddie {
public:
    Boo(int startX, int startY, PlayerAvatar* p1, PlayerAvatar* p2,StudentWorld* world);
    void doSomething();
    void waitingAction(PlayerAvatar* first, PlayerAvatar* second);
};

class Vortex: public ActivatingObject {
public:
    Vortex(int startX, int startY, int mDir, StudentWorld* world);
    void doSomething();
 };

//////////////////////////////////////////////////////////////////
// Squares - (but they are circle????)
//////////////////////////////////////////////////////////////////

class BankSquare: public ActivateOnPlayer {
public:
    BankSquare(int startX, int startY, int* bank, PlayerAvatar* p1, PlayerAvatar* p2, StudentWorld* world);
    void doSomething();
    void giveCoins(PlayerAvatar* player);
    void takeCoins(PlayerAvatar* player);
private:
    int* m_bankPtr;
};

class CoinSquare: public ActivateOnPlayer {
public:
    CoinSquare(int imageID, int startX, int startY, PlayerAvatar* p1, PlayerAvatar* p2, StudentWorld* world);
    void doSomething();
private:
    int m_coinChange;
    int m_coinSound;
};

class StarSquare: public ActivateOnPlayer {
public:
    StarSquare(int startX, int startY, PlayerAvatar* p1, PlayerAvatar* p2, StudentWorld* world);
    void doSomething();
};

class EventSquare: public ActivateOnPlayer {
public:
    EventSquare(int startX, int startY, PlayerAvatar* p1, PlayerAvatar* p2, StudentWorld* world);
    void doSomething();
    void teleportPlayer(PlayerAvatar* player);
    void swapPlayers(PlayerAvatar* player);
    void giveVortex(PlayerAvatar* player);
};

class DroppingSquare: public ActivateOnPlayer {
public:
    DroppingSquare(int startX, int startY, PlayerAvatar* p1, PlayerAvatar* p2, StudentWorld* world);
    void doSomething();
    void deductCoin();
    void deductStar();
};

class DirectionalSquare: public ActivateOnPlayer {
public:
    DirectionalSquare(int startX, int startY, int dir, PlayerAvatar* p1, PlayerAvatar* p2, StudentWorld* world);
    void doSomething();
};





#endif // ACTOR_H_
