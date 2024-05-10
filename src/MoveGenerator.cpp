#include "Engine.h"
#include "Move.h"
#include "BitBoardUtility.h"
#include "MoveGenerator.h"
#include <vector>
#include <cstdint>
#include <iostream>
// Testing of specific position: position startpos moves d5d6 h3g2 a2a3
//  These are all functionalities connected to move generation

uint_fast64_t knightMoves[64];
uint_fast64_t kingMoves[64];
uint_fast64_t pawnAttacks[2][2][64];

void MoveGenerator::GetPseudoLegalMoves(const Engine &engine, std::vector<Move> &pseudoLegalMoves)
{
    auto colorIndex = engine.board.whiteToMove ? 0 : 6;
    GetPseudoLegalPawnMoves(engine, pseudoLegalMoves);
    GetPseudoLegalKnightMoves(engine, pseudoLegalMoves);
    GetPseudoLegalKingMoves(engine, pseudoLegalMoves);
    GetPseudoLegalRookMoves(engine, engine.board.pieceBoards[3 + colorIndex] | engine.board.pieceBoards[4 + colorIndex], pseudoLegalMoves);   // rook + queen
    GetPseudoLegalBishopMoves(engine, engine.board.pieceBoards[2 + colorIndex] | engine.board.pieceBoards[4 + colorIndex], pseudoLegalMoves); // bishop + queen
}

void GetPseudoLegalPawnMoves(const Engine &engine, std::vector<Move> &pseudoLegalMoves)
{
    auto color = engine.board.whiteToMove;
    auto colorIndex = color ? 0 : 6;
    auto colorDirection = color ? -8 : 8;
    auto startRank = color ? 1 : 6;
    auto promotionRank = color ? 7 : 0;
    uint_fast64_t combinedColors = ~engine.board.colorBoards[color] & ~engine.board.colorBoards[!color];

    // Single Pawn pushes
    uint_fast64_t pushes = engine.board.pieceBoards[colorIndex]; // Get pawn bitboard of mover color
    if (color)
    { // if white, push up
        pushes = pushes >> 8;
    }
    else
    { // if black, push down
        pushes = pushes << 8;
    }
    pushes &= combinedColors; // remove pawns which landed on occupied squares
    auto to = BitScanForwards(pushes) - 1;
    while (to >= 0)
    {
        Move move(to - colorDirection, to);
        if (CheckBit(rankMasks[promotionRank], to))
        {
            for (auto i = 2; i < 6; i++)
            {
                move.convertTo = (PieceType)i;
                move.promotion = true;
                pseudoLegalMoves.push_back(move);
            }
        }
        else
        {
            pseudoLegalMoves.push_back(move);
        }
        UnsetBit(pushes, to);
        to = BitScanForwards(pushes) - 1;
    }

    // Double pawn pushes - push all pawns twice (check for blockers inbetween)
    pushes = engine.board.pieceBoards[colorIndex] & rankMasks[startRank];
    if (color)
    { // if white push up
        pushes = ((pushes >> 8) & combinedColors) >> 8;
    }
    else
    { // if black push down
        pushes = ((pushes << 8) & combinedColors) << 8;
    }
    pushes &= combinedColors; // remove pawns which landed on occupied squares
    to = BitScanForwards(pushes) - 1;
    while (to >= 0)
    {
        pseudoLegalMoves.push_back(Move(to - colorDirection * 2, to));
        UnsetBit(pushes, to);
        to = BitScanForwards(pushes) - 1;
    }

    // // Pawn Captures
    uint_fast64_t pawnBoard = engine.board.pieceBoards[colorIndex];
    auto from = BitScanForwards(pawnBoard) - 1;
    while (from >= 0)
    {
        for (auto j = 0; j < 2; j++)
        {
            uint64_t captures = pawnAttacks[!color][j][from] & (engine.board.colorBoards[color] | engine.board.ghostBoard);
            to = BitScanForwards(captures) - 1;
            if (to >= 0)
            {
                Move move(from, to);
                if (CheckBit(rankMasks[promotionRank], to))
                {
                    for (auto i = 2; i < 6; i++)
                    {
                        move.convertTo = (PieceType)i;
                        move.promotion = true;
                        pseudoLegalMoves.push_back(move);
                    }
                }
                else
                {
                    pseudoLegalMoves.push_back(move);
                }
            }
        }
        UnsetBit(pawnBoard, from);
        from = BitScanForwards(pawnBoard) - 1;
    }
}

