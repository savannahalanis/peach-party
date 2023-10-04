#include "Actor.h"
#include "StudentWorld.h"
#include <algorithm>
#include <cmath>
#include <vector>
//#include "GameWorld.h"

// Students:  Add code to this file, Actor.h, StudentWorld.h, and StudentWorld.cpp

//////////////////////////////////////////////////////////////////
// Actor
//////////////////////////////////////////////////////////////////

Actor::Actor(int imageID, int startX, int startY, int sDir, int depth, double size, bool impactable, int mDir, StudentWorld* world) : GraphObject(imageID, startX, startY, sDir, depth, size) {
    m_movingDirection = mDir;
    m_isAlive = true;
    m_isImpactable = impactable;
    m_world = world;
    m_ticksToMove = 0;
};

int Actor::setMovingDirection(int dir) {
    // sets moving direction and gives proper sprite direction
    switch(dir) {
        case right:
            m_movingDirection = right;
            setDirection(0);
            return 0;
        case up:
            m_movingDirection = up;
            setDirection(0);
            return 0;
        case left:
            m_movingDirection = left;
            setDirection(180);
            return 0;
        case down:
            m_movingDirection = down;
            setDirection(0);
            return 0;
    }
    return -1;
}

bool Actor::isFork(bool& r, bool& u, bool& l, bool& d) {
    // if a player is exactly on a spot (picture is in the middle of a square)
    if (!(getX()%16 == 0 && getY()%16 == 0))
        return false;
    int surroundingSquares = 0;

    // check right
    int surroundingX = getX();
    int surroundingY = getY();
    getPositionInThisDirection(right, 16, surroundingX, surroundingY);
    
    if(world()->board().getContentsOf(surroundingX/16, surroundingY/16) != Board::empty) {
            surroundingSquares++;
            r = true;
    } else
        r = false;

    // check up
    surroundingX = getX();
    surroundingY = getY();
    getPositionInThisDirection(up, 16, surroundingX, surroundingY);
    if(world()->board().getContentsOf(surroundingX/16, surroundingY/16) != Board::empty) {
            surroundingSquares++;
            u = true;
    } else
        u = false;

    // check left
    surroundingX = getX();
    surroundingY = getY();
    getPositionInThisDirection(left, 16, surroundingX, surroundingY);
    if(world()->board().getContentsOf(surroundingX/16, surroundingY/16) != Board::empty) {
            surroundingSquares++;
            l = true;
    } else
        l = false;

    // check down
    surroundingX = getX();
    surroundingY = getY();
    getPositionInThisDirection(down, 16, surroundingX, surroundingY);
    if(world()->board().getContentsOf(surroundingX/16, surroundingY/16) != Board::empty) {
            surroundingSquares++;
            d = true;
    } else
        d = false;

    return surroundingSquares > 2;
}

void Actor::changeDir(int futureXGrid, int futureYGrid) {
    bool isBackUpVertical = false;
    bool isBackUpHorizontal = false;
    
    while (0 == 0) {
        // prioritizes moving up/down for former right/left
        if (((movingDirection() == right || movingDirection() == left) && !isBackUpVertical) || isBackUpVertical) {
            //check for up
            getPositionInThisDirection(up, 16, futureXGrid, futureYGrid);
            getGridValues(futureXGrid, futureYGrid);
            if (world()->board().getContentsOf(futureXGrid, futureYGrid) != Board::empty) {
                setMovingDirection(up);
                setDirection(0);
                return;
            }
            // check for down
            getPositionInThisDirection(down, 16, futureXGrid, futureYGrid);
            getGridValues(futureXGrid, futureYGrid);
            if (world()->board().getContentsOf(futureXGrid, futureYGrid) != Board::empty) {
                setMovingDirection(down);
                setDirection(0);
                return;
            }
            isBackUpHorizontal = true;
        }
        // prioritizes moving up/down for former right/left
        else if ( ( (movingDirection() == up|| movingDirection() == down ) && !isBackUpHorizontal ) || isBackUpHorizontal) {
            // check right
            getPositionInThisDirection(right, 16, futureXGrid, futureYGrid);
            getGridValues(futureXGrid, futureYGrid);
            if (world()->board().getContentsOf(futureXGrid, futureYGrid) != Board::empty) {
                setMovingDirection(right);
                setDirection(0);
                return;
            }
            // check left
            getPositionInThisDirection(left, 16, futureXGrid, futureYGrid);
            getGridValues(futureXGrid, futureYGrid);
            if (world()->board().getContentsOf(futureXGrid, futureYGrid) != Board::empty) {
                setMovingDirection(left);
                setDirection(180);
                return;
            }
            isBackUpVertical = true;
        }
    }
}

