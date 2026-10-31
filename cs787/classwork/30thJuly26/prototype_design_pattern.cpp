#include <bits/stdc++.h>
using namespace std;

class MazePrototypeFactory : public MazeFactory {
    public:
        MazePrototypeFactory(Maze* , Wall*, Room*, Door*);
        virtual Maze* makeMaze() const;
        virtual Wall* makeWall() const;
        virtual Room* makeRoom(int) const;
        virtual Door* makeDoor(Room*, Room*) const;
    private:
        Maze* _prototypeMaze;
        Wall* _prototypeWall;
        Room* _prototypeRoom;
        Door* _prototypeDoor;
};

MazePrototypeFactory::MazePrototypeFactory(Maze* m, Wall* w, Room* r, Door* d) {
    _prototypeMaze = m;
    _prototypeWall = w;
    _prototypeRoom = r;
    _prototypeDoor = d;
}

Wall* MazePrototypeFactory::makeWall() const {
    return _prototypeWall->clone();
}

Maze* MazePrototypeFactory::makeMaze() const {
    return _prototypeMaze->clone();
}

Room* MazePrototypeFactory::makeRoom(int n) const {
    Room* room = _prototypeRoom->clone();
    room->setRoomNumber(n);
    return room;
}

Door* MazePrototypeFactory::makeDoor(Room* r1, Room* r2) const {
    Door* door = _prototypeDoor->clone();
    door->initialize(r1, r2);
    return door;
}

MazeGame game;
MazePrototypeFactory simpleMazeFactory(new Maze, new Wall, new Room, new Door);

Maze* maze = game.createMaze(simpleMazeFactory);

MazePrototypeFactory bombedMazeFactory(new Maze, new BombedWall, new RoomWithABomb, new Door);

class Door : public MapSite {
    public:
        Door();
        Door(const Door&);

        virtual void intialize(Room*, Room*);
        virtual Door* clone() const;

        virtual void Enter();
        Room* OtherSideFrom(Room*);

    private:
        Room* _room1;
        Room* _room2;
};

Door::Door(const Door& d) {
    _room1 = d._room1;
    _room2 = d._room2;
}

void Door::intialize(Room* r1, Room* r2) {
    _room1 = r1;
    _room2 = r2;
}

Door* Door::clone() const {
    return new Door(*this);
}

class BombedWall : public Wall {
    public:
        BombedWall();
        BombedWall(const BombedWall&);

        virtual Wall* Clone() const;
        bool hasBomb();

    private:
        bool _bomb;
};

BombedWall::BombedWall (const BombedWall& other) : Wall(other) {
    _bomb = other._bomb;
}

BombedWall* BombedWall::Clone() const {
    return new BombedWall(*this);
}