void GetPseudoLegalKnightMoves(const Engine &engine, std::vector<Move> &pseudoLegalMoves)
{
    auto color = engine.board.whiteToMove;
    auto colorIndex = color ? 0 : 6;
    uint_fast64_t thisBoard = ~engine.board.colorBoards[!color]; //  ~friendly blockers
    uint_fast64_t knights = engine.board.pieceBoards[1 + colorIndex];
    uint_fast64_t attacks = ZERO;
    auto from = BitScanForwards(knights) - 1;
    while (from >= 0)
    {
        attacks = knightMoves[from] & thisBoard;
        auto to = BitScanForwards(attacks) - 1;
        while (to >= 0)
        {
            pseudoLegalMoves.push_back(Move(from, to));
            UnsetBit(attacks, to);
            to = BitScanForwards(attacks) - 1;
        }
        UnsetBit(knights, from);
        from = BitScanForwards(knights) - 1;
    }
}

void GetPseudoLegalKingMoves(const Engine &engine, std::vector<Move> &pseudoLegalMoves)
{
    auto color = engine.board.whiteToMove;
    int kingIndex = engine.board.kingIndices[!color];
    auto castleColorIndex = color ? 0 : 2;                                                    // to obtain colored castling rules
    uint_fast64_t combinedColors = engine.board.colorBoards[0] | engine.board.colorBoards[1]; // | engine.board.attackBoard; // Bitboard with both friendly and unfriendly blockers and attacked squares for castling

    // normal king moves
    uint_fast64_t attacks = kingMoves[kingIndex] & ~engine.board.colorBoards[!color];
    auto to = BitScanForwards(attacks) - 1;
    while (to >= 0)
    {
        pseudoLegalMoves.push_back(Move(kingIndex, to));
        UnsetBit(attacks, to);
        to = BitScanForwards(attacks) - 1;
    }

    // fucking castling
    uint_fast64_t kingSideMask = castleMasks[!color][0];
    uint_fast64_t queenSideMask = castleMasks[!color][1];
    uint_fast64_t kingsideOccupation = kingSideMask & combinedColors; // castling masks with blocked indices removed
    uint_fast64_t queenSideOccupation = queenSideMask & combinedColors;
    auto kingSideCastleTarget = color ? 62 : 6;
    auto queenSideCastleTarget = color ? 58 : 2;
    if (kingsideOccupation == 0 && engine.board.castlingRights[castleColorIndex])
    {
        auto index = BitScanForwards(kingSideMask) - 1;
        auto canCastle = true;
        while (index >= 0)
        {
            if (MoveGenerator::IsSquareAttacked(engine, index, !color))
            {
                canCastle = false;
                break;
            }
            UnsetBit(kingSideMask, index);
            index = BitScanForwards(kingSideMask) - 1;
        }
        if (canCastle && !MoveGenerator::IsSquareAttacked(engine, kingIndex, !color))
        {
            pseudoLegalMoves.push_back(Move(kingIndex, kingSideCastleTarget));
        }
    }
    if (queenSideOccupation == 0 && engine.board.castlingRights[castleColorIndex + 1])
    {
        auto index = BitScanForwards(queenSideMask) - 1;
        auto canCastle = true;
        while (index >= 0)
        {
            if (index != queenSideCastleTarget - 1)
            {
                if (MoveGenerator::IsSquareAttacked(engine, index, !color))
                {
                    canCastle = false;
                    break;
                }
            }
            UnsetBit(queenSideMask, index);
            index = BitScanForwards(queenSideMask) - 1;
        }
        if (canCastle && !MoveGenerator::IsSquareAttacked(engine, kingIndex, !color))
        {
            pseudoLegalMoves.push_back(Move(kingIndex, queenSideCastleTarget));
        }
    }
}