void Actor::walkNoForwardSquare() {
    // name signifies it fixes the issue of not knowing if there is no forward square infront of you
        int currGraphX = getX();
        int currGraphY = getY();
        int futureGraphX = getX();
        int futureGraphY = getY();
        
        getPositionInThisDirection(movingDirection(), 16, futureGraphX, futureGraphY);
        getGridValues(currGraphX, currGraphY, futureGraphX, futureGraphY);
        
        // changes direction if next spot is open
        if ((getX() % 16 == 0) && (getY() % 16 == 0)) {
            switch(world()->board().getContentsOf(futureGraphX, futureGraphY)){
                case Board::empty: {
                    changeDir(currGraphX, currGraphY);
                }
                default:
                    break;
                    
            }
        }
}

void Actor::walkMoveForward() {
int futureX, futureY;
        getPositionInThisDirection(movingDirection(), 2, futureX, futureY);
        moveTo(futureX, futureY);
        getGridValues(futureX, futureY);
        changeTicksToMove(-1);
        if (ticksToMove() == 0)
            setWalkWaitStatus(waiting);
}

void Actor::getGridValues(int& x, int& y) {
    x = x/16;
    y = y/16;
}

void Actor::getGridValues(int& x1, int& y1, int& x2, int& y2) {
    getGridValues(x1, y1);
    getGridValues(x2, y2);
}

void Actor::randomTeleport() {
    int x, y;
    // looks for open spot until it finds one
    while(0 == 0) {
        x = randInt(0, 15);
        y = randInt(0, 15);
        if ((world()->board().getContentsOf(x, y) != Board::empty))
            break;
    }
    moveTo(x*16, y*16);
}

//////////////////////////////////////////////////////////////////
// PlayerAvatar
//////////////////////////////////////////////////////////////////

PlayerAvatar::PlayerAvatar(int imageID, int startX, int startY, StudentWorld* world) : Actor(imageID, startX, startY, 0, 0, 1, false, right, world) {
    switch(imageID) {
        case IID_PEACH:
            m_playerNum = 1;
            break;
        case IID_YOSHI:
            m_playerNum = 2;
    }
    m_coins = 0;
    setWalkWaitStatus(waiting);
    m_isLandableSquare = true;
    m_isLandableBaddie = true;
    m_hasVortex = false;
};

void PlayerAvatar::doSomething() {
    if (walkWaitStatus() == waiting) {
        switch (world()->getAction(m_playerNum)) {
            case ACTION_ROLL: {
                int dice_roll = randInt(1, 10);
                setTicksToMove(dice_roll * 8);
                setWalkWaitStatus(walking);
            }
                break;
            case ACTION_FIRE:
                if(m_hasVortex == true) {
                    world()->createVortex(this);
                    world()->playSound(SOUND_PLAYER_FIRE);
                    m_hasVortex = false;
                }
                break;
            default:
                return;
        }
    }

    if (walkWaitStatus() == walking) {
        if(world()->board().getContentsOf(getX()/16, getY()/16) != Board::right_dir_square && world()->board().getContentsOf(getX()/16, getY()/16) != Board::up_dir_square && world()->board().getContentsOf(getX()/16, getY()/16) != Board::left_dir_square && world()->board().getContentsOf(getX()/16, getY()/16) != Board::down_dir_square && !walkForkAction())
            return;
        else
            walkNoForwardSquare();
        
        walkMoveForward();
    }
    
}

