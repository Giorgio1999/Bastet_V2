#include "Engine.h"
#include "BitBoardUtility.h"
#include "MoveGenerator.h"
#include <array>
//  These are all functionalities connected to move generation

// Initialisation of lookup tables
// -------------------------------------------------------------------
bitboard knightMoves[64];
bitboard kingMoves[64];
bitboard pawnAttacks[2][2][64];
bitboard fillUpAttacks[8][64];
bitboard aFileAttacks[8][64];
// -------------------------------------------------------------------

// External Functions
// -------------------------------------------------------------------
void MoveGenerator::GetLegalMoves(Engine &engine, std::array<move, 256> &moveHolder, uint &moveHolderIndex)
{
    // Initialisation of some helpful quantities shared by all pieces
    Board &currentBoard = engine.CurrentBoard();                                                  // Reference to current board
    auto color = (currentBoard.flags & 1) == 1;                                                   // Bool stating who is to move: true white, false black
    auto colorIndex = color ? 0 : 6;                                                              // Index offset into piece boards depending on color
    auto colorDirection = color ? -8 : 8;                                                         // Direction of pawn moves depending on color
    auto startRank = color ? 1 : 6;                                                               // Start rank of pawns depending on color
    auto promoterRank = color ? 6 : 1;                                                            // Promotion rank of pawns depending on color
    bitboard combinedColors = currentBoard.colorBoards[color] | currentBoard.colorBoards[!color]; // All pieces
    bitboard notCombinedColors = ~combinedColors;                                                 // ~combined colors
    bitboard notThisBoard = ~currentBoard.colorBoards[!color];                                    //  ~friendly blockers
    bitboard otherBoard = currentBoard.colorBoards[color];                                        // Enemy pieces

    // Predeclaration to save time in main function
    square from;
    square to;
    bitboard attacks;
    move move;

    // Pawns
    //----------------------------------------------------------------------------------
    bitboard pusher = currentBoard.pieceBoards[colorIndex] & ~rankMasks[promoterRank];  // Pawns which will move up and not promote
    bitboard doublePusher = pusher & rankMasks[startRank];                              // Pawn which will move up twice
    bitboard promoter = currentBoard.pieceBoards[colorIndex] & rankMasks[promoterRank]; // Pawns which will move up and promote
    bitboard capturer = pusher;                                                         // Pawns which will capture and not promote
    bitboard promotionCapturer = promoter;                                              // Pawn which will capture and promote

    // Single pawn push
    if (color) // Push pawn up depending on color and removing pawns on occupied squares
    {
        pusher = (pusher >> 8) & notCombinedColors;
    }
    else
    {
        pusher = (pusher << 8) & notCombinedColors;
    }

    while (pusher > 0) // Add all pawn pushes to legalmoves if the move does not put player in check
    {
        to = PopLsb(pusher);
        move = Move(to - colorDirection, to);
        engine.MakeSimpleMove(move);
        if (IsSquareAttacked(engine, currentBoard.kingIndices[!color], !color))
        {
            engine.UndoLastMove();
            continue;
        }
        engine.UndoLastMove();
        moveHolder[moveHolderIndex] = move;
        moveHolderIndex++;
    }

    // Double pawn push
    if (color) // Push double pusher twice up depending on color and remove all pawns which land on occupied squares
    {
        doublePusher = (((doublePusher >> 8) & notCombinedColors) >> 8) & notCombinedColors;
    }
    else
    {
        doublePusher = (((doublePusher << 8) & notCombinedColors) << 8) & notCombinedColors;
    }

    while (doublePusher > 0) // Add all double pushes to legalmoves if the move does not put player in check
    {
        to = PopLsb(doublePusher);
        move = Move(to - 2 * colorDirection, to);
        engine.MakeSimpleMove(move);
        if (IsSquareAttacked(engine, currentBoard.kingIndices[!color], !color))
        {
            engine.UndoLastMove();
            continue;
        }
        engine.UndoLastMove();
        moveHolder[moveHolderIndex] = move;
        moveHolderIndex++;
    }

    // Captures without promotion
    while (capturer > 0) // Add all caputes to legalmoves if the moves does not put player in check
    {
        from = PopLsb(capturer);
        attacks = pawnAttacks[!color][0][from] & (otherBoard | currentBoard.ghostBoard); // Test if captures enemy piece or ghost
        if (attacks > 0)
        {
            move = Move(from, PopLsb(attacks));
            engine.MakeSimpleMove(move);
            if (!IsSquareAttacked(engine, currentBoard.kingIndices[!color], !color))
            {
                moveHolder[moveHolderIndex] = move;
                moveHolderIndex++;
            }
            engine.UndoLastMove();
        }
        attacks = pawnAttacks[!color][1][from] & (otherBoard | currentBoard.ghostBoard);
        if (attacks > 0)
        {
            move = Move(from, PopLsb(attacks));
            engine.MakeSimpleMove(move);
            if (IsSquareAttacked(engine, currentBoard.kingIndices[!color], !color))
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
    if (color) // Push all pawn onto promotion square if it is not occupied
    {
        promoter = (promoter >> 8) & notCombinedColors;
    }
    else
    {
        promoter = (promoter << 8) & notCombinedColors;
    }

    while (promoter > 0) // Add all promotions to legalmoves if the move does not put the player in check
    {
        to = PopLsb(promoter);
        move = Move(to - colorDirection, to);
        engine.MakeSimpleMove(move);
        if (IsSquareAttacked(engine, currentBoard.kingIndices[!color], !color))
        {
            engine.UndoLastMove();
            continue;
        }
        engine.UndoLastMove();
        for (uint8_t i = 1; i < 5; i++) // If move is valid loop over all possible promotions
        {
            moveHolder[moveHolderIndex] = Move(move, i, 1);
            moveHolderIndex++;
        }
    }

    // Capture with promotion
    while (promotionCapturer > 0) // Add all captures that land on promotions squares to legalmoves if the move does not put the player in check
    {
        from = PopLsb(promotionCapturer);
        attacks = pawnAttacks[!color][0][from] & otherBoard;
        if (attacks > 0)
        {
            move = Move(from, PopLsb(attacks));
            engine.MakeSimpleMove(move);
            if (!IsSquareAttacked(engine, currentBoard.kingIndices[!color], !color))
            {
                for (uint8_t i = 1; i < 5; i++) // If move is valid loop over all possible promotions
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
            if (IsSquareAttacked(engine, currentBoard.kingIndices[!color], !color))
            {
                engine.UndoLastMove();
                continue;
            }
            engine.UndoLastMove();
            for (uint8_t i = 1; i < 5; i++) // If move is valid loop over all possible promotions
            {
                moveHolder[moveHolderIndex] = Move(move, i, 1);
                moveHolderIndex++;
            }
        }
    }
    // ---------------------------------------------------------------------------------------------------------------------------

    // Knights
    // --------------------------------------------------------------------------------------------------------------------------
    bitboard knights = currentBoard.pieceBoards[1 + colorIndex]; // Knight bitboard
    while (knights > 0)                                          // Loop over all knights
    {
        from = PopLsb(knights);
        attacks = knightMoves[from] & notThisBoard; // Get knight moves that do not land on friendly pieces
        while (attacks > 0)                         // Loop over all possible moves and add move to legalmoves if it does not put the player in check
        {
            to = PopLsb(attacks);
            move = Move(from, to);
            engine.MakeSimpleMove(move);
            if (IsSquareAttacked(engine, currentBoard.kingIndices[!color], !color))
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
    int kingIndex = currentBoard.kingIndices[!color]; // Square of friendly king

    // normal king moves
    attacks = kingMoves[kingIndex] & notThisBoard; // Get king moves that do not land on friendly pieces
    while (attacks > 0)                            // Loop over all possibles moves and add to legalmoves if move does not put player in check
    {
        to = PopLsb(attacks);
        move = Move(kingIndex, to);
        engine.MakeSimpleMove(move);
        if (IsSquareAttacked(engine, to, !color))
        {
            engine.UndoLastMove();
            continue;
        }
        engine.UndoLastMove();
        moveHolder[moveHolderIndex] = move;
        moveHolderIndex++;
    }

    // fucking castling
    bitboard kingSideMask = castleMasks[!color][0];                // Kingside castling mask
    bitboard queenSideMask = castleMasks[!color][1];               // Queenside castling mask
    bitboard kingsideOccupation = kingSideMask & combinedColors;   // Get occupation of kingside castling mask
    bitboard queenSideOccupation = queenSideMask & combinedColors; // Get occupation of queenside castling mask
    auto kingSideCastleTarget = color ? 62 : 6;                    // Target square for kingside castling depending on color
    auto queenSideCastleTarget = color ? 58 : 2;                   // Target square for queenside castling depending on color

    if (kingsideOccupation == 0 && currentBoard.flags & (color ? 0b00000010 : 0b00001000)) // If kingside castling mask is unoccupied and color dependend castling rights are given
    {
        auto canCastle = true;
        while (kingSideMask > 0) // Castling is only allowed if none of the king side castling mask squares is attacked
        {
            to = PopLsb(kingSideMask);
            if (MoveGenerator::IsSquareAttacked(engine, to, !color))
            {
                canCastle = false;
                break;
            }
        }
        if (canCastle && !MoveGenerator::IsSquareAttacked(engine, kingIndex, !color)) // Castling is only allowed if king is not in check
        {
            moveHolder[moveHolderIndex] = Move(kingIndex, kingSideCastleTarget);
            moveHolderIndex++;
        }
    }

    if (queenSideOccupation == 0 && currentBoard.flags & (color ? 0b00000100 : 0b00010000)) // If kingside castling mask is unoccupied and color dependend castling rights are given
    {
        auto canCastle = true;
        while (queenSideMask > 0) // Castling is only allowed if none of the king side castling mask squares is attacked
        {
            to = PopLsb(queenSideMask);
            if (to != queenSideCastleTarget - 1) // However, last square before rook is allowed to be attacked
            {
                if (MoveGenerator::IsSquareAttacked(engine, to, !color))
                {
                    canCastle = false;
                    break;
                }
            }
        }
        if (canCastle && !MoveGenerator::IsSquareAttacked(engine, kingIndex, !color)) // Castling is only allowed if king is not in check
        {
            moveHolder[moveHolderIndex] = Move(kingIndex, queenSideCastleTarget);
            moveHolderIndex++;
        }
    }
    // ---------------------------------------------------------------------------------------------

    // Rooks
    // -------------------------------------------------------------------------------------------------
    bitboard rookBoard = currentBoard.pieceBoards[3 + colorIndex] | currentBoard.pieceBoards[4 + colorIndex]; // Horizontal and vertical slider board (rook + queen)

    while (rookBoard > 0) // Loop over all horizontal and vertical sliders
    {
        from = PopLsb(rookBoard);

        attacks = (GetFileAttacks(from, combinedColors) | GetRankAttacks(from, combinedColors)) & notThisBoard; // Fetch kindergarten file attacks and removed friendly blockers, since attacks do not descriminate by color
        while (attacks > 0)                                                                                     // loop over all possible moves and add to legalmoves if move does not put player in check
        {
            to = PopLsb(attacks);
            move = Move(from, to);
            engine.MakeSimpleMove(move);
            if (IsSquareAttacked(engine, currentBoard.kingIndices[!color], !color))
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
    bitboard bishopBoard = currentBoard.pieceBoards[2 + colorIndex] | currentBoard.pieceBoards[4 + colorIndex]; // Diagonal slider board (bishop + queen)

    while (bishopBoard > 0) // Loop over all diagonal sliders
    {
        from = PopLsb(bishopBoard);
        attacks = (GetDiagonalAttacks(from, combinedColors) | GetAntiDiagonalAttacks(from, combinedColors)) & notThisBoard; // Fetch kindergarten attacks on diagonal and anti diagonal
        while (attacks > 0)                                                                                                 // loop over all possible moves and add to legalmoves if move does not put player in check
        {
            to = PopLsb(attacks);
            move = Move(from, to);
            engine.MakeSimpleMove(move);
            if (IsSquareAttacked(engine, currentBoard.kingIndices[!color], !color))
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
}

bool MoveGenerator::IsSquareAttacked(Engine &engine, const int &index, const bool &attackingColor)
{
    // Initialisation of some helpful quantities shared by all pieces
    Board &currentBoard = engine.CurrentBoard(); // Reference to current board
    auto colorIndex = attackingColor ? 0 : 6;    // Index offset into piece boards depending on attacking color

    // Bitboards containing slider pieces
    bitboard horAndVertSliderBoard = currentBoard.pieceBoards[3 + colorIndex] | currentBoard.pieceBoards[4 + colorIndex];
    bitboard diagonalSliderBoard = currentBoard.pieceBoards[2 + colorIndex] | currentBoard.pieceBoards[4 + colorIndex];

    // Bitboards containing all pieces which cant attack by sliding
    bitboard horAndVertBlockerBoard = (engine.gameHistory[engine.gameHistoryIndex].colorBoards[!attackingColor] & ~engine.gameHistory[engine.gameHistoryIndex].pieceBoards[3 + colorIndex] & ~engine.gameHistory[engine.gameHistoryIndex].pieceBoards[4 + colorIndex]) | engine.CurrentBoard().colorBoards[attackingColor];
    bitboard diagonalBlockerBoard = (engine.gameHistory[engine.gameHistoryIndex].colorBoards[!attackingColor] & ~engine.gameHistory[engine.gameHistoryIndex].pieceBoards[2 + colorIndex] & ~engine.gameHistory[engine.gameHistoryIndex].pieceBoards[4 + colorIndex]) | engine.CurrentBoard().colorBoards[attackingColor];

    // Knights: can the piece on index attack a attackingcolor knight like a knight
    bitboard attacks = knightMoves[index] & currentBoard.pieceBoards[1 + colorIndex];
    if (attacks > 0)
    {
        return true;
    }

    // King: is the piece on index attacked by the attackingcolor king
    int attackingKingIndex = currentBoard.kingIndices[!attackingColor];
    attacks = kingMoves[attackingKingIndex];
    if ((attacks & (ONE << index)) > 0)
    {
        return true;
    }

    // Pawns: can the piece on index attack an attackingcolor pawn like a pawn
    attacks = (pawnAttacks[attackingColor][0][index] | pawnAttacks[attackingColor][1][index]) & currentBoard.pieceBoards[colorIndex];
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
    attacks = (GetFileAttacks(index, horAndVertBlockerBoard) | GetRankAttacks(index, horAndVertBlockerBoard)) & horAndVertSliderBoard;
    if (attacks > 0)
    {
        return true;
    }

    return false;
}

void MoveGenerator::PreComputeMoves()
{
    PreComputeKingMoves();
    PreComputeKnightMoves();
    PreComputePawnAttacks();
    PreComputeFillUpAttacks();
    PreComputeAFileAttacks();
}
// -------------------------------------------------------------------

// Internal Functions
// -------------------------------------------------------------------
// Precompute lookuptables
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
    for (auto rank = 0; rank < 8; rank++) // loop over all 8 ranks
    {
        for (bitboard occ = 0; occ < 64; occ++) // loop over all 64 blocker configurations (blockers on the border do not count)
        {
            bitboard up = 0;
            bitboard down = 0;
            bitboard decompressedOcc = 0;
            for (auto i = 1; i < 8; i++)
            {
                decompressedOcc |= (occ & fileMasks[i - 1]) << (i * 8 - (i - 1)); // Convert occupation into file occupation
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

// Fetch kindergarten slider attacks
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
// -------------------------------------------------------------------