void GetPseudoLegalRookMoves(const Engine &engine, const uint_fast64_t &rookPieceBoard, std::vector<Move> &pseudoLegalMoves)
{
    auto color = engine.board.whiteToMove;
    uint_fast64_t thisBoard = engine.board.colorBoards[!color];
    uint_fast64_t otherBoard = engine.board.colorBoards[color];
    uint_fast64_t rookBoard = rookPieceBoard;

    auto from = BitScanForwards(rookBoard) - 1;
    while (from >= 0)
    {
        auto to = from + 8;
        while (to < 64)
        {
            if (CheckBit(thisBoard, to))
            {
                break;
            }
            pseudoLegalMoves.push_back(Move(from, to));
            if (CheckBit(otherBoard, to))
            {
                break;
            }
            to += 8;
        }
        to = from - 8;
        while (to >= 0)
        {
            if (CheckBit(thisBoard, to))
            {
                break;
            }
            pseudoLegalMoves.push_back(Move(from, to));
            if (CheckBit(otherBoard, to))
            {
                break;
            }
            to -= 8;
        }
        to = from + 1;
        while (to < 64)
        {
            if (CheckBit(thisBoard, to) || CheckBit(fileMasks[0], to))
            {
                break;
            }
            pseudoLegalMoves.push_back(Move(from, to));
            if (CheckBit(otherBoard, to))
            {
                break;
            }
            to++;
        }
        to = from - 1;
        while (to >= 0)
        {
            if (CheckBit(thisBoard, to) || CheckBit(fileMasks[7], to))
            {
                break;
            }
            pseudoLegalMoves.push_back(Move(from, to));
            if (CheckBit(otherBoard, to))
            {
                break;
            }
            to--;
        }
        UnsetBit(rookBoard, from);
        from = BitScanForwards(rookBoard) - 1;
    }
}

void GetPseudoLegalBishopMoves(const Engine &engine, const uint_fast64_t &bishopPieceBoard, std::vector<Move> &pseudoLegalMoves)
{
    auto color = engine.board.whiteToMove;
    uint_fast64_t thisBoard = engine.board.colorBoards[!color];
    uint_fast64_t otherBoard = engine.board.colorBoards[color];
    uint_fast64_t bishopBoard = bishopPieceBoard;

    auto from = BitScanForwards(bishopBoard) - 1;
    while (from >= 0)
    {
        auto to = from + 9;
        while (to < 64)
        {
            if (CheckBit(fileMasks[0], to) || CheckBit(thisBoard, to))
            {
                break;
            }
            pseudoLegalMoves.push_back(Move(from,to));
            if (CheckBit(otherBoard, to))
            {
                break;
            }
            to += 9;
        }
        to = from + 7;
        while (to < 64)
        {
            if (CheckBit(fileMasks[7], to) || CheckBit(thisBoard, to))
            {
                break;
            }
            pseudoLegalMoves.push_back(Move(from,to));
            if (CheckBit(otherBoard, to))
            {
                break;
            }
            to += 7;
        }
        to = from - 9;
        while (to >= 0)
        {
            if (CheckBit(fileMasks[7], to) || CheckBit(thisBoard, to))
            {
                break;
            }
            pseudoLegalMoves.push_back(Move(from,to));
            if (CheckBit(otherBoard, to))
            {
                break;
            }
            to -= 9;
        }
        to = from - 7;
        while (to >= 0)
        {
            if (CheckBit(fileMasks[0], to) || CheckBit(thisBoard, to))
            {
                break;
            }
            pseudoLegalMoves.push_back(Move(from,to));
            if (CheckBit(otherBoard, to))
            {
                break;
            }
            to -= 7;
        }
        UnsetBit(bishopBoard,from);
        from = BitScanForwards(bishopBoard)-1;
    }
}

