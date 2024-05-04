#include "Engine.h"
#include "Move.h"
#include "BitBoardUtility.h"
#include "MoveGenerator.h"
#include <vector>
#include <cstdint>

// These are all functionalities connected to move generation

void MoveGenerator::GetPseudoLegalMoves(const Engine &engine, std::vector<Move> &pseudoLegalMoves)
{
    auto colorIndex = engine.board.whiteToMove ? 0 : 6;
    GetPseudoLegalPawnMoves(engine, pseudoLegalMoves);
    GetPseudoLegalKnightMoves(engine, pseudoLegalMoves);
    GetPseudoLegalKingMoves(engine, pseudoLegalMoves);
    GetPseudoLegalRookMoves(engine, engine.board.pieceBoards[3 + colorIndex] | engine.board.pieceBoards[4 + colorIndex], pseudoLegalMoves); //rook + queen
    GetPseudoLegalBishopMoves(engine, engine.board.pieceBoards[2 + colorIndex] | engine.board.pieceBoards[4 + colorIndex], pseudoLegalMoves);   //bishop + queen
}

void GetPseudoLegalPawnMoves(const Engine &engine, std::vector<Move> &pseudoLegalMoves)
{
    auto color = engine.board.whiteToMove;
    auto colorIndex = color ? 0 : 6;
    auto colorDirection = color ? -1 : 1;
    auto startRank = color ? 6 : 1;
    auto promotionRank = color ? 0 : 7;

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
    pushes &= (~engine.board.colorBoards[color] | ~engine.board.colorBoards[!color]); // remove pawns which landed on occupied squares
    for (auto i = 0; i < 8; i++)
    {
        auto j = startRank + 1; // we only care for pawn moves between start rank and one short of promotion rank
        for (auto k = 0; k < 5; k++)
        {
            if (CheckBit(pushes, i, j))
            {
                pseudoLegalMoves.push_back(Move(i, j - colorDirection, i, j)); // Push back all the remaining valid moves
            }
            j += colorDirection;
        }
    }

    // Double pawn pushes - push all remaining pawns one more time
    uint_fast64_t doublePushes;
    if (color)
    { // if white push up
        doublePushes = pushes >> 8;
    }
    else
    { // if black push down
        doublePushes = pushes << 8;
    }
    doublePushes &= (~engine.board.colorBoards[color] | ~engine.board.colorBoards[!color]); // remove pawns which landed on occupied squares

    auto fourthRank = color ? 4 : 3; // valid double push target squares
    for (auto i = 0; i < 8; i++)
    {
        if (CheckBit(doublePushes, i, fourthRank))
        {
            pseudoLegalMoves.push_back(Move(i, fourthRank - 2 * colorDirection, i, fourthRank)); // only push back moves which landed on valid double push squares
        }
    }

    // Pawn Captures
    uint_fast64_t captures;
    if (color) // Shift pawn board diagonal up or down depending on color
    {
        captures = engine.board.pieceBoards[colorIndex] >> 7 | engine.board.pieceBoards[colorIndex] >> 9;
    }
    else
    {
        captures = engine.board.pieceBoards[colorIndex] << 7 | engine.board.pieceBoards[colorIndex] << 9;
    }
    captures = captures & (engine.board.colorBoards[color] | engine.board.ghostBoard); // check for capturable pieces there
    for (auto i = 1; i < 7; i++)                                                       // pawns at the edge need to be handled seperately to prevent telepawns
    {
        auto j = startRank + colorDirection; // Again only consider captures between start rank and one short of promotion
        for (auto k = 0; k < 5; k++)
        {
            if (CheckBit(captures, i, j))
            {
                if (CheckBit(engine.board.pieceBoards[colorIndex], i - 1, j - colorDirection)) // Captures are diagonal moves
                {
                    pseudoLegalMoves.push_back(Move(i - 1, j - colorDirection, i, j));
                }
                if (CheckBit(engine.board.pieceBoards[colorIndex], i + 1, j - colorDirection)) // Because two pawns might capture on the same square, we need to check from where the captures came
                {
                    pseudoLegalMoves.push_back(Move(i + 1, j - colorDirection, i, j));
                }
            }
            j += colorDirection;
        }
    }
    auto j = startRank + colorDirection; // Treatment of the border pawns
    for (auto k = 0; k < 5; k++)
    {
        if (CheckBit(captures, 0, j))
        {
            if (CheckBit(engine.board.pieceBoards[colorIndex], 1, j - colorDirection))
            {
                pseudoLegalMoves.push_back(Move(1, j - colorDirection, 0, j));
            }
        }
        j += colorDirection;
    }
    j = startRank + colorDirection;
    for (auto k = 0; k < 5; k++)
    {
        if (CheckBit(captures, 7, j))
        {
            if (CheckBit(engine.board.pieceBoards[colorIndex], 6, j - colorDirection))
            {
                pseudoLegalMoves.push_back(Move(6, j - colorDirection, 7, j));
            }
        }
        j += colorDirection;
    }

    // Promotion - if a pawn move ends on the promotion rank, we need to assign convert to to the moves
    // First nonborder pawns
    for (auto i = 1; i < 7; i++)
    {
        if (CheckBit(pushes, i, promotionRank))
        {
            Move promotion = Move(i, promotionRank - colorDirection, i, promotionRank);
            for (auto pieceType = 2; pieceType < 6; pieceType++)
            {
                promotion.convertTo = (PieceType)pieceType;
                promotion.promotion = true;
                pseudoLegalMoves.push_back(promotion);
            }
        }
        if (CheckBit(captures, i, promotionRank))
        {
            if (CheckBit(engine.board.pieceBoards[colorIndex], i - 1, promotionRank - colorDirection)) // Captures are diagonal moves
            {
                Move promotion = Move(i - 1, promotionRank - colorDirection, i, promotionRank);
                for (auto pieceType = 2; pieceType < 6; pieceType++)
                {
                    promotion.convertTo = (PieceType)pieceType;
                    promotion.promotion = true;
                    pseudoLegalMoves.push_back(promotion);
                }
            }
            if (CheckBit(engine.board.pieceBoards[colorIndex], i + 1, promotionRank - colorDirection)) // Because two pawns might capture on the same square, we need to check from where the captures came
            {
                Move promotion = Move(i + 1, j - colorDirection, i, promotionRank);
                for (auto pieceType = 2; pieceType < 6; pieceType++)
                {
                    promotion.convertTo = (PieceType)pieceType;
                    promotion.promotion = true;
                    pseudoLegalMoves.push_back(promotion);
                }
            }
        }
    }
    // Now problematic border pawns
    for (auto i = 0; i < 8; i += 7)
    {
        if (CheckBit(pushes, i, promotionRank))
        {
            Move promotion = Move(i, promotionRank - colorDirection, i, promotionRank);
            for (auto pieceType = 2; pieceType < 6; pieceType++)
            {
                promotion.convertTo = (PieceType)pieceType;
                promotion.promotion = true;
                pseudoLegalMoves.push_back(promotion);
            }
        }
    }
    if (CheckBit(captures, 0, promotionRank))
    {
        if (CheckBit(engine.board.pieceBoards[colorIndex], 0 + 1, promotionRank - colorDirection)) // Because two pawns might capture on the same square, we need to check from where the captures came
        {
            Move promotion = Move(0 + 1, promotionRank - colorDirection, 0, promotionRank);
            for (auto pieceType = 2; pieceType < 6; pieceType++)
            {
                promotion.convertTo = (PieceType)pieceType;
                promotion.promotion = true;
                pseudoLegalMoves.push_back(promotion);
            }
        }
    }
    if (CheckBit(captures, 7, promotionRank))
    {
        if (CheckBit(engine.board.pieceBoards[colorIndex], 7 - 1, promotionRank - colorDirection)) // Captures are diagonal moves
        {
            Move promotion = Move(7 - 1, promotionRank - colorDirection, 7, promotionRank);
            for (auto pieceType = 2; pieceType < 6; pieceType++)
            {
                promotion.convertTo = (PieceType)pieceType;
                promotion.promotion = true;
                pseudoLegalMoves.push_back(promotion);
            }
        }
    }
}

