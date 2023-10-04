#include "StudentWorld.h"
#include "GameConstants.h"
#include <string>
#include <iostream>
#include <sstream>
#include <vector>
using namespace std;

GameWorld* createStudentWorld(string assetPath) {
    return new StudentWorld(assetPath);
}

StudentWorld::StudentWorld(string assetPath)
: GameWorld(assetPath) { }

StudentWorld::~StudentWorld() {
    cleanUp();
}

int StudentWorld::init() {
    // loads
    string board_file = assetPath() + "board0" + to_string(getBoardNumber()) + ".txt";
    if (m_Board.loadBoard(board_file)!= Board::load_success) {
        return GWSTATUS_BOARD_ERROR;
    }

    // iterate through all the the icons on the board, adds item to vector
    // looks for players first, as needs to pass player coords to the other objects
    for (int i = 0; i < BOARD_WIDTH; i++ )
        for (int j = 0; j < BOARD_HEIGHT; j++)
            switch(m_Board.getContentsOf(i, j)){
                case Board::player:
                   m_P1 = new PlayerAvatar(IID_PEACH, SPRITE_WIDTH * i, SPRITE_WIDTH * j, this);
                    m_P2 = new PlayerAvatar(IID_YOSHI, SPRITE_WIDTH * i, SPRITE_WIDTH * j, this);
                    m_actors.push_back(new CoinSquare(IID_BLUE_COIN_SQUARE, SPRITE_WIDTH * i, SPRITE_WIDTH * j, m_P1, m_P2, this));
                    break;
            }

    for (int i = 0; i < BOARD_WIDTH; i++ )
        for (int j = 0; j < BOARD_HEIGHT; j++)
            switch(m_Board.getContentsOf(i, j)){
                case Board::blue_coin_square:
                    m_actors.push_back(new CoinSquare(IID_BLUE_COIN_SQUARE, SPRITE_WIDTH * i, SPRITE_WIDTH * j, m_P1, m_P2,this));
                    break;
                case Board::red_coin_square:
                    m_actors.push_back(new CoinSquare(IID_RED_COIN_SQUARE, SPRITE_WIDTH * i, SPRITE_WIDTH * j, m_P1, m_P2, this));
                    break;
                case Board::up_dir_square:
                    m_actors.push_back(new DirectionalSquare( SPRITE_WIDTH * i, SPRITE_WIDTH * j, 90, m_P1, m_P2, this));
                    break;
                case Board::down_dir_square:
                    m_actors.push_back(new DirectionalSquare( SPRITE_WIDTH * i, SPRITE_WIDTH * j, 270, m_P1, m_P2, this));
                    break;
                case Board::left_dir_square:
                    m_actors.push_back(new DirectionalSquare( SPRITE_WIDTH * i, SPRITE_WIDTH * j, 180, m_P1, m_P2, this));
                    break;
                case Board::right_dir_square:
                    m_actors.push_back(new DirectionalSquare( SPRITE_WIDTH * i, SPRITE_WIDTH * j, 0, m_P1, m_P2, this));
                    break;
                case Board::event_square:
                    m_actors.push_back(new EventSquare(SPRITE_WIDTH * i, SPRITE_WIDTH * j, m_P1, m_P2, this));
                    break;
                case Board::bank_square:
                    m_actors.push_back(new BankSquare(SPRITE_WIDTH * i, SPRITE_WIDTH * j, &m_bank, m_P1, m_P2, this));
                    break;
                case Board::star_square:
                    m_actors.push_back(new StarSquare(SPRITE_WIDTH * i, SPRITE_WIDTH * j, m_P1, m_P2, this));
                    break;
                case Board::bowser:
                    m_actors.push_back(new Bowser(SPRITE_WIDTH * i, SPRITE_WIDTH * j, m_P1, m_P2, this));
                    m_actors.push_back(new CoinSquare(IID_BLUE_COIN_SQUARE, SPRITE_WIDTH * i, SPRITE_WIDTH * j, m_P1, m_P2, this));
                    break;
                case Board::boo:
                    m_actors.push_back(new Boo(SPRITE_WIDTH * i, SPRITE_WIDTH * j, m_P1, m_P2, this));
                    m_actors.push_back(new CoinSquare(IID_BLUE_COIN_SQUARE, SPRITE_WIDTH * i, SPRITE_WIDTH * j, m_P1, m_P2, this));
                    break;
                case Board::empty:
                    break;
            }
    
    startCountdownTimer(99);
    
    m_bank = 0;

    return GWSTATUS_CONTINUE_GAME;
}

