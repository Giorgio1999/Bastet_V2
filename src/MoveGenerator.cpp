#include "Engine.h"
#include "BoardUtility.h"
#include "BitBoardUtility.h"
#include "MoveGenerator.h"
#include <vector>
#include <cstdint>
#include <iostream>
#include <cstring>
#include <array>
// Testing of specific position: position startpos moves d5d6 h3g2 a2a3
//  These are all functionalities connected to move generation

bitboard knightMoves[64];
bitboard kingMoves[64];
bitboard pawnAttacks[2][2][64];
bitboard fillUpAttacks[8][64];
bitboard aFileAttacks[8][64];

void GetPseudoLegalPawnMoves(Engine &engine, std::array<move, 256> &moveHolder, uint &moveHolderIndex)
{
    auto color = (engine.gameHistory[engine.gameHistoryIndex].flags & 1) == 1;
    auto colorIndex = color ? 0 : 6;
    auto colorDirection = color ? -8 : 8;
    auto startRank = color ? 1 : 6;
    auto promoterRank = color ? 6 : 1;
    bitboard combinedColors = engine.gameHistory[engine.gameHistoryIndex].colorBoards[color] | engine.gameHistory[engine.gameHistoryIndex].colorBoards[!color];
    bitboard pusher = engine.gameHistory[engine.gameHistoryIndex].pieceBoards[colorIndex] & ~rankMasks[promoterRank];
    bitboard doublePusher = pusher & rankMasks[startRank];
    bitboard promoter = engine.gameHistory[engine.gameHistoryIndex].pieceBoards[colorIndex] & rankMasks[promoterRank];
    bitboard capturer = pusher;
    bitboard promotionCapturer = promoter;

    // Single pawn push
    if (color)
    {
        pusher = (pusher >> 8) & ~combinedColors;
    }
    else
    {
        pusher = (pusher << 8) & ~combinedColors;
    }
    while (pusher > 0)
    {
        auto to = PopLsb(pusher);
        moveHolder[moveHolderIndex] = Move(to - colorDirection, to);
        moveHolderIndex++;
    }

    // Double pawn push
    if (color)
    {
        doublePusher = (((doublePusher >> 8) & ~combinedColors) >> 8) & ~combinedColors;
    }
    else
    {
        doublePusher = (((doublePusher << 8) & ~combinedColors) << 8) & ~combinedColors;
    }
    while (doublePusher > 0)
    {
        auto to = PopLsb(doublePusher);
        moveHolder[moveHolderIndex] = Move(to - 2 * colorDirection, to);
        moveHolderIndex++;
    }

    // Captures without promotion
    while (capturer > 0)
    {
        auto from = PopLsb(capturer);
        bitboard attacks = pawnAttacks[!color][0][from] & (engine.gameHistory[engine.gameHistoryIndex].colorBoards[color] | engine.gameHistory[engine.gameHistoryIndex].ghostBoard);
        if (attacks > 0)
        {
            moveHolder[moveHolderIndex] = Move(from, PopLsb(attacks));
            moveHolderIndex++;
        }
        attacks = pawnAttacks[!color][1][from] & (engine.gameHistory[engine.gameHistoryIndex].colorBoards[color] | engine.gameHistory[engine.gameHistoryIndex].ghostBoard);
        if (attacks > 0)
        {
            moveHolder[moveHolderIndex] = Move(from, PopLsb(attacks));
            moveHolderIndex++;
        }
    }

    // Push with promotion
    if (color)
    {
        promoter = (promoter >> 8) & ~combinedColors;
    }
    else
    {
        promoter = (promoter << 8) & ~combinedColors;
    }
    while (promoter > 0)
    {
        auto to = PopLsb(promoter);
        move move = Move(to - colorDirection, to);
        for (uint8_t i = 1; i < 5; i++)
        {
            moveHolder[moveHolderIndex] = Move(move, i, 1);
            moveHolderIndex++;
        }
    }

    // Capture with promotion
    while (promotionCapturer > 0)
    {
        auto from = PopLsb(promotionCapturer);
        bitboard attacks = pawnAttacks[!color][0][from] & engine.gameHistory[engine.gameHistoryIndex].colorBoards[color];
        if (attacks > 0)
        {
            move move = Move(from, PopLsb(attacks));
            for (uint8_t i = 1; i < 5; i++)
            {
                moveHolder[moveHolderIndex] = Move(move, i, 1);
                moveHolderIndex++;
            }
        }
        attacks = pawnAttacks[!color][1][from] & engine.gameHistory[engine.gameHistoryIndex].colorBoards[color];
        if (attacks > 0)
        {
            move move = Move(from, PopLsb(attacks));
            for (uint8_t i = 1; i < 5; i++)
            {
                moveHolder[moveHolderIndex] = Move(move, i, 1);
                moveHolderIndex++;
            }
        }
    }
}