void MoveGenerator::GenerateAttacks(const Engine &engine, const bool &color, uint_fast64_t &attackBoard)
{
    auto colorIndex = color ? 0 : 6;
    GeneratePawnAttacks(engine, color, attackBoard);
    GenerateKnightAttacks(engine, color, attackBoard);
    GenerateKingAttacks(engine, color, attackBoard);
    GenerateRookAttacks(engine, color, engine.board.pieceBoards[3 + colorIndex] | engine.board.pieceBoards[4 + colorIndex], attackBoard);
    GenerateBishopAttacks(engine, color, engine.board.pieceBoards[2 + colorIndex] | engine.board.pieceBoards[4 + colorIndex], attackBoard);
}

void GeneratePawnAttacks(const Engine &engine, const bool &color, uint_fast64_t &attackBoard)
{
    // basically move generation code but only with captures
    auto colorIndex = color ? 0 : 6;
    auto colorDirection = color ? -1 : 1;
    auto startRank = color ? 6 : 1;
    uint_fast64_t captures;
    if (color) // Shift pawn board diagonal up or down depending on color
    {
        captures = engine.board.pieceBoards[colorIndex] >> 7 | engine.board.pieceBoards[colorIndex] >> 9;
    }
    else
    {
        captures = engine.board.pieceBoards[colorIndex] << 7 | engine.board.pieceBoards[colorIndex] << 9;
    }
    // No anding with occupations, since we want attacks, meaning hypothetical captures
    for (auto i = 1; i < 7; i++) // pawns at the edge need to be handled seperately to prevent telepawns
    {
        auto j = startRank + colorDirection; // Again only consider captures between start rank and one short of promotion
        for (auto k = 0; k < 6; k++)         // Instead of stopping before the promotion rank, we can go up to there since promotions do not have to be considered
        {
            if (CheckBit(captures, i, j))
            {
                if (CheckBit(engine.board.pieceBoards[colorIndex], i - 1, j - colorDirection) || CheckBit(engine.board.pieceBoards[colorIndex], i + 1, j - colorDirection)) // Captures are diagonal moves
                {
                    SetBit(attackBoard, i, j); // If the capture is valed, set the corresponding attackbit
                }
            }
            j += colorDirection;
        }
    }
    auto j = startRank + colorDirection; // Treatment of the border pawns
    for (auto k = 0; k < 6; k++)
    {
        if (CheckBit(captures, 0, j))
        {
            if (CheckBit(engine.board.pieceBoards[colorIndex], 1, j - colorDirection))
            {
                SetBit(attackBoard, 0, j);
            }
        }
        j += colorDirection;
    }
    j = startRank + colorDirection;
    for (auto k = 0; k < 6; k++)
    {
        if (CheckBit(captures, 7, j))
        {
            if (CheckBit(engine.board.pieceBoards[colorIndex], 6, j - colorDirection))
            {
                SetBit(attackBoard, 7, j);
            }
        }
        j += colorDirection;
    }
}

void GenerateKnightAttacks(const Engine &engine, const bool &color, uint_fast64_t &attackBoard)
{
    // This is basically the same as the move generation function but with setting attackboard bits
    auto colorIndex = color ? 0 : 6;
    for (auto i = 0; i < 8; i++) // loop over all squares to find knights
    {
        for (auto j = 0; j < 8; j++)
        {
            if (CheckBit(engine.board.pieceBoards[1 + colorIndex], i, j))
            {
                for (auto di = -2; di < 3; di += 4) // Loop over all knight increments
                {
                    for (auto dj = -1; dj < 2; dj += 2)
                    {
                        auto ii = i + di;
                        auto jj = j + dj;
                        if (ii >= 0 && ii < 8 && jj >= 0 && jj < 8) // Check for knight moves with 2x y
                        {
                            SetBit(attackBoard, ii, jj);
                        }
                        ii = i + dj;
                        jj = j + di;
                        if (ii >= 0 && ii < 8 && jj >= 0 && jj < 8) // Check for knight moves with x 2y
                        {
                            SetBit(attackBoard, ii, jj);
                        }
                    }
                }
            }
        }
    }
}

