#include "Engine.h"
#include "Move.h"
#include "BitBoardUtility.h"
#include "MoveGenerator.h"
#include <vector>
#include <cstdint>

// These are all functionalities connected to move generation

void MoveGenerator::GetPseudoLegalMoves(const Engine &engine, std::vector<Move> &pseudoLegalMoves)
{
    GetPseudoLegalPawnMoves(engine, pseudoLegalMoves);
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
    captures = captures & (engine.board.colorBoards[color] | engine.board.ghostBoards[color]); // check for capturable pieces there
    for (auto i = 1; i < 7; i++)                                                               // pawns at the edge need to be handled seperately to prevent telepawns
    {
        auto j = startRank + 1; // Again only consider captures between start rank and one short of promotion
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
    auto j = startRank+1; // Treatment of the border pawns
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
    j = startRank+1;
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