void GetPseudoLegalKnightMoves(Engine &engine, std::array<move, 256> &moveHolder, uint &moveHolderIndex)
{
    auto color = (engine.gameHistory[engine.gameHistoryIndex].flags & 1) == 1;
    auto colorIndex = color ? 0 : 6;
    bitboard thisBoard = ~engine.gameHistory[engine.gameHistoryIndex].colorBoards[!color]; //  ~friendly blockers
    bitboard knights = engine.gameHistory[engine.gameHistoryIndex].pieceBoards[1 + colorIndex];
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
            moveHolder[moveHolderIndex] = Move(from, to);
            moveHolderIndex++;
        }
    }
}

void GetPseudoLegalKingMoves(Engine &engine, std::array<move, 256> &moveHolder, uint &moveHolderIndex)
{
    auto color = (engine.gameHistory[engine.gameHistoryIndex].flags & 1) == 1;
    int kingIndex = engine.gameHistory[engine.gameHistoryIndex].kingIndices[!color];                                                                   // to obtain colored castling rules
    bitboard combinedColors = engine.gameHistory[engine.gameHistoryIndex].colorBoards[0] | engine.gameHistory[engine.gameHistoryIndex].colorBoards[1]; // | engine.gameHistory[engine.gameHistoryIndex].attackBoard; // Bitboard with both friendly and unfriendly blockers and attacked squares for castling

    // normal king moves
    bitboard attacks = kingMoves[kingIndex] & ~engine.gameHistory[engine.gameHistoryIndex].colorBoards[!color];
    auto to = 0;
    while (attacks > 0)
    {
        to = PopLsb(attacks);
        moveHolder[moveHolderIndex] = Move(kingIndex, to);
        moveHolderIndex++;
    }

    // fucking castling
    bitboard kingSideMask = castleMasks[!color][0];
    bitboard queenSideMask = castleMasks[!color][1];
    bitboard kingsideOccupation = kingSideMask & combinedColors; // castling masks with blocked indices removed
    bitboard queenSideOccupation = queenSideMask & combinedColors;
    auto kingSideCastleTarget = color ? 62 : 6;
    auto queenSideCastleTarget = color ? 58 : 2;
    auto index = 0;
    if (kingsideOccupation == 0 && engine.gameHistory[engine.gameHistoryIndex].flags & (color ? 0b00000010 : 0b00001000))
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
            moveHolder[moveHolderIndex] = Move(kingIndex, kingSideCastleTarget);
            moveHolderIndex++;
        }
    }
    if (queenSideOccupation == 0 && engine.gameHistory[engine.gameHistoryIndex].flags & (color ? 0b00000100 : 0b00010000))
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
            moveHolder[moveHolderIndex] = Move(kingIndex, queenSideCastleTarget);
            moveHolderIndex++;
        }
    }
}

void GetPseudoLegalRookMoves(Engine &engine, const bitboard &rookPieceBoard, std::array<move, 256> &moveHolder, uint &moveHolderIndex)
{
    auto color = (engine.gameHistory[engine.gameHistoryIndex].flags & 1) == 1;
    bitboard thisBoard = engine.gameHistory[engine.gameHistoryIndex].colorBoards[!color];
    bitboard otherBoard = engine.gameHistory[engine.gameHistoryIndex].colorBoards[color];
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
            moveHolder[moveHolderIndex] = Move(from, to);
            moveHolderIndex++;
        }

        attacks = (GetRankAttacks(from, combinedBoard) | GetRankAttacks(from, combinedBoard)) & ~thisBoard;
        while (attacks > 0)
        {
            to = PopLsb(attacks);
            moveHolder[moveHolderIndex] = Move(from, to);
            moveHolderIndex++;
        }
    }
}