void GenerateKingAttacks(const Engine &engine, const bool &color, uint_fast64_t &attackBoard)
{
    // also the same as move generation, but without castling
    Coord kingCoord = engine.board.kingCoords[!color];

    // normal king moves
    uint_fast64_t moves = ZERO;
    SetBit(moves, kingCoord.x, kingCoord.y);
    moves = moves >> 8 | moves >> 9 | moves >> 1 | moves << 7 | moves << 8 | moves << 9 | moves << 1 | moves >> 7; // all king increments
    for (auto i = 0; i < 8; i++)
    {
        for (auto j = 0; j < 8; j++)
        {
            if (CheckBit(moves, i, j) && (std::abs(i - kingCoord.x) <= 1 && std::abs(j - kingCoord.y) <= 1))
            { // moves are set bits of moves which lie in range of the king
                SetBit(attackBoard, i, j);
            }
        }
    }
}

void GenerateRookAttacks(const Engine &engine, const bool &color, const uint_fast64_t &rookPieceBoard, uint_fast64_t &attackBoard)
{
    // Same as move generation
    uint_fast64_t bothBoards = engine.board.colorBoards[!color] | engine.board.colorBoards[color];
    UnsetBit(bothBoards, engine.board.kingCoords[color].x, engine.board.kingCoords[color].y); // Remove enemy king from blockers so slider can see behind king
    for (auto i = 0; i < 8; i++)                                                              // Loop to find square of rook piece
    {
        for (auto j = 0; j < 8; j++)
        {
            if (CheckBit(rookPieceBoard, i, j))
            {
                for (auto di = 1; di < 8 - i; di++) // for each rook piece loop over all squares to the edges of the board
                {
                    SetBit(attackBoard, i + di, j);
                    if (CheckBit(bothBoards, i + di, j)) // stop if landed on piece
                    {
                        break;
                    }
                }
                for (auto di = -1; di >= -i; di--)
                {
                    SetBit(attackBoard, i + di, j);
                    if (CheckBit(bothBoards, i + di, j))
                    {
                        break;
                    }
                }
                for (auto dj = 1; dj < 8 - j; dj++)
                {
                    SetBit(attackBoard, i, j + dj);
                    if (CheckBit(bothBoards, i, j + dj))
                    {
                        break;
                    }
                }
                for (auto dj = -1; dj >= -j; dj--)
                {
                    SetBit(attackBoard, i, j + dj);
                    if (CheckBit(bothBoards, i, j + dj))
                    {
                        break;
                    }
                }
            }
        }
    }
}
void GenerateBishopAttacks(const Engine &engine, const bool &color, const uint_fast64_t &bishopPieceBoard, uint_fast64_t &attackBoard)
{
    // Same as move generation
    uint_fast64_t bothBoards = engine.board.colorBoards[!color] | engine.board.colorBoards[color];
    UnsetBit(bothBoards, engine.board.kingCoords[color].x, engine.board.kingCoords[color].y); // Remove enemy king from blockers so slider can see behind king

    for (auto i = 0; i < 8; i++) // loop to find bishop piece
    {
        for (auto j = 0; j < 8; j++)
        {
            if (CheckBit(bishopPieceBoard, i, j))
            {
                for (auto d = 1; d < std::min(8 - i, 8 - j); d++) // loop over all directions till the end of the board
                {
                    SetBit(attackBoard, i + d, j + d);
                    if (CheckBit(bothBoards, i + d, j + d)) // stop if captured enemy piece
                    {
                        break;
                    }
                }
                for (auto d = -1; d >= std::max(-i, -j); d--)
                {
                    SetBit(attackBoard, i + d, j + d);
                    if (CheckBit(bothBoards, i + d, j + d))
                    {
                        break;
                    }
                }
                for (auto d = 1; d < std::min(8 - i, j + 1); d++)
                {
                    SetBit(attackBoard, i + d, j - d);
                    if (CheckBit(bothBoards, i + d, j - d))
                    {
                        break;
                    }
                }
                for (auto d = 1; d < std::min(i + 1, 8 - j); d++)
                {
                    SetBit(attackBoard, i - d, j + d);
                    if (CheckBit(bothBoards, i - d, j + d))
                    {
                        break;
                    }
                }
            }
        }
    }
}