int StudentWorld::move() {

    setGameStatText(updatedGameText());
    
    if (timeRemaining() <= 0) {
        playSound(SOUND_GAME_FINISHED);
        if(determineWinner() == 1) {
            setFinalScore(m_P1->stars(), m_P1->coins());
            return GWSTATUS_PEACH_WON;
        } else {
            setFinalScore(m_P2->stars(), m_P2->coins());
            return GWSTATUS_YOSHI_WON;
        }
    }
    

    std::vector<Actor*>::iterator it = m_actors.begin();
    while (it != m_actors.end()) {
        if ((*it)->isAlive()) {
            (*it)->doSomething();
            it++;
        } else {
            delete *it;
            it = m_actors.erase(it);
        }
    }

    if(m_P1->isAlive())
        m_P1->doSomething();
    if(m_P2->isAlive())
        m_P2->doSomething();
    
    // Update the Game Status Line
    //update display text // update the coins/stars stats text at screen top
    
    return GWSTATUS_CONTINUE_GAME;
}

void StudentWorld::cleanUp() {
    std::vector<Actor*>::iterator it = m_actors.begin();
    while (it != m_actors.end()) {
        delete *it;
        it = m_actors.erase(it);
    }
    
    // pointer checks ensure that cleanUp() and the destructor don't cause a double deleteion
    if (m_P1 != nullptr) {
        delete m_P1;
        m_P1 = nullptr;
    }if (m_P2 != nullptr) {
        delete m_P2;
        m_P2 = nullptr;
    }
        
    
}

string StudentWorld::updatedGameText() {
    ostringstream oss;
    oss << "P1 Roll: " << (m_P1->ticksToMove()+4)/8;
    oss << " Stars: " << m_P1->stars() << " $$ " << m_P1->coins();
    if (m_P1->hasVortex())
        oss << " VOR";
    oss << " | Time: " << timeRemaining();
    oss << " | Bank: " << coinsInBank();
    oss << " | P2 Roll: " << (m_P2->ticksToMove()+4)/8;
    oss << " Stars: " << m_P2->stars() << " $$ " << m_P2->coins();
    if (m_P2->hasVortex())
        oss << " VOR";
    
    return oss.str();
}

void StudentWorld::possibleDroppingSquareCreation(Bowser* bowser) {
    if(randInt(0, 3) != 1)
        return;
    playSound(SOUND_DROPPING_SQUARE_CREATED);
    // looks for square before dropping square to kill
    // determines squareness, as squares have a moving direction of -1 (they can't move)
    std::vector<Actor*>::iterator it = m_actors.begin();
    while (it != m_actors.end()) {
        if((*it)->getX() == bowser->getX() && (*it)->getY() == bowser->getY() && (*it)->movingDirection() == -1) {
            (*it)->ruthlesslyObilerateAndDestroy();
        }
        it++;
    }
    m_actors.push_back(new DroppingSquare(bowser->getX(), bowser->getY(), m_P1, m_P2, this));

}

void StudentWorld::createVortex(PlayerAvatar* player) {
    m_actors.push_back(new Vortex(player->getX(), player->getY(), player->movingDirection(), this));
}

bool StudentWorld::isOverlapping(Actor* vortex, std::vector<Actor*>::iterator& affected) {

    int a1MinX = vortex->getX();
    int a1MaxX = vortex->getX() + SPRITE_WIDTH-1;
    int a1MinY = vortex->getY();
    int a1MaxY = vortex->getY() + SPRITE_HEIGHT-1;

    for(std::vector<Actor*>::iterator it = m_actors.begin(); it != m_actors.end(); it++) {
        int a2MinX = (*it)->getX();
        int a2MaxX = (*it)->getX() + SPRITE_WIDTH-1;
        int a2MinY = (*it)->getY();
        int a2MaxY = (*it)->getY() + SPRITE_HEIGHT-1;

        // checks if any overlap betwen the actors
        if ((*it)->isImpactable() && a1MinX <= a2MaxX && a2MinX <= a1MaxX && a1MinY <= a2MaxY && a2MinY <= a1MaxY) {
            affected = it;
            return true;
        }
    }
    return false;
}

int StudentWorld::determineWinner() {
    int winner;
    if(m_P1->stars() != m_P2->stars()) {
        winner = (m_P1->stars() > m_P2->stars()) ? 1 : 2;
    } else if (m_P1->coins() != m_P2->coins()) {
        winner = (m_P1->coins() > m_P2->coins()) ? 1 : 2;
    } else {
        winner = (randInt(0,1) == 0) ? 1 : 2;
    }
    return winner;
}