bool PlayerAvatar::walkForkAction() {
    // if the Player is at a fork, then allows player to choose direction
    bool r, u, l, d;
    if (isFork(r, u, l, d)) {
        int newMDir = world()->getAction(m_playerNum);
        switch(newMDir) {
            case ACTION_RIGHT:
                if (movingDirection() != left)
                    setMovingDirection(right);
                else return false;
                break;
            case ACTION_UP:
                if (movingDirection() != down)
                    setMovingDirection(up);
                else return false;
                break;
            case ACTION_LEFT:
                if (movingDirection() != right)
                    setMovingDirection(left);
                else return false;
                break;
            case ACTION_DOWN:
                if (movingDirection() != up)
                    setMovingDirection(down);
                else return false;
                break;
            default:
                return false;
        }
    }
    return true;
}

void PlayerAvatar::changeCoins(int c) {
    // accounts for negative coins
    if (c < 0 && abs(c) > coins())
        resetCoins();
    else
        m_coins += c;
}

void PlayerAvatar::changeStars(int s) {
    // accounts for negative stars
    if (s < 0 && abs(s) > stars())
        resetStars();
    else
        m_stars += s;
}

void PlayerAvatar::swapStars(PlayerAvatar* other){
    std::swap(m_stars, other->m_stars);
}

void PlayerAvatar::swapCoins(PlayerAvatar* other) {
    std::swap(m_coins, other->m_coins);
}

void PlayerAvatar::swapTeleport(PlayerAvatar* other) {
    // swap coords
    int tempX = getX();
    int tempY = getY();
    moveTo(other->getX(), other->getY());
    other->moveTo(tempX, tempY);

    // swap coins + stars
    std::swap(m_coins, other->m_coins);
    std::swap(m_stars, other->m_stars);

    // swap ticksToMove
    int tempTTM = ticksToMove();
    setTicksToMove(other->ticksToMove());
    other->setTicksToMove(tempTTM);

    // swap walkWaitStatus
    int tempWWS = walkWaitStatus();
    setWalkWaitStatus(other->walkWaitStatus());
    other->setWalkWaitStatus(tempWWS);
}

void PlayerAvatar::walkMoveForward() {
    // adds functionality onto existing function
    Actor::walkMoveForward();
    if (ticksToMove() == 0)
        m_isLandableSquare = true;
}

//////////////////////////////////////////////////////////////////
// ActivatingObject
//////////////////////////////////////////////////////////////////

ActivatingObject::ActivatingObject(int imageID, int startX, int startY, int sDir, int depth, double size, bool impactable, int mDir, StudentWorld* world) : Actor(imageID, startX, startY, sDir, depth, size, impactable, mDir, world) { }

//////////////////////////////////////////////////////////////////
// ActivateOnPlayer
//////////////////////////////////////////////////////////////////
 
ActivateOnPlayer::ActivateOnPlayer(int imageID, int startX, int startY, int sDir, int depth, double size, bool impactable, int mDir, PlayerAvatar* P1, PlayerAvatar* P2, StudentWorld* world) : ActivatingObject(imageID, startX, startY, sDir, depth, size, impactable, mDir, world) {
    m_P1 = P1;
    m_P2 = P2;
    m_P1isActive = true;
    m_P2isActive = true;
}

bool ActivateOnPlayer::isOverlap(PlayerAvatar* affectedPlayer) {
    // is for direct landing
    if (affectedPlayer->getX() == getX() && affectedPlayer->getY() == getY())
        return true;
    return false;
}

//////////////////////////////////////////////////////////////////
// Baddie
//////////////////////////////////////////////////////////////////