void MoveGenerator::GetLegalMoves(Engine &engine, std::vector<Move> &legalmoves)
{
    auto color = engine.board.whiteToMove;
    auto colorIndex = color ? 0 : 6;
    std::vector<Move> pseudoLegalMoves;
    GetPseudoLegalPawnMoves(engine, pseudoLegalMoves);
    GetPseudoLegalKnightMoves(engine, pseudoLegalMoves);
    GetPseudoLegalRookMoves(engine, engine.board.pieceBoards[3 + colorIndex] | engine.board.pieceBoards[4 + colorIndex], pseudoLegalMoves);
    GetPseudoLegalBishopMoves(engine, engine.board.pieceBoards[2 + colorIndex] | engine.board.pieceBoards[4 + colorIndex], pseudoLegalMoves);
    GetPseudoLegalKingMoves(engine, pseudoLegalMoves);
    for (const auto &move : pseudoLegalMoves)
    {
        // std::cout << "Checking out: " << Move2Str(move) << "\n";
        engine.MakeSimpleMove(move);
        if (!MoveGenerator::IsSquareAttacked(engine, engine.board.kingIndices[!color], !color))
        {
            legalmoves.push_back(move);
        }
        engine.UndoLastMove();
    }
}

bool MoveGenerator::IsSquareAttacked(const Engine &engine, const int &index, const bool &attackingColor)
{
    auto colorIndex = attackingColor ? 0 : 6;
    // Bitboards containing slider pieces
    uint_fast64_t horAndVertSliderBoard = engine.board.pieceBoards[3 + colorIndex] | engine.board.pieceBoards[4 + colorIndex];
    uint_fast64_t diagonalSliderBoard = engine.board.pieceBoards[2 + colorIndex] | engine.board.pieceBoards[4 + colorIndex];
    // Bitboards containing all pieces which cant check by sliding
    uint_fast64_t horAndVertBlockerBoard = (engine.board.colorBoards[!attackingColor] & ~engine.board.pieceBoards[3 + colorIndex] & ~engine.board.pieceBoards[4 + colorIndex]) | engine.board.colorBoards[attackingColor];
    uint_fast64_t diagonalBlockerBoard = (engine.board.colorBoards[!attackingColor] & ~engine.board.pieceBoards[2 + colorIndex] & ~engine.board.pieceBoards[4 + colorIndex]) | engine.board.colorBoards[attackingColor];

    // Knights: can the piece on index attack a attackingcolor knight like a knight
    uint_fast64_t attacks = knightMoves[index] & engine.board.pieceBoards[1 + colorIndex];
    if (attacks > 0)
    {
        return true;
    }

    // King: is the piece on index attacked by the attackingcolor king
    int attackingKingIndex = engine.board.kingIndices[!attackingColor];
    attacks = kingMoves[attackingKingIndex];
    if (CheckBit(attacks, index))
    {
        return true;
    }

    // Pawns: can the piece on index attack an attackingcolor pawn like a pawn
    attacks = (pawnAttacks[attackingColor][0][index] | pawnAttacks[attackingColor][1][index]) & engine.board.pieceBoards[colorIndex];
    if (attacks > 0)
    {
        return true;
    }

    // Diagonal sliders: can the piece on index attack a attackingcolor diagonal slider like a diagonal slider
    auto increment = index + 9;
    while (increment < 64)
    {
        if (CheckBit(fileMasks[0], increment) || CheckBit(diagonalBlockerBoard, increment))
        {
            break;
        }
        if (CheckBit(diagonalSliderBoard, increment))
        {
            return true;
        }
        increment += 9;
    }
    increment = index + 7;
    while (increment < 64)
    {
        if (CheckBit(fileMasks[7], increment) || CheckBit(diagonalBlockerBoard, increment))
        {
            break;
        }
        if (CheckBit(diagonalSliderBoard, increment))
        {
            return true;
        }
        increment += 7;
    }
    increment = index - 9;
    while (increment >= 0)
    {
        if (CheckBit(fileMasks[7], increment) || CheckBit(diagonalBlockerBoard, increment))
        {
            break;
        }
        if (CheckBit(diagonalSliderBoard, increment))
        {
            return true;
        }
        increment -= 9;
    }
    increment = index - 7;
    while (increment >= 0)
    {
        if (CheckBit(fileMasks[0], increment) || CheckBit(diagonalBlockerBoard, increment))
        {
            break;
        }
        if (CheckBit(diagonalSliderBoard, increment))
        {
            return true;
        }
        increment -= 7;
    }

    // Horizontal and vertical sliders: can the piece on index attack a attacking color hor and vert slider like a hor and vert slider
    increment = index + 8;
    while (increment < 64)
    {
        if (CheckBit(horAndVertBlockerBoard, increment))
        {
            break;
        }
        if (CheckBit(horAndVertSliderBoard, increment))
        {
            return true;
        }
        increment += 8;
    }
    increment = index - 8;
    while (increment >= 0)
    {
        if (CheckBit(horAndVertBlockerBoard, increment))
        {
            break;
        }
        if (CheckBit(horAndVertSliderBoard, increment))
        {
            return true;
        }
        increment -= 8;
    }
    increment = index + 1;
    while (increment < 64)
    {
        if (CheckBit(horAndVertBlockerBoard, increment) || CheckBit(fileMasks[0], increment))
        {
            break;
        }
        if (CheckBit(horAndVertSliderBoard, increment))
        {
            return true;
        }
        increment++;
    }
    increment = index - 1;
    while (increment >= 0)
    {
        if (CheckBit(horAndVertBlockerBoard, increment) || CheckBit(fileMasks[7], increment))
        {
            break;
        }
        if (CheckBit(horAndVertSliderBoard, increment))
        {
            return true;
        }
        increment--;
    }

    return false;
}