void GetPseudoLegalBishopMoves(Engine &engine, const bitboard &bishopPieceBoard, std::array<move, 256> &moveHolder, uint &moveHolderIndex)
{
    auto color = (engine.gameHistory[engine.gameHistoryIndex].flags & 1) == 1;
    bitboard thisBoard = engine.gameHistory[engine.gameHistoryIndex].colorBoards[!color];
    bitboard otherBoard = engine.gameHistory[engine.gameHistoryIndex].colorBoards[color];
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
            moveHolder[moveHolderIndex] = Move(from, to);
            moveHolderIndex++;
        }
    }
}

void MoveGenerator::GetLegalMoves(Engine &engine, std::array<move, 256> &moveHolder, uint &moveHolderIndex)
{
    auto color = (engine.gameHistory[engine.gameHistoryIndex].flags & 1) == 1;
    auto colorIndex = color ? 0 : 6;
    auto colorDirection = color ? -8 : 8;
    auto startRank = color ? 1 : 6;
    auto promoterRank = color ? 6 : 1;
    bitboard combinedColors = engine.gameHistory[engine.gameHistoryIndex].colorBoards[color] | engine.gameHistory[engine.gameHistoryIndex].colorBoards[!color];
    bitboard notCombinedColors = ~combinedColors;
    bitboard notThisBoard = ~engine.gameHistory[engine.gameHistoryIndex].colorBoards[!color]; //  ~friendly blockers
    bitboard otherBoard = engine.gameHistory[engine.gameHistoryIndex].colorBoards[color];
    bitboard knights = engine.gameHistory[engine.gameHistoryIndex].pieceBoards[1 + colorIndex];
    // GetPseudoLegalPawnMoves(engine, moveHolder, moveHolderIndex);
    // GetPseudoLegalKnightMoves(engine, moveHolder, moveHolderIndex);
    // GetPseudoLegalRookMoves(engine, engine.gameHistory[engine.gameHistoryIndex].pieceBoards[3 + colorIndex] | engine.gameHistory[engine.gameHistoryIndex].pieceBoards[4 + colorIndex], moveHolder, moveHolderIndex);
    // GetPseudoLegalBishopMoves(engine, engine.gameHistory[engine.gameHistoryIndex].pieceBoards[2 + colorIndex] | engine.gameHistory[engine.gameHistoryIndex].pieceBoards[4 + colorIndex], moveHolder, moveHolderIndex);
    // GetPseudoLegalKingMoves(engine, moveHolder, moveHolderIndex);
    square from;
    square to;
    bitboard attacks;
    move move;

    // Pawns
    //----------------------------------------------------------------------------------
    bitboard pusher = engine.gameHistory[engine.gameHistoryIndex].pieceBoards[colorIndex] & ~rankMasks[promoterRank];
    bitboard doublePusher = pusher & rankMasks[startRank];
    bitboard promoter = engine.gameHistory[engine.gameHistoryIndex].pieceBoards[colorIndex] & rankMasks[promoterRank];
    bitboard capturer = pusher;
    bitboard promotionCapturer = promoter;

    // Single pawn push
    if (color)
    {
        pusher = (pusher >> 8) & notCombinedColors;
    }
    else
    {
        pusher = (pusher << 8) & notCombinedColors;
    }
    while (pusher > 0)
    {
        to = PopLsb(pusher);
        move = Move(to - colorDirection, to);
        engine.MakeSimpleMove(move);
        if (IsSquareAttacked(engine, engine.gameHistory[engine.gameHistoryIndex].kingIndices[!color], !color))
        {
            engine.UndoLastMove();
            continue;
        }
        engine.UndoLastMove();
        moveHolder[moveHolderIndex] = move;
        moveHolderIndex++;
    }

    // Double pawn push
    if (color)
    {
        doublePusher = (((doublePusher >> 8) & notCombinedColors) >> 8) & notCombinedColors;
    }
    else
    {
        doublePusher = (((doublePusher << 8) & notCombinedColors) << 8) & notCombinedColors;
    }
    while (doublePusher > 0)
    {
        to = PopLsb(doublePusher);
        move = Move(to - 2 * colorDirection, to);
        engine.MakeSimpleMove(move);
        if (IsSquareAttacked(engine, engine.gameHistory[engine.gameHistoryIndex].kingIndices[!color], !color))
        {
            engine.UndoLastMove();
            continue;
        }
        engine.UndoLastMove();
        moveHolder[moveHolderIndex] = move;
        moveHolderIndex++;
    }

    // Captures without promotion
    while (capturer > 0)
    {
        from = PopLsb(capturer);
        attacks = pawnAttacks[!color][0][from] & (otherBoard | engine.gameHistory[engine.gameHistoryIndex].ghostBoard);
        if (attacks > 0)
        {
            move = Move(from, PopLsb(attacks));
            engine.MakeSimpleMove(move);
            if (!IsSquareAttacked(engine, engine.gameHistory[engine.gameHistoryIndex].kingIndices[!color], !color))
            {
                moveHolder[moveHolderIndex] = move;
                moveHolderIndex++;
            }
            engine.UndoLastMove();
        }
        attacks = pawnAttacks[!color][1][from] & (otherBoard | engine.gameHistory[engine.gameHistoryIndex].ghostBoard);
        // PrintBitBoard(attacks);
        // PrintBitBoard(pawnAttacks[!color][1][from]);
        // PrintBitBoard(otherBoard);
        if (attacks > 0)
        {
            move = Move(from, PopLsb(attacks));
            engine.MakeSimpleMove(move);
            if (IsSquareAttacked(engine, engine.gameHistory[engine.gameHistoryIndex].kingIndices[!color], !color))
            {
                engine.UndoLastMove();
                continue;
            }
            engine.UndoLastMove();
            moveHolder[moveHolderIndex] = move;
            moveHolderIndex++;
        }
    }

    // Push with promotion
    if (color)
    {
        promoter = (promoter >> 8) & notCombinedColors;
    }
    else
    {
        promoter = (promoter << 8) & notCombinedColors;
    }
    while (promoter > 0)
    {
        to = PopLsb(promoter);
        move = Move(to - colorDirection, to);
        engine.MakeSimpleMove(move);
        if (IsSquareAttacked(engine, engine.gameHistory[engine.gameHistoryIndex].kingIndices[!color], !color))
        {
            engine.UndoLastMove();
            continue;
        }
        engine.UndoLastMove();
        for (uint8_t i = 1; i < 5; i++)
        {
            moveHolder[moveHolderIndex] = Move(move, i, 1);
            moveHolderIndex++;
        }
    }

    // Capture with promotion
    while (promotionCapturer > 0)
    {
        from = PopLsb(promotionCapturer);
        attacks = pawnAttacks[!color][0][from] & otherBoard;
        if (attacks > 0)
        {
            move = Move(from, PopLsb(attacks));
            engine.MakeSimpleMove(move);
            if (!IsSquareAttacked(engine, engine.gameHistory[engine.gameHistoryIndex].kingIndices[!color], !color))
            {
                for (uint8_t i = 1; i < 5; i++)
                {
                    moveHolder[moveHolderIndex] = Move(move, i, 1);
                    moveHolderIndex++;
                }
            }
            engine.UndoLastMove();
        }
        attacks = pawnAttacks[!color][1][from] & otherBoard;
        if (attacks > 0)
        {
            move = Move(from, PopLsb(attacks));
            engine.MakeSimpleMove(move);
            if (IsSquareAttacked(engine, engine.gameHistory[engine.gameHistoryIndex].kingIndices[!color], !color))
            {
                engine.UndoLastMove();
                continue;
            }
            engine.UndoLastMove();
            for (uint8_t i = 1; i < 5; i++)
            {
                moveHolder[moveHolderIndex] = Move(move, i, 1);
                moveHolderIndex++;
            }
        }
    }
    // ---------------------------------------------------------------------------------------------------------------------------

    // Knights
    // --------------------------------------------------------------------------------------------------------------------------
    while (knights > 0)
    {
        from = PopLsb(knights);
        attacks = knightMoves[from] & notThisBoard;
        while (attacks > 0)
        {
            to = PopLsb(attacks);
            move = Move(from, to);
            engine.MakeSimpleMove(move);
            if (IsSquareAttacked(engine, engine.gameHistory[engine.gameHistoryIndex].kingIndices[!color], !color))
            {
                engine.UndoLastMove();
                continue;
            }
            engine.UndoLastMove();
            moveHolder[moveHolderIndex] = move;
            moveHolderIndex++;
        }
    }
    // -----------------------------------------------------------------------------------------------------------------------------

    // King
    //  ----------------------------------------------------------------------------------------------------------------------------------
    int kingIndex = engine.gameHistory[engine.gameHistoryIndex].kingIndices[!color];

    // normal king moves
    attacks = kingMoves[kingIndex] & notThisBoard;
    while (attacks > 0)
    {
        to = PopLsb(attacks);
        move = Move(kingIndex, to);
        engine.MakeSimpleMove(move);
        if (IsSquareAttacked(engine, engine.gameHistory[engine.gameHistoryIndex].kingIndices[!color], !color))
        {
            engine.UndoLastMove();
            continue;
        }
        engine.UndoLastMove();
        moveHolder[moveHolderIndex] = move;
        moveHolderIndex++;
    }

    // fucking castling
    bitboard kingSideMask = castleMasks[!color][0];
    bitboard queenSideMask = castleMasks[!color][1];
    bitboard kingsideOccupation = kingSideMask & combinedColors; // castling masks with blocked indices removed
    bitboard queenSideOccupation = queenSideMask & combinedColors;
    auto kingSideCastleTarget = color ? 62 : 6;
    auto queenSideCastleTarget = color ? 58 : 2;
    if (kingsideOccupation == 0 && engine.gameHistory[engine.gameHistoryIndex].flags & (color ? 0b00000010 : 0b00001000))
    {
        auto canCastle = true;
        while (kingSideMask > 0)
        {
            to = PopLsb(kingSideMask);
            if (MoveGenerator::IsSquareAttacked(engine, to, !color))
            {
                canCastle = false;
                break;
            }
        }
        if (canCastle && !MoveGenerator::IsSquareAttacked(engine, kingIndex, !color))
        {
            moveHolder[moveHolderIndex] = Move(kingIndex, kingSideCastleTarget);
            moveHolderIndex++;
        }
    }
    if (queenSideOccupation == 0 && engine.gameHistory[engine.gameHistoryIndex].flags & (color ? 0b00000100 : 0b00010000))
    {
        auto canCastle = true;
        while (queenSideMask > 0)
        {
            to = PopLsb(queenSideMask);
            if (to != queenSideCastleTarget - 1)
            {
                if (MoveGenerator::IsSquareAttacked(engine, to, !color))
                {
                    canCastle = false;
                    break;
                }
            }
        }
        if (canCastle && !MoveGenerator::IsSquareAttacked(engine, kingIndex, !color))
        {
            moveHolder[moveHolderIndex] = Move(kingIndex, queenSideCastleTarget);
            moveHolderIndex++;
        }
    }
    // ---------------------------------------------------------------------------------------------

    // Rooks
    // -------------------------------------------------------------------------------------------------
    bitboard rookBoard = engine.gameHistory[engine.gameHistoryIndex].pieceBoards[3 + colorIndex] | engine.gameHistory[engine.gameHistoryIndex].pieceBoards[4 + colorIndex];

    while (rookBoard > 0)
    {
        from = PopLsb(rookBoard);
        attacks = GetFileAttacks(from, combinedColors) & notThisBoard;
        while (attacks > 0)
        {
            to = PopLsb(attacks);
            move = Move(from, to);
            engine.MakeSimpleMove(move);
            if (IsSquareAttacked(engine, engine.gameHistory[engine.gameHistoryIndex].kingIndices[!color], !color))
            {
                engine.UndoLastMove();
                continue;
            }
            engine.UndoLastMove();
            moveHolder[moveHolderIndex] = move;
            moveHolderIndex++;
        }

        attacks = (GetRankAttacks(from, combinedColors) | GetRankAttacks(from, combinedColors)) & notThisBoard;
        while (attacks > 0)
        {
            to = PopLsb(attacks);
            move = Move(from, to);
            engine.MakeSimpleMove(move);
            if (IsSquareAttacked(engine, engine.gameHistory[engine.gameHistoryIndex].kingIndices[!color], !color))
            {
                engine.UndoLastMove();
                continue;
            }
            engine.UndoLastMove();
            moveHolder[moveHolderIndex] = move;
            moveHolderIndex++;
        }
    }
    // -------------------------------------------------------------------------------------------------------------------

    // Bishops
    // ---------------------------------------------------------------------------------------------------------------------
    bitboard bishopBoard = engine.gameHistory[engine.gameHistoryIndex].pieceBoards[2 + colorIndex] | engine.gameHistory[engine.gameHistoryIndex].pieceBoards[4 + colorIndex];

    while (bishopBoard > 0)
    {
        from = PopLsb(bishopBoard);
        attacks = (GetDiagonalAttacks(from, combinedColors) | GetAntiDiagonalAttacks(from, combinedColors)) & notThisBoard;
        while (attacks > 0)
        {
            to = PopLsb(attacks);
            move = Move(from, to);
            engine.MakeSimpleMove(move);
            if (IsSquareAttacked(engine, engine.gameHistory[engine.gameHistoryIndex].kingIndices[!color], !color))
            {
                engine.UndoLastMove();
                continue;
            }
            engine.UndoLastMove();
            moveHolder[moveHolderIndex] = move;
            moveHolderIndex++;
        }
    }
    // -----------------------------------------------------------------------------------------------------------------------

    // auto end = 0;
    // for (uint i = 0; i < moveHolderIndex; i++)
    // {
    //     engine.MakeSimpleMove(moveHolder[i]);
    //     if (!MoveGenerator::IsSquareAttacked(engine, engine.gameHistory[engine.gameHistoryIndex].kingIndices[!color], !color))
    //     {
    //         std::memcpy(&moveHolder[end], &moveHolder[i], sizeof(move));
    //         end++;
    //     }
    //     engine.UndoLastMove();
    // }
    // moveHolderIndex = end;
}

