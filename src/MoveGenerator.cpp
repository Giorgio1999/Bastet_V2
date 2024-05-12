#include "Engine.h"
#include "Move.h"
#include "BitBoardUtility.h"
#include "MoveGenerator.h"
#include <vector>
#include <cstdint>
#include <iostream>
// Testing of specific position: position startpos moves d5d6 h3g2 a2a3
//  These are all functionalities connected to move generation

bitboard knightMoves[64];
bitboard kingMoves[64];
bitboard pawnAttacks[2][2][64];
bitboard fillUpAttacks[8][64];
bitboard aFileAttacks[8][64];

void MoveGenerator::GetPseudoLegalMoves(const Engine &engine, std::vector<Move> &pseudoLegalMoves)
{
    auto colorIndex = engine.gameHistory.at(engine.gameHistoryIndex).whiteToMove ? 0 : 6;
    GetPseudoLegalPawnMoves(engine, pseudoLegalMoves);
    GetPseudoLegalKnightMoves(engine, pseudoLegalMoves);
    GetPseudoLegalKingMoves(engine, pseudoLegalMoves);
    GetPseudoLegalRookMoves(engine, engine.gameHistory.at(engine.gameHistoryIndex).pieceBoards[3 + colorIndex] | engine.gameHistory.at(engine.gameHistoryIndex).pieceBoards[4 + colorIndex], pseudoLegalMoves);   // rook + queen
    GetPseudoLegalBishopMoves(engine, engine.gameHistory.at(engine.gameHistoryIndex).pieceBoards[2 + colorIndex] | engine.gameHistory.at(engine.gameHistoryIndex).pieceBoards[4 + colorIndex], pseudoLegalMoves); // bishop + queen
}