bool MoveGenerator::IsSquareAttacked(const Engine &engine, const Coord &square, const bool &attackingColor)
{
    return IsSquareAttacked(engine, square.y * 8 + square.x, attackingColor);
}

void MoveGenerator::PreComputeKnightMoves()
{
    uint_fast64_t localMoves = 0;
    for (auto i = 0; i < 64; i++)
    {
        localMoves = ONE << i;
        localMoves = (localMoves & ~fileMasks[7]) << 17 | (localMoves & ~(fileMasks[6] | fileMasks[7])) << 10 | (localMoves & ~(fileMasks[6] | fileMasks[7])) >> 6 | (localMoves & ~fileMasks[7]) >> 15 | (localMoves & ~fileMasks[0]) >> 17 | (localMoves & ~(fileMasks[0] | fileMasks[1])) >> 10 | (localMoves & ~(fileMasks[0] | fileMasks[1])) << 6 | (localMoves & ~fileMasks[0]) << 15;
        knightMoves[i] = localMoves;
    }
}

void MoveGenerator::PreComputeKingMoves()
{
    uint_fast64_t localMoves = 0;
    for (auto i = 0; i < 64; i++)
    {
        localMoves = ONE << i;
        localMoves = localMoves << 8 | (localMoves & ~fileMasks[7]) << 9 | (localMoves & ~fileMasks[7]) << 1 | (localMoves & ~fileMasks[7]) >> 7 | localMoves >> 8 | (localMoves & ~fileMasks[0]) >> 9 | (localMoves & ~fileMasks[0]) >> 1 | (localMoves & ~fileMasks[0]) << 7;
        kingMoves[i] = localMoves;
    }
}

void MoveGenerator::PreComputePawnAttacks()
{
    uint_fast64_t localMoves = 0;
    for (auto i = 0; i < 64; i++)
    {
        localMoves = ONE << i;
        // white left
        pawnAttacks[0][0][i] = (localMoves & ~fileMasks[0]) >> 9;
        // white right
        pawnAttacks[0][1][i] = (localMoves & ~fileMasks[7]) >> 7;
        // black left
        pawnAttacks[1][0][i] = (localMoves & ~fileMasks[0]) << 7;
        // black right
        pawnAttacks[1][1][i] = (localMoves & ~fileMasks[7]) << 9;
    }
}