Baddie::Baddie(int imageID, int startX, int startY, PlayerAvatar* p1, PlayerAvatar* p2, StudentWorld* world) : ActivateOnPlayer(imageID, startX, startY, 0, 0, 1, true, right, p1, p2, world) {
    m_pauseCounter = 180;
}

void Baddie::newRandomDir() {
    // randomly tries directions until one works
    while (0 == 0) {
        int newDirection = randInt(0,3) * 90;
        int futureXGrid, futureYGrid;

        if (newDirection == right) {
            getPositionInThisDirection(right, 16, futureXGrid, futureYGrid);
            getGridValues(futureXGrid, futureYGrid);
            if (world()->board().getContentsOf(futureXGrid, futureYGrid) != Board::empty) {
                setMovingDirection(right);
                setDirection(0);
                return;
            }
        }

        if (newDirection == up) {
            getPositionInThisDirection(up, 16, futureXGrid, futureYGrid);
            getGridValues(futureXGrid, futureYGrid);
            if (world()->board().getContentsOf(futureXGrid, futureYGrid) != Board::empty) {
                setMovingDirection(up);
                setDirection(0);
                return;
            }
        }

        if (newDirection == left) {
            getPositionInThisDirection(left, 16, futureXGrid, futureYGrid);
            getGridValues(futureXGrid, futureYGrid);
            if (world()->board().getContentsOf(futureXGrid, futureYGrid) != Board::empty) {
                setMovingDirection(left);
                setDirection(180);
                return;
            }
        }

        if (newDirection == down) {
            getPositionInThisDirection(down, 16, futureXGrid, futureYGrid);
            getGridValues(futureXGrid, futureYGrid);
            if (world()->board().getContentsOf(futureXGrid, futureYGrid) != Board::empty) {
                setMovingDirection(down);
                setDirection(0);
                return;
            }
        }
    }

}

void Baddie::pauseCounterAction() {
    setTicksToMove(randInt(1,10) * 16);
    newRandomDir();
    setWalkWaitStatus(walking);
}

void Baddie::walkingAction() {
    // boolean values show which spots are valid to walk on
    bool r, u, l, d; 
        if (isFork(r, u, l, d))
            newRandomDir();
        else
            walkNoForwardSquare();
        walkMoveForward();
        changeTicksToMove(-1);
        if (ticksToMove() <= 0) {
            setWalkWaitStatus(waiting);
            setPauseCounter(180);
        }
}

void Baddie::randomTeleport() {
    Actor::randomTeleport();
    setWalkWaitStatus(waiting);
    setDirection(0);
    setMovingDirection(right);
    setTicksToMove(180);
}

//////////////////////////////////////////////////////////////////
// Bowser
//////////////////////////////////////////////////////////////////

Bowser::Bowser(int startX, int startY, PlayerAvatar* p1, PlayerAvatar* p2, StudentWorld* world) : Baddie(IID_BOWSER, startX, startY, p1, p2, world) {
}

void Bowser::doSomething() {
    
    if(walkWaitStatus() == waiting) {
        if(!possibleLoseAllCoins(P1()) && isOverlap(P1()) && P1isActive() && P1()->isLandableBaddie()) {
            P1Activate();
            P1()->makeUnlandableBaddie();
        } else if (!P1isActive() && !isOverlap(P1()))
            P1Activate();
        if(!possibleLoseAllCoins(P2()) && isOverlap(P2()) && P2isActive() && P2()->isLandableBaddie()) {
            P1Activate();
            P2()->makeUnlandableBaddie();
        } else if (!P2isActive() && !isOverlap(P2()))
            P2Activate();

        changePauseCounter(-1);

        if (pauseCounter() == 0) {
            pauseCounterAction();
        }
    }
    
    if(walkWaitStatus() == walking) {
        walkingAction();
    }
}

void Bowser::walkingAction() {
    Baddie::walkingAction();
    if (ticksToMove() <= 0)
            world()->possibleDroppingSquareCreation(this);
}