void GetPseudoLegalKnightMoves(const Engine &engine, std::vector<Move> &pseudoLegalMoves)
{
    auto color = engine.board.whiteToMove;
    auto colorIndex = color ? 0 : 6;
    uint_fast64_t thisBoard = ~engine.board.colorBoards[!color]; // friendly blockers
    for (auto i = 0; i < 8; i++)                                 // loop over all squares to find knights
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
                            if (CheckBit(thisBoard, ii, jj))
                            {
                                pseudoLegalMoves.push_back(Move(i, j, ii, jj));
                            }
                        }
                        ii = i + dj;
                        jj = j + di;
                        if (ii >= 0 && ii < 8 && jj >= 0 && jj < 8) // Check for knight moves with x 2y
                        {
                            if (CheckBit(thisBoard, ii, jj))
                            {
                                pseudoLegalMoves.push_back(Move(i, j, ii, jj));
                            }
                        }
                    }
                }
            }
        }
    }
}

void GetPseudoLegalKingMoves(const Engine &engine, std::vector<Move> &pseudoLegalMoves)
{
    auto color = engine.board.whiteToMove;
    Coord kingCoord = engine.board.kingCoords[!color];
    auto castleColorIndex = color ? 0 : 2;                                                    // to obtain colored castling rules
    uint_fast64_t combinedColors = engine.board.colorBoards[0] | engine.board.colorBoards[1]; // Bitboard with both friendly and unfriendly blockers for castling
    auto firstRank = color ? 7 : 0;

    // normal king moves
    uint_fast64_t moves = ZERO;
    SetBit(moves, kingCoord.x, kingCoord.y);
    moves = moves >> 8 | moves >> 9 | moves >> 1 | moves << 7 | moves << 8 | moves << 9 | moves << 1 | moves >> 7; // all king increments
    moves &= ~engine.board.colorBoards[!color];                                                                    // removes blocked suqares
    for (auto i = 0; i < 8; i++)
    {
        for (auto j = 0; j < 8; j++)
        {
            if (CheckBit(moves, i, j) && (std::abs(i - kingCoord.x) == 1 || std::abs(j - kingCoord.y) == 1))
            { // moves are set bits of moves which lie in range of the king
                pseudoLegalMoves.push_back(Move(kingCoord.x, kingCoord.y, i, j));
            }
        }
    }

    // fucking castling
    auto occupied = false;
    if (engine.board.castlingRights[castleColorIndex + 1])
    { // Queenside castling
        for (auto di = -1; di > -4; di--)
        { // loop from king to left rook
            if (CheckBit(combinedColors, 4 + di, firstRank))
            { // if occupied square is found break
                occupied = true;
                break;
            }
        }
        if (!occupied)
        { // valid move if no blockers inbetween
            pseudoLegalMoves.push_back(Move(4, firstRank, 2, firstRank));
        }
    }
    occupied = false; // same but for kingside
    if (engine.board.castlingRights[castleColorIndex])
    {
        for (auto di = 1; di < 3; di++)
        {
            if (CheckBit(combinedColors, 4 + di, firstRank))
            {
                occupied = true;
                break;
            }
        }
        if (!occupied)
        {
            pseudoLegalMoves.push_back(Move(4, firstRank, 6, firstRank));
        }
    }
}