void GetPseudoLegalPawnMoves(const Engine &engine, std::vector<Move> &pseudoLegalMoves)
{
    auto color = engine.gameHistory.at(engine.gameHistoryIndex).whiteToMove;
    auto colorIndex = color ? 0 : 6;
    auto colorDirection = color ? -8 : 8;
    auto startRank = color ? 1 : 6;
    auto promotionRank = color ? 7 : 0;
    bitboard combinedColors = ~engine.gameHistory.at(engine.gameHistoryIndex).colorBoards[color] & ~engine.gameHistory.at(engine.gameHistoryIndex).colorBoards[!color];

    // Single Pawn pushes
    bitboard pushes = engine.gameHistory.at(engine.gameHistoryIndex).pieceBoards[colorIndex]; // Get pawn bitboard of mover color
    if (color)
    { // if white, push up
        pushes = pushes >> 8;
    }
    else
    { // if black, push down
        pushes = pushes << 8;
    }
    pushes &= combinedColors; // remove pawns which landed on occupied squares
    auto to = 0;
    while (pushes > 0)
    {
        to = PopLsb(pushes);
        Move move(to - colorDirection, to);
        if (CheckBit(rankMasks[promotionRank], to))
        {
            for (auto i = 1; i < 5; i++)
            {
                move.convertTo = i;
                move.promotion = true;
                pseudoLegalMoves.push_back(move);
            }
        }
        else
        {
            pseudoLegalMoves.push_back(move);
        }
    }

    // Double pawn pushes - push all pawns twice (check for blockers inbetween)
    pushes = engine.gameHistory.at(engine.gameHistoryIndex).pieceBoards[colorIndex] & rankMasks[startRank];
    if (color)
    { // if white push up
        pushes = ((pushes >> 8) & combinedColors) >> 8;
    }
    else
    { // if black push down
        pushes = ((pushes << 8) & combinedColors) << 8;
    }
    pushes &= combinedColors; // remove pawns which landed on occupied squares
    while (pushes > 0)
    {
        to = PopLsb(pushes);
        pseudoLegalMoves.push_back(Move(to - colorDirection * 2, to));
    }

    // // Pawn Captures
    bitboard pawnBoard = engine.gameHistory.at(engine.gameHistoryIndex).pieceBoards[colorIndex];
    auto from = 0;
    while (pawnBoard > 0)
    {
        from = PopLsb(pawnBoard);
        for (auto j = 0; j < 2; j++)
        {
            bitboard captures = pawnAttacks[!color][j][from] & (engine.gameHistory.at(engine.gameHistoryIndex).colorBoards[color] | engine.gameHistory.at(engine.gameHistoryIndex).ghostBoard);
            if (captures > 0)
            {
                to = PopLsb(captures);
                Move move(from, to);
                if (CheckBit(rankMasks[promotionRank], to))
                {
                    for (auto i = 1; i < 5; i++)
                    {
                        move.convertTo = i;
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
    }
}

void GetPseudoLegalKnightMoves(const Engine &engine, std::vector<Move> &pseudoLegalMoves)
{
    auto color = engine.gameHistory.at(engine.gameHistoryIndex).whiteToMove;
    auto colorIndex = color ? 0 : 6;
    bitboard thisBoard = ~engine.gameHistory.at(engine.gameHistoryIndex).colorBoards[!color]; //  ~friendly blockers
    bitboard knights = engine.gameHistory.at(engine.gameHistoryIndex).pieceBoards[1 + colorIndex];
    bitboard attacks = ZERO;
    auto from = 0;
    auto to = 0;
    while (knights > 0)
    {
        from = PopLsb(knights);
        attacks = knightMoves[from] & thisBoard;
        while (attacks > 0)
        {
            to = PopLsb(attacks);
            pseudoLegalMoves.push_back(Move(from, to));
        }
    }
}

void GetPseudoLegalKingMoves(const Engine &engine, std::vector<Move> &pseudoLegalMoves)
{
    auto color = engine.gameHistory.at(engine.gameHistoryIndex).whiteToMove;
    int kingIndex = engine.gameHistory.at(engine.gameHistoryIndex).kingIndices[!color];
    auto castleColorIndex = color ? 0 : 2;                                               // to obtain colored castling rules
    bitboard combinedColors = engine.gameHistory.at(engine.gameHistoryIndex).colorBoards[0] | engine.gameHistory.at(engine.gameHistoryIndex).colorBoards[1]; // | engine.gameHistory.at(engine.gameHistoryIndex).attackBoard; // Bitboard with both friendly and unfriendly blockers and attacked squares for castling

    // normal king moves
    bitboard attacks = kingMoves[kingIndex] & ~engine.gameHistory.at(engine.gameHistoryIndex).colorBoards[!color];
    auto to = 0;
    while (attacks > 0)
    {
        to = PopLsb(attacks);
        pseudoLegalMoves.push_back(Move(kingIndex, to));
    }

    // fucking castling
    bitboard kingSideMask = castleMasks[!color][0];
    bitboard queenSideMask = castleMasks[!color][1];
    bitboard kingsideOccupation = kingSideMask & combinedColors; // castling masks with blocked indices removed
    bitboard queenSideOccupation = queenSideMask & combinedColors;
    auto kingSideCastleTarget = color ? 62 : 6;
    auto queenSideCastleTarget = color ? 58 : 2;
    auto index = 0;
    if (kingsideOccupation == 0 && engine.gameHistory.at(engine.gameHistoryIndex).castlingRights[castleColorIndex])
    {
        auto canCastle = true;
        index = 0;
        while (kingSideMask > 0)
        {
            index = PopLsb(kingSideMask);
            if (MoveGenerator::IsSquareAttacked(engine, index, !color))
            {
                canCastle = false;
                break;
            }
        }
        if (canCastle && !MoveGenerator::IsSquareAttacked(engine, kingIndex, !color))
        {
            pseudoLegalMoves.push_back(Move(kingIndex, kingSideCastleTarget));
        }
    }
    if (queenSideOccupation == 0 && engine.gameHistory.at(engine.gameHistoryIndex).castlingRights[castleColorIndex + 1])
    {
        index = 0;
        auto canCastle = true;
        while (queenSideMask > 0)
        {
            index = PopLsb(queenSideMask);
            if (index != queenSideCastleTarget - 1)
            {
                if (MoveGenerator::IsSquareAttacked(engine, index, !color))
                {
                    canCastle = false;
                    break;
                }
            }
        }
        if (canCastle && !MoveGenerator::IsSquareAttacked(engine, kingIndex, !color))
        {
            pseudoLegalMoves.push_back(Move(kingIndex, queenSideCastleTarget));
        }
    }
}

void GetPseudoLegalRookMoves(const Engine &engine, const bitboard &rookPieceBoard, std::vector<Move> &pseudoLegalMoves)
{
    auto color = engine.gameHistory.at(engine.gameHistoryIndex).whiteToMove;
    bitboard thisBoard = engine.gameHistory.at(engine.gameHistoryIndex).colorBoards[!color];
    bitboard otherBoard = engine.gameHistory.at(engine.gameHistoryIndex).colorBoards[color];
    bitboard combinedBoard = thisBoard | otherBoard;
    bitboard rookBoard = rookPieceBoard;

    auto from = 0;
    auto to = 0;
    bitboard attacks = 0;
    while (rookBoard > 0)
    {
        from = PopLsb(rookBoard);
        attacks = GetFileAttacks(from, combinedBoard) & ~thisBoard;
        while (attacks > 0)
        {
            to = PopLsb(attacks);
            pseudoLegalMoves.push_back(Move(from, to));
        }

        attacks = (GetRankAttacks(from, combinedBoard) | GetRankAttacks(from, combinedBoard)) & ~thisBoard;
        while (attacks > 0)
        {
            to = PopLsb(attacks);
            pseudoLegalMoves.push_back(Move(from, to));
        }
    }
}

void GetPseudoLegalBishopMoves(const Engine &engine, const bitboard &bishopPieceBoard, std::vector<Move> &pseudoLegalMoves)
{
    auto color = engine.gameHistory.at(engine.gameHistoryIndex).whiteToMove;
    bitboard thisBoard = engine.gameHistory.at(engine.gameHistoryIndex).colorBoards[!color];
    bitboard otherBoard = engine.gameHistory.at(engine.gameHistoryIndex).colorBoards[color];
    bitboard bishopBoard = bishopPieceBoard;
    bitboard combinedBoard = thisBoard | otherBoard;

    auto from = 0;
    auto to = 0;
    while (bishopBoard > 0)
    {
        from = PopLsb(bishopBoard);
        bitboard attacks = (GetDiagonalAttacks(from, combinedBoard) | GetAntiDiagonalAttacks(from, combinedBoard)) & ~thisBoard;
        while (attacks > 0)
        {
            to = PopLsb(attacks);
            pseudoLegalMoves.push_back(Move(from, to));
        }
    }
}

void MoveGenerator::GetLegalMoves(Engine &engine, std::vector<Move> &legalmoves)
{
    auto color = engine.gameHistory.at(engine.gameHistoryIndex).whiteToMove;
    auto colorIndex = color ? 0 : 6;
    std::vector<Move> pseudoLegalMoves;
    GetPseudoLegalPawnMoves(engine, pseudoLegalMoves);
    GetPseudoLegalKnightMoves(engine, pseudoLegalMoves);
    GetPseudoLegalRookMoves(engine, engine.gameHistory.at(engine.gameHistoryIndex).pieceBoards[3 + colorIndex] | engine.gameHistory.at(engine.gameHistoryIndex).pieceBoards[4 + colorIndex], pseudoLegalMoves);
    GetPseudoLegalBishopMoves(engine, engine.gameHistory.at(engine.gameHistoryIndex).pieceBoards[2 + colorIndex] | engine.gameHistory.at(engine.gameHistoryIndex).pieceBoards[4 + colorIndex], pseudoLegalMoves);
    GetPseudoLegalKingMoves(engine, pseudoLegalMoves);
    for (const auto &move : pseudoLegalMoves)
    {
        engine.MakeSimpleMove(move);
        if (!MoveGenerator::IsSquareAttacked(engine, engine.gameHistory.at(engine.gameHistoryIndex).kingIndices[!color], !color))
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
    bitboard horAndVertSliderBoard = engine.gameHistory.at(engine.gameHistoryIndex).pieceBoards[3 + colorIndex] | engine.gameHistory.at(engine.gameHistoryIndex).pieceBoards[4 + colorIndex];
    bitboard diagonalSliderBoard = engine.gameHistory.at(engine.gameHistoryIndex).pieceBoards[2 + colorIndex] | engine.gameHistory.at(engine.gameHistoryIndex).pieceBoards[4 + colorIndex];
    // Bitboards containing all pieces which cant check by sliding
    bitboard horAndVertBlockerBoard = (engine.gameHistory.at(engine.gameHistoryIndex).colorBoards[!attackingColor] & ~engine.gameHistory.at(engine.gameHistoryIndex).pieceBoards[3 + colorIndex] & ~engine.gameHistory.at(engine.gameHistoryIndex).pieceBoards[4 + colorIndex]) | engine.gameHistory.at(engine.gameHistoryIndex).colorBoards[attackingColor];
    bitboard diagonalBlockerBoard = (engine.gameHistory.at(engine.gameHistoryIndex).colorBoards[!attackingColor] & ~engine.gameHistory.at(engine.gameHistoryIndex).pieceBoards[2 + colorIndex] & ~engine.gameHistory.at(engine.gameHistoryIndex).pieceBoards[4 + colorIndex]) | engine.gameHistory.at(engine.gameHistoryIndex).colorBoards[attackingColor];

    // Knights: can the piece on index attack a attackingcolor knight like a knight
    bitboard attacks = knightMoves[index] & engine.gameHistory.at(engine.gameHistoryIndex).pieceBoards[1 + colorIndex];
    if (attacks > 0)
    {
        return true;
    }

    // King: is the piece on index attacked by the attackingcolor king
    int attackingKingIndex = engine.gameHistory.at(engine.gameHistoryIndex).kingIndices[!attackingColor];
    attacks = kingMoves[attackingKingIndex];
    if (CheckBit(attacks, index))
    {
        return true;
    }

    // Pawns: can the piece on index attack an attackingcolor pawn like a pawn
    attacks = (pawnAttacks[attackingColor][0][index] | pawnAttacks[attackingColor][1][index]) & engine.gameHistory.at(engine.gameHistoryIndex).pieceBoards[colorIndex];
    if (attacks > 0)
    {
        return true;
    }

    // Diagonal sliders: can the piece on index attack a attackingcolor diagonal slider like a diagonal slider
    attacks = (GetDiagonalAttacks(index, diagonalBlockerBoard) | GetAntiDiagonalAttacks(index, diagonalBlockerBoard)) & diagonalSliderBoard;
    if (attacks > 0)
    {
        return true;
    }

    // Horizontal and vertical sliders: can the piece on index attack a attacking color hor and vert slider like a hor and vert slider
    attacks = GetFileAttacks(index, horAndVertBlockerBoard) & horAndVertSliderBoard;
    if(attacks>0)
    {
        return true;
    }

    attacks = GetRankAttacks(index, horAndVertBlockerBoard) & horAndVertSliderBoard;
    if (attacks > 0)
    {
        return true;
    }

    return false;
}

bool MoveGenerator::IsSquareAttacked(const Engine &engine, const Coord &square, const bool &attackingColor)
{
    return IsSquareAttacked(engine, square.y * 8 + square.x, attackingColor);
}

bitboard GetDiagonalAttacks(const int &index, const bitboard &occ)
{
    bitboard compressedOcc = ((occ & diagonalAttackMasks[index]) * fileMasks[1]) >> 58;
    return fillUpAttacks[index & 7][compressedOcc] & diagonalAttackMasks[index];
}

bitboard GetAntiDiagonalAttacks(const int &index, const bitboard &occ)
{
    bitboard compressedOcc = ((occ & antiDiagonalAttackMasks[index]) * fileMasks[1]) >> 58;
    return fillUpAttacks[index & 7][compressedOcc] & antiDiagonalAttackMasks[index];
}

bitboard GetRankAttacks(const int &index, const bitboard &occ)
{
    bitboard compressedOcc = ((occ & rankAttackMasks[index]) * fileMasks[1]) >> 58;
    return fillUpAttacks[index & 7][compressedOcc] & rankAttackMasks[index];
}

bitboard GetFileAttacks(const int &index, const bitboard &occ)
{
    const bitboard antiDiac7h2 = 0x0204081020408000;
    bitboard compressedOcc = fileMasks[0] & (occ >> (index & 7));
    compressedOcc = (antiDiac7h2 * compressedOcc) >> 58;
    return aFileAttacks[index >> 3][compressedOcc] << (index & 7);
}

void MoveGenerator::PreComputeMoves()
{
    PreComputeKingMoves();
    PreComputeKnightMoves();
    PreComputePawnAttacks();
    PreComputeFillUpAttacks();
    PreComputeAFileAttacks();
}

void PreComputeKnightMoves()
{
    bitboard localMoves = 0;
    for (auto i = 0; i < 64; i++)
    {
        localMoves = ONE << i;
        localMoves = (localMoves & ~fileMasks[7]) << 17 | (localMoves & ~(fileMasks[6] | fileMasks[7])) << 10 | (localMoves & ~(fileMasks[6] | fileMasks[7])) >> 6 | (localMoves & ~fileMasks[7]) >> 15 | (localMoves & ~fileMasks[0]) >> 17 | (localMoves & ~(fileMasks[0] | fileMasks[1])) >> 10 | (localMoves & ~(fileMasks[0] | fileMasks[1])) << 6 | (localMoves & ~fileMasks[0]) << 15;
        knightMoves[i] = localMoves;
    }
}

void PreComputeKingMoves()
{
    bitboard localMoves = 0;
    for (auto i = 0; i < 64; i++)
    {
        localMoves = ONE << i;
        localMoves = localMoves << 8 | (localMoves & ~fileMasks[7]) << 9 | (localMoves & ~fileMasks[7]) << 1 | (localMoves & ~fileMasks[7]) >> 7 | localMoves >> 8 | (localMoves & ~fileMasks[0]) >> 9 | (localMoves & ~fileMasks[0]) >> 1 | (localMoves & ~fileMasks[0]) << 7;
        kingMoves[i] = localMoves;
    }
}

void PreComputePawnAttacks()
{
    bitboard localMoves = 0;
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

void PreComputeFillUpAttacks()
{
    bitboard localmoves = 0;
    for (auto file = 0; file < 8; file++) // loop over all 8 files
    {
        for (bitboard occ = 0; occ < 64; occ++) // loop over all 64 blocker configurations (blockers on the border do not count)
        {
            bitboard left = 0;
            bitboard right = 0;
            bitboard decompressedOcc = occ << 1; // get 8 bit occupation
            if (file < 7)
            {
                left = 1 << (file + 1);                         // left rank direction
                for (auto shift = 0; shift < 6 - file; shift++) // loop till the end of the rank
                {
                    left |= (left & ~decompressedOcc) << 1; // kind of flood fill; tries to push bits onto squares but if blocker is on square, no bits can pass the blocker
                }
            }
            if (file > 0)
            {
                right = 1 << (file - 1); // right rank direction
                for (auto shift = 0; shift < file; shift++)
                {
                    right |= (right & ~decompressedOcc) >> 1;
                }
            }
            left |= right; // combine left and right
            localmoves = left;
            localmoves |= localmoves << 8 | localmoves << 16 | localmoves << 24 | localmoves << 32 | localmoves << 40 | localmoves << 48 | localmoves << 56; // create 8 copies
            fillUpAttacks[file][occ] = localmoves;
        }
    }
}

void PreComputeAFileAttacks()
{
    for (auto rank = 0; rank < 8; rank++)
    {
        for (bitboard occ = 0; occ < 64; occ++)
        {
            bitboard up = 0;
            bitboard down = 0;
            bitboard decompressedOcc = 0;
            for (auto i = 1; i < 8; i++)
            {
                decompressedOcc |= (occ & fileMasks[i - 1]) << (i * 8 - (i - 1));
            }
            if (rank < 7)
            {
                up = ONE << (rank + 1) * 8;
                for (auto shift = 0; shift < 6 - rank; shift++)
                {
                    up |= (up & ~decompressedOcc) << 8;
                }
            }
            if (rank > 0)
            {
                down = ONE << (rank - 1) * 8;
                for (auto shift = 0; shift < rank; shift++)
                {
                    down |= (down & ~decompressedOcc) >> 8;
                }
            }
            up |= down;
            aFileAttacks[rank][occ] = up;
        }
    }
}