bool Bowser::possibleLoseAllCoins(PlayerAvatar* affectedPlayer) {
    // random chance to have player lose all coins
    if(isOverlap(affectedPlayer) && affectedPlayer->walkWaitStatus() == waiting) {
        if (randInt(0,1) == 0) {
            world()->playSound(SOUND_BOWSER_ACTIVATE);
            affectedPlayer->resetCoins();
        }
        return true;
    }
    return false;
}

//////////////////////////////////////////////////////////////////
// Boo
//////////////////////////////////////////////////////////////////

Boo::Boo(int startX, int startY, PlayerAvatar* p1, PlayerAvatar* p2, StudentWorld* world) : Baddie(IID_BOO, startX, startY, p1, p2, world) {
    
}
                                           
void Boo::doSomething() {
    if(walkWaitStatus() == waiting) {
        if(isOverlap(P1()) && P1isActive() && P1()->isLandableBaddie()) {
            waitingAction(P1(), P2());
            P1()->makeUnlandableBaddie();
        } else if (!isOverlap(P1())) {
            P1Activate();
        }
        
        if(isOverlap(P2()) && P2isActive() && P2()->isLandableBaddie()) {
            waitingAction(P2(), P1());
            P2()->makeUnlandableBaddie();
        } else if (!isOverlap(P2())) {
            P2Activate();
        }

        changePauseCounter(-1);

        if(pauseCounter() == 0)
            pauseCounterAction();
    } else if (walkWaitStatus() == walking)
        walkingAction();
        
}

void Boo::waitingAction(PlayerAvatar* first, PlayerAvatar* second) {
    switch(randInt(0,1)){
        case 0:
            first->swapStars(second);
        case 1:
            first->swapCoins(second);
    }
    world()->playSound(SOUND_BOO_ACTIVATE);

}

//////////////////////////////////////////////////////////////////
// Vortex
//////////////////////////////////////////////////////////////////

Vortex::Vortex(int startX, int startY, int mDir, StudentWorld* world) : ActivatingObject(IID_VORTEX, startX, startY, 0, 0, 1, false, mDir, world) {
}

void Vortex::doSomething() {
    if(!isAlive())
        return;

    // kills Vortx if it goes off of the board
    int futureX, futureY;
    getPositionInThisDirection(movingDirection(), 2, futureX, futureY);
    moveTo(futureX, futureY);
    if(getX() > 256 || getY() > 256 || getX() < 0 || getY() < 0) 
        ruthlesslyObilerateAndDestroy();

    // if it comes into contact with a baddie, it kills it
    std::vector<Actor*> vortexLife;
    vortexLife.push_back(this);
    std::vector<Actor*>::iterator affectedBaddie = vortexLife.begin();
    world()->isOverlapping(this, affectedBaddie);
    if (!(*affectedBaddie)->isImpactable())
       return;
    ruthlesslyObilerateAndDestroy();
    world()->playSound(SOUND_HIT_BY_VORTEX);
    (*affectedBaddie)->randomTeleport();
}

//////////////////////////////////////////////////////////////////
// BankSquare
//////////////////////////////////////////////////////////////////

BankSquare::BankSquare(int startX, int startY, int* bank, PlayerAvatar* p1, PlayerAvatar* p2, StudentWorld* world) : ActivateOnPlayer(IID_BANK_SQUARE, startX, startY, right, 1, 1, false, -1, p1, p2, world) {
    m_bankPtr = bank;
}

void BankSquare::doSomething() {
    if(!isAlive())
        return;
    
    if (isOverlap(P1()) && P1isActive()) {
        if (P1()->walkWaitStatus() == PlayerAvatar::walking)
            takeCoins(P1());
        else
            giveCoins(P1());
        P1Deactivate();
    }

    if (isOverlap(P2()) && P1isActive()) {
        if (P2()->walkWaitStatus() == PlayerAvatar::walking)
            takeCoins(P2());
        else 
            giveCoins(P2());
        P2Deactivate();
    }

    if(!P1isActive() && !isOverlap(P1())) 
        P1Activate();
    if(!P2isActive() && !isOverlap(P2()))
        P2Activate();
}

