#pragma once
//------------------------------------------------------------------------------
/**
    Implement the pacman game loop.
    
    Kudos to:
    http://home.comcast.net/~jpittman2/pacman/pacmandossier.html
    And (especially for the cornering stuff!):
    http://pacman.shaunew.com/
*/
#include "Core/Types.h"
#include "canvas.h"

namespace Paclone {

class game {
public:
    /// static game elements
    enum TileType {
        Empty = 0,
        Dot,
        Wall,
        Door,
        
        NumTileTypes,
    };
    /// dynamic game elements
    enum ActorType {
        Blinky = 0,
        Pinky,
        Inky,
        Clyde,
        Pacman,
        
        NumActorTypes,
    };
    /// directions
    enum Direction {
        NoDirection = 0,
        Left,
        Right,
        Up,
        Down,
        
        NumDirections,
    };
    /// ghost states
    enum GhostState {
        None,
        Chase,
        Scatter,
        Frightened,
        Hollow,
        House,
        LeaveHouse,
        EnterHouse,
    };

    static const int NumLives = 3;
    static const int Width = 28;
    static const int Height = 36;
    static const int NumEnergizers = 4;
    static const int NumSprites = NumEnergizers + NumActorTypes + NumLives;
    
    /// constructor
    game();
    
    /// initiliaze (called once)
    void Init(canvas* canvas);
    /// update the game (called per frame)
    void Update(int tick, canvas* canvas, Direction input);
    /// cleanup (called once)
    void Cleanup();
    
private:
    static const int TileSize = 8;
    static const int TileMidX = 3;
    static const int TileMidY = 4;
    static const Oryol::int16 AntePortasPixelX = 13 * TileSize + TileMidX + TileSize/2;
    static const Oryol::int16 AntePortasPixelY = 14 * TileSize + TileMidY;
    static const Oryol::int32 NumScatterChasePhases = 8;

    int gameTick = 0;
    int dotCounter = 0;
    int noDotFrames = 0;    // number of frames since last dot eaten
    int score = 0;
    int hiscore = 0;
    int lives = NumLives;
    
    struct Actor {
        ActorType type;
        int spriteIndex = 0;
        Direction dir = NoDirection;
        Direction nextDir = NoDirection;
        Oryol::int32 moveTick = 0;
        Oryol::int16 pixelX = 0;
        Oryol::int16 pixelY = 0;
        Oryol::int16 tileX  = 0;
        Oryol::int16 tileY  = 0;
        Oryol::int16 distToMidX = 0;
        Oryol::int16 distToMidY = 0;
        Oryol::int16 targetX = 0;
        Oryol::int16 targetY = 0;
        Oryol::int16 homeBasePixelX = 0;
        Oryol::int16 homeBasePixelY = 0;
        GhostState state = Scatter;
        int frightenedTick = 0;
        int dotCounter = 0;
        int dotLimit = 0;
        bool forceLeaveHouse = false;
    } actors[NumActorTypes];
    
    struct Energizer {
        Oryol::int16 tileX = 0;
        Oryol::int16 tileY = 0;
        int spriteIndex = 0;
        bool active = false;
    } energizers[NumEnergizers];
    
    static bool nearEqual(Oryol::int16 i0, Oryol::int16 i1, Oryol::int16 maxDiff) {
        Oryol::int16 diff = i1 - i0;
        diff = diff >= 0 ? diff : -diff;
        o_assert(diff >= 0);
        return diff <= maxDiff;
    };
    static Oryol::int16 clampX(Oryol::int16 x) {
        if (x < 0) return 0;
        else if (x >= Width) return Width - 1;
        else return x;
    };
    static Oryol::int16 clampY(Oryol::int16 y) {
        if (y < 0) return 0;
        else if (y >= Height) return Height - 1;
        else return y;
    };
    void setTile(Oryol::int16 x, Oryol::int16 y, TileType type) {
        this->tileMap[clampY(y)][clampX(x)] = type;
    };
    TileType tile(Oryol::int16 x, Oryol::int16 y) const {
        return this->tileMap[clampY(y)][clampX(x)];
    };
    static void homePixelPos(Oryol::int16 inTileX, Oryol::int16 inTileY, Oryol::int16& outPixelX, Oryol::int16& outPixelY) {
        outPixelX = inTileX * TileSize + TileMidX + TileSize/2;
        outPixelY = inTileY * TileSize + TileMidY;
    };
    
    static Oryol::int32 targetDistSq(Oryol::int16 x, Oryol::int16 y, Oryol::int16 targetX, Oryol::int16 targetY) {
        Oryol::int16 dx = x - targetX;
        Oryol::int16 dy = y - targetY;
        return dx * dx + dy * dy;
    };
    static void commitPosition(Actor& actor) {
        actor.tileX = actor.pixelX / TileSize;
        actor.tileY = actor.pixelY / TileSize;
        actor.distToMidX = TileMidX - actor.pixelX % TileSize;
        actor.distToMidY = TileMidY - actor.pixelY % TileSize;
    };
    static GhostState lookupScatterChaseMode(int tick) {
        for (int i = 0; i < NumScatterChasePhases; i++) {
            if (tick < scatterChaseTable[i].frame) {
                return (GhostState) scatterChaseTable[i].state;
            }
        }
        // can't happen
        return Chase;
    };

    void setupTiles();
    void setupActors();
    void setupEnergizers();
    void updateActors(Direction input);
    void drawActors(canvas* canvas) const;
    void drawEnergizers(canvas* canvas) const;
    void drawChrome(canvas* canvas) const;
    bool isBlocked(const Actor& actor, Oryol::int16 tileX, Oryol::int16 tileY) const;
    Oryol::int32 actorSpeed(const Actor& actor) const;
    bool canMove(const Actor& actor, Direction dir, bool allowCornering) const;
    void computeMove(Actor& actor, Direction dir, bool allowCornering) const;
    void move(Actor& actor, bool allowCornering) const;
    void handleCollide(canvas* canvas);
    void eatDot(canvas* canvas);
    void eatEnergizer(Energizer& energizer);
    void killPacman();
    void killGhost(Actor& ghost);
    void chooseScatterTarget(Actor& ghost) const;
    void chooseChaseTarget(Actor& ghost) const;
    void chooseFrightenedTarget(Actor& ghost) const;
    void chooseHollowTarget(Actor& ghost) const;
    void chooseLeaveHouseTarget(Actor& ghost) const;
    void updateGhostDirection(Actor& ghost) const;
    void updateHouseDirection(Actor& ghost) const;
    void updateEnterHouseDirection(Actor& ghost) const;
    void updateLeaveHouseDirection(Actor& ghost) const;
    void updateGhostState(Actor& ghost);
    void updateGhostDotCounters();
    void addScore(int val);

    static TileType tileMap[Height][Width];
    static const Sheet::SpriteId defaultSpriteMap[NumActorTypes][NumDirections];
    static const Sheet::SpriteId hollowSpriteMap[NumDirections];
    static const Direction reverseDir[NumDirections];
    static const Oryol::int16 dirVec[NumDirections][2];
    static const Oryol::int16 homeTilePos[NumActorTypes][2];
    static const struct scatterChase {
        Oryol::int32 frame;
        GhostState state;
    } scatterChaseTable[NumScatterChasePhases];
};
    
} // namespace Paclone
