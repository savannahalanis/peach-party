#ifndef STUDENTWORLD_H_
#define STUDENTWORLD_H_

#include "GameWorld.h"
#include "Actor.h"
#include "Board.h"
#include <string>
#include <vector>

class StudentWorld : public GameWorld {
public:
    StudentWorld(std::string assetPath);
    ~StudentWorld();
    
    // mutators
    virtual int init();
    virtual int move();
    virtual void cleanUp();
    void possibleDroppingSquareCreation(Bowser* bowser);
    void createVortex(PlayerAvatar* vortex);
    
    // accessors
    Board& board() {return m_Board;}
    int coinsInBank() {return m_bank;}
    
    // extra
    std::string updatedGameText();
    bool isOverlapping(Actor* vortex, std::vector<Actor*>::iterator& affected);
    int determineWinner();
    
private:
    PlayerAvatar* m_P1;
    PlayerAvatar* m_P2;
    std::vector<Actor*> m_actors;
    Board m_Board;
    int m_bank;
};

#endif // STUDENTWORLD_H_