void BankSquare::giveCoins(PlayerAvatar* player) {
    player->changeCoins(*m_bankPtr);
    *m_bankPtr = 0;
    world()->playSound(SOUND_DEPOSIT_BANK);
}

void BankSquare::takeCoins(PlayerAvatar* player) {
    if (player->coins() < 5) {
        player->resetCoins();
        *m_bankPtr += player->coins();
    } else {
        player->changeCoins(-5);
        *m_bankPtr += 5;
    }
    world()->playSound(SOUND_WITHDRAW_BANK);
}

//////////////////////////////////////////////////////////////////
// CoinSquare
//////////////////////////////////////////////////////////////////

CoinSquare::CoinSquare(int imageID, int startX, int startY, PlayerAvatar* p1, PlayerAvatar* p2, StudentWorld* world) : ActivateOnPlayer(imageID, startX, startY, right, 1, 1, false, -1, p1, p2, world) {
    // one class for both red and blue, as the ID is used to determine the coin change amount and the sound
    switch(imageID) {
        case IID_BLUE_COIN_SQUARE:
            m_coinChange = 3;
            m_coinSound = SOUND_GIVE_COIN;
            break;
        case IID_RED_COIN_SQUARE:
            m_coinChange = -3;
            m_coinSound = SOUND_TAKE_COIN;
    }
}

void CoinSquare::doSomething() {
    if(!isAlive())
        return;
    
    // ensures that only landed players get coins
    if (isOverlap(P1()) && P1isActive() && P1()->isLandableSquare()) {
        P1()->changeCoins(m_coinChange);
        world()->playSound(m_coinSound);
        P1Deactivate();
        P1()->makeUnlandableSquare();
    } else if(!P1isActive() && !isOverlap(P1()))
        P1Activate();

    if (isOverlap(P2()) && P2isActive() && P2()->isLandableSquare()) {
        P2()->changeCoins(m_coinChange);
        world()->playSound(m_coinSound);
        P2Deactivate();
        P2()->makeUnlandableSquare();
    } else if(!P2isActive() && !isOverlap(P2()))
        P2Activate();
    
}

//////////////////////////////////////////////////////////////////
// StarSquare
//////////////////////////////////////////////////////////////////

StarSquare::StarSquare(int startX, int startY, PlayerAvatar* p1, PlayerAvatar* p2, StudentWorld* world) : ActivateOnPlayer(IID_STAR_SQUARE, startX, startY, right, 1, 1, false, -1, p1, p2, world) {
}

void StarSquare::doSomething() {
    if(!isAlive())
        return;
    
    // allows for landing and passing activation
    if (isOverlap(P1()) && P1isActive()) {
        if (P1()->coins() >= 20) {
            P1()->changeCoins(-20);
            P1()->changeStars(1);
            world()->playSound(SOUND_GIVE_STAR);;
        }
        P1Deactivate();
    }

    if (isOverlap(P2()) && P2isActive()) {
        if (P2()->coins() >= 20) {
            P2()->changeCoins(-20);
            P2()->changeStars(1);
            world()->playSound(SOUND_GIVE_STAR);
        }
        P2Deactivate();
    }

    if(!P1isActive() && !isOverlap(P1())) 
        P1Activate();
    
    if(!P2isActive() && !isOverlap(P2()))
        P2Activate();
}

//////////////////////////////////////////////////////////////////
// EventSquare
//////////////////////////////////////////////////////////////////

EventSquare::EventSquare(int startX, int startY, PlayerAvatar* p1, PlayerAvatar* p2, StudentWorld* world) : ActivateOnPlayer(IID_EVENT_SQUARE, startX, startY, right, 1, 1, false, -1, p1, p2, world) {

}
 