bool MoveGenerator::IsSquareAttacked(const Engine &engine, const int &index, const bool &attackingColor)
{
    auto colorIndex = attackingColor ? 0 : 6;
    // Bitboards containing slider pieces
    bitboard horAndVertSliderBoard = engine.gameHistory[engine.gameHistoryIndex].pieceBoards[3 + colorIndex] | engine.gameHistory[engine.gameHistoryIndex].pieceBoards[4 + colorIndex];
    bitboard diagonalSliderBoard = engine.gameHistory[engine.gameHistoryIndex].pieceBoards[2 + colorIndex] | engine.gameHistory[engine.gameHistoryIndex].pieceBoards[4 + colorIndex];
    // Bitboards containing all pieces which cant check by sliding
    bitboard horAndVertBlockerBoard = (engine.gameHistory[engine.gameHistoryIndex].colorBoards[!attackingColor] & ~engine.gameHistory[engine.gameHistoryIndex].pieceBoards[3 + colorIndex] & ~engine.gameHistory[engine.gameHistoryIndex].pieceBoards[4 + colorIndex]) | engine.gameHistory[engine.gameHistoryIndex].colorBoards[attackingColor];
    bitboard diagonalBlockerBoard = (engine.gameHistory[engine.gameHistoryIndex].colorBoards[!attackingColor] & ~engine.gameHistory[engine.gameHistoryIndex].pieceBoards[2 + colorIndex] & ~engine.gameHistory[engine.gameHistoryIndex].pieceBoards[4 + colorIndex]) | engine.gameHistory[engine.gameHistoryIndex].colorBoards[attackingColor];

    // Knights: can the piece on index attack a attackingcolor knight like a knight
    bitboard attacks = knightMoves[index] & engine.gameHistory[engine.gameHistoryIndex].pieceBoards[1 + colorIndex];
    if (attacks > 0)
    {
        return true;
    }

    // King: is the piece on index attacked by the attackingcolor king
    int attackingKingIndex = engine.gameHistory[engine.gameHistoryIndex].kingIndices[!attackingColor];
    attacks = kingMoves[attackingKingIndex];
    // if (CheckBit(attacks, index))
    if (attacks & (ONE << index))
    {
        return true;
    }

    // Pawns: can the piece on index attack an attackingcolor pawn like a pawn
    attacks = (pawnAttacks[attackingColor][0][index] | pawnAttacks[attackingColor][1][index]) & engine.gameHistory[engine.gameHistoryIndex].pieceBoards[colorIndex];
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
    if (attacks > 0)
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