void GetPseudoLegalRookMoves(const Engine &engine, const uint_fast64_t &rookPieceBoard, std::vector<Move> &pseudoLegalMoves)
{
    auto color = engine.board.whiteToMove;
    uint_fast64_t thisBoard = engine.board.colorBoards[!color];
    uint_fast64_t otherBoard = engine.board.colorBoards[color];

    for (auto i = 0; i < 8; i++)
    {
        for (auto j = 0; j < 8; j++)
        {
            if (CheckBit(rookPieceBoard, i, j))
            {
                for (auto di = 1; di < 8 - i; di++)
                {
                    if (CheckBit(thisBoard, i + di, j))
                    {
                        break;
                    }
                    pseudoLegalMoves.push_back(Move(i, j, i + di, j));
                    if (CheckBit(otherBoard, i + di, j))
                    {
                        break;
                    }
                }
                for (auto di = -1; di >= -i; di--)
                {
                    if (CheckBit(thisBoard, i + di, j))
                    {
                        break;
                    }
                    pseudoLegalMoves.push_back(Move(i, j, i + di, j));
                    if (CheckBit(otherBoard, i + di, j))
                    {
                        break;
                    }
                }
                for (auto dj = 1; dj < 8 - j; dj++)
                {
                    if (CheckBit(thisBoard, i, j + dj))
                    {
                        break;
                    }
                    pseudoLegalMoves.push_back(Move(i, j, i, j + dj));
                    if (CheckBit(otherBoard, i, j + dj))
                    {
                        break;
                    }
                }
                for (auto dj = -1; dj >= -j; dj--)
                {
                    if (CheckBit(thisBoard, i, j + dj))
                    {
                        break;
                    }
                    pseudoLegalMoves.push_back(Move(i, j, i, j + dj));
                    if (CheckBit(otherBoard, i, j + dj))
                    {
                        break;
                    }
                }
            }
        }
    }
}

void GetPseudoLegalBishopMoves(const Engine &engine, const uint_fast64_t &bishopPieceBoard, std::vector<Move> &pseudoLegalMoves)
{
    auto color = engine.board.whiteToMove;
    uint_fast64_t thisBoard = engine.board.colorBoards[!color];
    uint_fast64_t otherBoard = engine.board.colorBoards[color];

    for (auto i = 0; i < 8; i++)
    {
        for (auto j = 0; j < 8; j++)
        {
            if (CheckBit(bishopPieceBoard, i, j))
            {
                for (auto d = 1; d < std::min(8 - i, 8 - j); d++)
                {
                    if (CheckBit(thisBoard, i + d, j + d))
                    {
                        break;
                    }
                    pseudoLegalMoves.push_back(Move(i, j, i + d, j + d));
                    if (CheckBit(otherBoard, i + d, j + d))
                    {
                        break;
                    }
                }
                for (auto d = -1; d >= std::max(-i, -j); d--)
                {
                    if (CheckBit(thisBoard, i + d, j + d))
                    {
                        break;
                    }
                    pseudoLegalMoves.push_back(Move(i, j, i + d, j + d));
                    if (CheckBit(otherBoard, i + d, j + d))
                    {
                        break;
                    }
                }
                for (auto d = 1; d < std::min(8 - i, j); d++)
                {
                    if (CheckBit(thisBoard, i + d, j - d))
                    {
                        break;
                    }
                    pseudoLegalMoves.push_back(Move(i, j, i + d, j - d));
                    if (CheckBit(otherBoard, i + d, j - d))
                    {
                        break;
                    }
                }
                for (auto d = 1; d < std::min(i, 8 - j); d++)
                {
                    if (CheckBit(thisBoard, i - d, j + d))
                    {
                        break;
                    }
                    pseudoLegalMoves.push_back(Move(i, j, i - d, j + d));
                    if (CheckBit(otherBoard, i - d, j + d))
                    {
                        break;
                    }
                }
            }
        }
    }
}