void EventSquare::doSomething() {
    if(!isAlive())
        return;
    
    // upon landing, choses random action
    if (isOverlap(P1()) && P1isActive() && P1()->isLandableSquare()){
        switch(randInt(0,2)) {
            case 0:
                teleportPlayer(P1());
                break;
            case 1:
                swapPlayers(P2());
                break;
            case 2:
                giveVortex(P1());
                break;
            default:
                break;
        }
       P1()->makeUnlandableSquare();
    } else if(!isOverlap(P1())) 
        P1Activate();

    if (isOverlap(P2()) && P2isActive() && P2()->isLandableSquare()){
        switch(randInt(0, 2)) {
            case 0:
                teleportPlayer(P2());
                break;
            case 1:
                swapPlayers(P1());
                break;
            case 2:
                giveVortex(P2());
                break;
            default:
                break;
        }
       P2()->makeUnlandableSquare();
    } else if(!isOverlap(P2())) 
        P2Activate();
}

void EventSquare::teleportPlayer(PlayerAvatar* player) {
    world()->playSound(SOUND_PLAYER_TELEPORT);
    player->randomTeleport();
}

void EventSquare::swapPlayers(PlayerAvatar* player) {
    if(player == P1())
        player->swapTeleport(P2());
    else if(player == P2())
        player->swapTeleport(P1());
    if(P1isActive()) {
        P1Deactivate();
        P2Activate();
    } else if (P2isActive()) {
        P1Activate();
        P2Deactivate();
    }
    world()->playSound(SOUND_PLAYER_TELEPORT);
}

void EventSquare::giveVortex(PlayerAvatar* player) {
    if(!player->hasVortex()) {
        world()->playSound(SOUND_GIVE_VORTEX);
        player->recieveVortex();
    }
}

//////////////////////////////////////////////////////////////////
// DroppingSquare
//////////////////////////////////////////////////////////////////

DroppingSquare::DroppingSquare(int startX, int startY, PlayerAvatar* p1, PlayerAvatar* p2, StudentWorld* world) : ActivateOnPlayer(IID_DROPPING_SQUARE, startX, startY, right, 1, 1, false, -1, p1, p2, world) {
    
}

void DroppingSquare::doSomething() {
    if(!isAlive())
        return;

    if (isOverlap(P1()) && P1isActive() && P1()->isLandableSquare()) {
        switch(randInt(0,1)) {
            case 0:
                P1()->changeCoins(-10);
                break;
            case 1:
                P1()->changeStars(-1);
                break;
        }
        P1()->makeUnlandableSquare();
        P1Deactivate();
        world()->playSound(SOUND_DROPPING_SQUARE_ACTIVATE);
    } else if(!P1isActive() && !isOverlap(P1()))
        P1Activate();

    if (isOverlap(P2()) && P2isActive() && P2()->isLandableSquare()) {
        switch(randInt(0,1)) {
            case 0:
                P2()->changeCoins(-10);
                break;
            case 1:
                P1()->changeStars(-1);
                break;
        }
        P2()->makeUnlandableSquare();
        P2Deactivate();
        world()->playSound(SOUND_DROPPING_SQUARE_ACTIVATE);
    } else if(!P2isActive() && !isOverlap(P2()))
        P2Activate();

}

//////////////////////////////////////////////////////////////////
// DirectionSquare
//////////////////////////////////////////////////////////////////

DirectionalSquare::DirectionalSquare(int startX, int startY, int direction, PlayerAvatar* p1, PlayerAvatar* p2, StudentWorld* world) : ActivateOnPlayer(IID_DIR_SQUARE, startX, startY, direction, 1, 1, false, -1, p1, p2, world) {
    setDirection(direction);
}

void DirectionalSquare::doSomething() {
    if (isOverlap(P1()) && P1isActive()) {
        P1()->setMovingDirection(getDirection());
    }

    if (isOverlap(P2()) && P2isActive()) {
        P2()->setMovingDirection(getDirection());
    }
}

