#include "MoveGenerator.h"
#include "BitBoardUtility.h"
#include "Engine.h"
#include <array>
//  These are all functionalities connected to move generation

// External Functions
// -------------------------------------------------------------------
void MoveGenerator::GetLegalMoves(Engine &engine,
                                  std::array<move, 256> &moveHolder,
                                  uint &moveHolderIndex) {
    // Initialisation of some helpful quantities shared by all pieces
    Board &currentBoard = engine.CurrentBoard(); // Reference to current board
    auto color = (currentBoard.flags & PTM) ==
                 1; // Bool stating who is to move: true white, false black
    auto ncolor = !color; // ~color
    auto colorIndex =
        color
            ? WHITEPIECES
            : BLACKPIECES; // Index offset into piece boards depending on color
    int kingIndex =
        BitScanForwards(currentBoard.pieceBoards[KING + colorIndex]) -
        1; // Square of friendly king, if there is no friendly king (-1), there
           // are no legal moves
    if (kingIndex < 0) {
        moveHolderIndex = 0;
        return;
    }
    auto colorDirection =
        color ? -NO : NO; // Direction of pawn moves depending on color
    auto startRank = color ? 1 : 6; // Start rank of pawns depending on color
    auto promoterRank =
        color ? 6 : 1; // Promotion rank of pawns depending on color
    bitboard combinedColors = currentBoard.colorBoards[color] |
                              currentBoard.colorBoards[ncolor]; // All pieces
    bitboard notCombinedColors = ~combinedColors; // ~combined colors
    bitboard notThisBoard =
        ~currentBoard.colorBoards[ncolor]; //  ~friendly blockers
    bitboard otherBoard = currentBoard.colorBoards[color]; // Enemy pieces

    // Predeclaration to save time in main function
    square from;
    square to;
    bitboard attacks;
    move move;

    // Pawns
    //----------------------------------------------------------------------------------
    bitboard pusher =
        currentBoard.pieceBoards[colorIndex] &
        ~rankMasks[promoterRank]; // Pawns which will move up and not promote
    bitboard doublePusher =
        pusher & rankMasks[startRank]; // Pawn which will move up twice
    bitboard promoter =
        currentBoard.pieceBoards[colorIndex] &
        rankMasks[promoterRank]; // Pawns which will move up and promote
    bitboard capturer = pusher;  // Pawns which will capture and not promote
    bitboard promotionCapturer =
        promoter; // Pawn which will capture and promote

    // Single pawn push
    if (color) // Push pawn up depending on color and removing pawns on occupied
               // squares
    {
        pusher = (pusher >> NO) & notCombinedColors;
    } else {
        pusher = (pusher << NO) & notCombinedColors;
    }

    while (pusher > 0) // Add all pawn pushes to legalmoves if the move does not
                       // put player in check
    {
        to = PopLsb(pusher);
        move = Move(to - colorDirection, to);
        engine.MakeSimpleMove(move);
        if (MoveGenerator::IsSquareAttacked(engine, kingIndex, ncolor)) {
            engine.UndoLastSimpleMove();
            continue;
        }
        engine.UndoLastSimpleMove();
        moveHolder[moveHolderIndex] = move;
        moveHolderIndex++;
    }

    // Double pawn push
    if (color) // Push double pusher twice up depending on color and remove all
               // pawns which land on occupied squares
    {
        doublePusher = (((doublePusher >> NO) & notCombinedColors) >> NO) &
                       notCombinedColors;
    } else {
        doublePusher = (((doublePusher << NO) & notCombinedColors) << NO) &
                       notCombinedColors;
    }

    while (doublePusher > 0) // Add all double pushes to legalmoves if the move
                             // does not put player in check
    {
        to = PopLsb(doublePusher);
        move = Move(to - 2 * colorDirection, to);
        engine.MakeSimpleMove(move);
        if (MoveGenerator::IsSquareAttacked(engine, kingIndex, ncolor)) {
            engine.UndoLastSimpleMove();
            continue;
        }
        engine.UndoLastSimpleMove();
        moveHolder[moveHolderIndex] = move;
        moveHolderIndex++;
    }

    // Captures without promotion
    while (capturer > 0) // Add all caputes to legalmoves if the moves does not
                         // put player in check
    {
        from = PopLsb(capturer);
        attacks =
            pawnAttacks[ncolor][0][from] &
            (otherBoard |
             currentBoard.ghostBoard); // Test if captures enemy piece or ghost
        if (attacks > 0) {
            move = Move(from, PopLsb(attacks));
            engine.MakeSimpleMove(move);
            if (!MoveGenerator::IsSquareAttacked(engine, kingIndex, ncolor)) {
                moveHolder[moveHolderIndex] = move;
                moveHolderIndex++;
            }
            engine.UndoLastSimpleMove();
        }
        attacks = pawnAttacks[ncolor][1][from] &
                  (otherBoard | currentBoard.ghostBoard);
        if (attacks > 0) {
            move = Move(from, PopLsb(attacks));
            engine.MakeSimpleMove(move);
            if (MoveGenerator::IsSquareAttacked(engine, kingIndex, ncolor)) {
                engine.UndoLastSimpleMove();
                continue;
            }
            engine.UndoLastSimpleMove();
            moveHolder[moveHolderIndex] = move;
            moveHolderIndex++;
        }
    }

    // Push with promotion
    if (color) // Push all pawn onto promotion square if it is not occupied
    {
        promoter = (promoter >> NO) & notCombinedColors;
    } else {
        promoter = (promoter << NO) & notCombinedColors;
    }

    while (promoter > 0) // Add all promotions to legalmoves if the move does
                         // not put the player in check
    {
        to = PopLsb(promoter);
        move = Move(to - colorDirection, to);
        engine.MakeSimpleMove(move);
        if (MoveGenerator::IsSquareAttacked(engine, kingIndex, ncolor)) {
            engine.UndoLastSimpleMove();
            continue;
        }
        engine.UndoLastSimpleMove();
        for (uint8_t i = KNIGHT; i < KING;
             i++) // If move is valid loop over all possible promotions
        {
            moveHolder[moveHolderIndex] = Move(move, i, 1);
            moveHolderIndex++;
        }
    }

    // Capture with promotion
    while (promotionCapturer >
           0) // Add all captures that land on promotions squares to legalmoves
              // if the move does not put the player in check
    {
        from = PopLsb(promotionCapturer);
        attacks = pawnAttacks[ncolor][0][from] & otherBoard;
        if (attacks > 0) {
            move = Move(from, PopLsb(attacks));
            engine.MakeSimpleMove(move);
            if (!MoveGenerator::IsSquareAttacked(engine, kingIndex, ncolor)) {
                for (uint8_t i = KNIGHT; i < KING;
                     i++) // If move is valid loop over all possible promotions
                {
                    moveHolder[moveHolderIndex] = Move(move, i, 1);
                    moveHolderIndex++;
                }
            }
            engine.UndoLastSimpleMove();
        }
        attacks = pawnAttacks[ncolor][1][from] & otherBoard;
        if (attacks > 0) {
            move = Move(from, PopLsb(attacks));
            engine.MakeSimpleMove(move);
            if (MoveGenerator::IsSquareAttacked(engine, kingIndex, ncolor)) {
                engine.UndoLastSimpleMove();
                continue;
            }
            engine.UndoLastSimpleMove();
            for (uint8_t i = KNIGHT; i < KING;
                 i++) // If move is valid loop over all possible promotions
            {
                moveHolder[moveHolderIndex] = Move(move, i, 1);
                moveHolderIndex++;
            }
        }
    }
    // ---------------------------------------------------------------------------------------------------------------------------

    // Knights
    // --------------------------------------------------------------------------------------------------------------------------
    bitboard knights =
        currentBoard.pieceBoards[KNIGHT + colorIndex]; // Knight bitboard
    while (knights > 0)                                // Loop over all knights
    {
        from = PopLsb(knights);
        attacks =
            knightMoves[from] & notThisBoard; // Get knight moves that do not
                                              // land on friendly pieces
        while (attacks > 0) // Loop over all possible moves and add move to
                            // legalmoves if it does not put the player in check
        {
            to = PopLsb(attacks);
            move = Move(from, to);
            engine.MakeSimpleMove(move);
            if (MoveGenerator::IsSquareAttacked(engine, kingIndex, ncolor)) {
                engine.UndoLastSimpleMove();
                continue;
            }
            engine.UndoLastSimpleMove();
            moveHolder[moveHolderIndex] = move;
            moveHolderIndex++;
        }
    }
    // -----------------------------------------------------------------------------------------------------------------------------

    // King
    //  ----------------------------------------------------------------------------------------------------------------------------------
    // normal king moves
    attacks =
        kingMoves[kingIndex] &
        notThisBoard;   // Get king moves that do not land on friendly pieces
    while (attacks > 0) // Loop over all possibles moves and add to legalmoves
                        // if move does not put player in check
    {
        to = PopLsb(attacks);
        move = Move(kingIndex, to);
        engine.MakeSimpleMove(move);
        if (IsSquareAttacked(engine, to, ncolor)) {
            engine.UndoLastSimpleMove();
            continue;
        }
        engine.UndoLastSimpleMove();
        moveHolder[moveHolderIndex] = move;
        moveHolderIndex++;
    }

    // fucking castling
    bitboard kingSideMask = castleMasks[ncolor][0];  // Kingside castling mask
    bitboard queenSideMask = castleMasks[ncolor][1]; // Queenside castling mask
    bitboard kingsideOccupation =
        kingSideMask &
        combinedColors; // Get occupation of kingside castling mask
    bitboard queenSideOccupation =
        queenSideMask &
        combinedColors; // Get occupation of queenside castling mask
                        // auto kingSideCastleTarget = color ? 62 : 6; // Target
                        // square for kingside castling depending on color
    auto kingSideCastleTarget =
        color ? G1
              : G8; // Target square for kingside castling depending on color
    auto queenSideCastleTarget =
        color ? C1
              : C8; // Target square for queenside castling depending on color

    if (kingsideOccupation == 0 &&
        currentBoard.flags &
            (color ? KCW : KCB)) // If kingside castling mask is unoccupied and
                                 // color dependend castling rights are given
    {
        auto canCastle = true;
        while (kingSideMask > 0) // Castling is only allowed if none of the king
                                 // side castling mask squares is attacked
        {
            to = PopLsb(kingSideMask);
            if (MoveGenerator::IsSquareAttacked(engine, to, ncolor)) {
                canCastle = false;
                break;
            }
        }
        if (canCastle &&
            !MoveGenerator::IsSquareAttacked(
                engine, kingIndex,
                ncolor)) // Castling is only allowed if king is not in check
        {
            moveHolder[moveHolderIndex] = Move(kingIndex, kingSideCastleTarget);
            moveHolderIndex++;
        }
    }

    if (queenSideOccupation == 0 &&
        currentBoard.flags &
            (color ? QCW : QCB)) // If kingside castling mask is unoccupied and
                                 // color dependend castling rights are given
    {
        auto canCastle = true;
        while (queenSideMask > 0) // Castling is only allowed if none of the
                                  // king side castling mask squares is attacked
        {
            to = PopLsb(queenSideMask);
            if (to !=
                queenSideCastleTarget - 1) // However, last square before rook
                                           // is allowed to be attacked
            {
                if (MoveGenerator::IsSquareAttacked(engine, to, ncolor)) {
                    canCastle = false;
                    break;
                }
            }
        }
        if (canCastle &&
            !MoveGenerator::IsSquareAttacked(
                engine, kingIndex,
                ncolor)) // Castling is only allowed if king is not in check
        {
            moveHolder[moveHolderIndex] =
                Move(kingIndex, queenSideCastleTarget);
            moveHolderIndex++;
        }
    }
    // ---------------------------------------------------------------------------------------------

    // Rooks
    // -------------------------------------------------------------------------------------------------
    bitboard rookBoard =
        currentBoard.pieceBoards[ROOK + colorIndex] |
        currentBoard
            .pieceBoards[QUEEN + colorIndex]; // Horizontal and vertical slider
                                              // board (rook + queen)

    while (rookBoard > 0) // Loop over all horizontal and vertical sliders
    {
        from = PopLsb(rookBoard);

        attacks = (GetFileAttacks(from, combinedColors) |
                   GetRankAttacks(from, combinedColors)) &
                  notThisBoard; // Fetch kindergarten file attacks and removed
                                // friendly blockers, since attacks do not
                                // descriminate by color
        while (attacks > 0)     // loop over all possible moves and add to
                            // legalmoves if move does not put player in check
        {
            to = PopLsb(attacks);
            move = Move(from, to);
            engine.MakeSimpleMove(move);
            if (MoveGenerator::IsSquareAttacked(engine, kingIndex, ncolor)) {
                engine.UndoLastSimpleMove();
                continue;
            }
            engine.UndoLastSimpleMove();
            moveHolder[moveHolderIndex] = move;
            moveHolderIndex++;
        }
    }
    // -------------------------------------------------------------------------------------------------------------------

    // Bishops
    // ---------------------------------------------------------------------------------------------------------------------
    bitboard bishopBoard =
        currentBoard.pieceBoards[BISHOP + colorIndex] |
        currentBoard.pieceBoards[QUEEN + colorIndex]; // Diagonal slider board
                                                      // (bishop + queen)

    while (bishopBoard > 0) // Loop over all diagonal sliders
    {
        from = PopLsb(bishopBoard);
        attacks = (GetDiagonalAttacks(from, combinedColors) |
                   GetAntiDiagonalAttacks(from, combinedColors)) &
                  notThisBoard; // Fetch kindergarten attacks on diagonal and
                                // anti diagonal
        while (attacks > 0)     // loop over all possible moves and add to
                            // legalmoves if move does not put player in check
        {
            to = PopLsb(attacks);
            move = Move(from, to);
            engine.MakeSimpleMove(move);
            if (MoveGenerator::IsSquareAttacked(engine, kingIndex, ncolor)) {
                engine.UndoLastSimpleMove();
                continue;
            }
            engine.UndoLastSimpleMove();
            moveHolder[moveHolderIndex] = move;
            moveHolderIndex++;
        }
    }
    // -----------------------------------------------------------------------------------------------------------------------
}

void MoveGenerator::GetLegalCaptures(Engine &engine,
                                     std::array<move, 256> &moveHolder,
                                     uint &moveHolderIndex) {
    // Initialisation of some helpful quantities shared by all pieces
    Board &currentBoard = engine.CurrentBoard(); // Reference to current board
    auto color = (currentBoard.flags & 1) ==
                 1; // Bool stating who is to move: true white, false black
    auto ncolor = !color; // ~color
    auto colorIndex =
        color
            ? WHITEPIECES
            : BLACKPIECES; // Index offset into piece boards depending on color
    int kingIndex =
        BitScanForwards(currentBoard.pieceBoards[KING + colorIndex]) -
        1; // Square of friendly king, if there is no friendly king (-1), then
           // there are no legal moves
    if (kingIndex < 0) {
        moveHolderIndex = 0;
        return;
    }

    auto promoterRank =
        color ? 6 : 1; // Promotion rank of pawns depending on color
    bitboard combinedColors = currentBoard.colorBoards[color] |
                              currentBoard.colorBoards[ncolor]; // All pieces
    bitboard notThisBoard =
        ~currentBoard.colorBoards[ncolor]; //  ~friendly blockers
    bitboard otherBoard = currentBoard.colorBoards[color]; // Enemy pieces

    // Predeclaration to save time in main function
    square from;
    square to;
    bitboard attacks;
    move move;

    // Pawns
    //----------------------------------------------------------------------------------
    bitboard capturer =
        currentBoard.pieceBoards[colorIndex] &
        ~rankMasks[promoterRank]; // Pawns which will capture and not promote
    bitboard promotionCapturer =
        currentBoard.pieceBoards[colorIndex] &
        rankMasks[promoterRank]; // Pawn which will capture and promote

    // Captures without promotion
    while (capturer > 0) // Add all caputes to legalmoves if the moves does not
                         // put player in check
    {
        from = PopLsb(capturer);
        attacks =
            pawnAttacks[ncolor][0][from] &
            (otherBoard |
             currentBoard.ghostBoard); // Test if captures enemy piece or ghost
        if (attacks > 0) {
            move = Move(from, PopLsb(attacks));
            engine.MakeSimpleMove(move);
            if (!MoveGenerator::IsSquareAttacked(engine, kingIndex, ncolor)) {
                moveHolder[moveHolderIndex] = move;
                moveHolderIndex++;
            }
            engine.UndoLastSimpleMove();
        }
        attacks = pawnAttacks[ncolor][1][from] &
                  (otherBoard | currentBoard.ghostBoard);
        if (attacks > 0) {
            move = Move(from, PopLsb(attacks));
            engine.MakeSimpleMove(move);
            if (MoveGenerator::IsSquareAttacked(engine, kingIndex, ncolor)) {
                engine.UndoLastSimpleMove();
                continue;
            }
            engine.UndoLastSimpleMove();
            moveHolder[moveHolderIndex] = move;
            moveHolderIndex++;
        }
    }

    // Capture with promotion
    while (promotionCapturer >
           0) // Add all captures that land on promotions squares to legalmoves
              // if the move does not put the player in check
    {
        from = PopLsb(promotionCapturer);
        attacks = pawnAttacks[ncolor][0][from] & otherBoard;
        if (attacks > 0) {
            move = Move(from, PopLsb(attacks));
            engine.MakeSimpleMove(move);
            if (!MoveGenerator::IsSquareAttacked(engine, kingIndex, ncolor)) {
                for (uint8_t i = KNIGHT; i < KING;
                     i++) // If move is valid loop over all possible promotions
                {
                    moveHolder[moveHolderIndex] = Move(move, i, 1);
                    moveHolderIndex++;
                }
            }
            engine.UndoLastSimpleMove();
        }
        attacks = pawnAttacks[ncolor][1][from] & otherBoard;
        if (attacks > 0) {
            move = Move(from, PopLsb(attacks));
            engine.MakeSimpleMove(move);
            if (MoveGenerator::IsSquareAttacked(engine, kingIndex, ncolor)) {
                engine.UndoLastSimpleMove();
                continue;
            }
            engine.UndoLastSimpleMove();
            for (uint8_t i = KNIGHT; i < KING;
                 i++) // If move is valid loop over all possible promotions
            {
                moveHolder[moveHolderIndex] = Move(move, i, 1);
                moveHolderIndex++;
            }
        }
    }
    // ---------------------------------------------------------------------------------------------------------------------------

    // Knights
    // --------------------------------------------------------------------------------------------------------------------------
    bitboard knights =
        currentBoard.pieceBoards[KNIGHT + colorIndex]; // Knight bitboard
    while (knights > 0)                                // Loop over all knights
    {
        from = PopLsb(knights);
        attacks = knightMoves[from] & notThisBoard &
                  otherBoard; // Get knight moves that do not land on friendly
                              // pieces and caputre an enemy piece
        while (attacks > 0)   // Loop over all possible moves and add move to
                            // legalmoves if it does not put the player in check
        {
            to = PopLsb(attacks);
            move = Move(from, to);
            engine.MakeSimpleMove(move);
            if (MoveGenerator::IsSquareAttacked(engine, kingIndex, ncolor)) {
                engine.UndoLastSimpleMove();
                continue;
            }
            engine.UndoLastSimpleMove();
            moveHolder[moveHolderIndex] = move;
            moveHolderIndex++;
        }
    }
    // -----------------------------------------------------------------------------------------------------------------------------

    // King
    //  ----------------------------------------------------------------------------------------------------------------------------------

    // normal king moves
    attacks = kingMoves[kingIndex] & notThisBoard &
              otherBoard; // Get king moves that do not land on friendly pieces
                          // and capture an enemy piece
    while (attacks > 0)   // Loop over all possibles moves and add to legalmoves
                          // if move does not put player in check
    {
        to = PopLsb(attacks);
        move = Move(kingIndex, to);
        engine.MakeSimpleMove(move);
        if (IsSquareAttacked(engine, to, ncolor)) {
            engine.UndoLastSimpleMove();
            continue;
        }
        engine.UndoLastSimpleMove();
        moveHolder[moveHolderIndex] = move;
        moveHolderIndex++;
    }
    // ---------------------------------------------------------------------------------------------

    // Rooks
    // -------------------------------------------------------------------------------------------------
    bitboard rookBoard =
        currentBoard.pieceBoards[ROOK + colorIndex] |
        currentBoard
            .pieceBoards[QUEEN + colorIndex]; // Horizontal and vertical slider
                                              // board (rook + queen)

    while (rookBoard > 0) // Loop over all horizontal and vertical sliders
    {
        from = PopLsb(rookBoard);

        attacks =
            (GetFileAttacks(from, combinedColors) |
             GetRankAttacks(from, combinedColors)) &
            notThisBoard &
            otherBoard; // Fetch kindergarten file attacks and removed friendly
                        // blockers, since attacks do not descriminate by color,
                        // only consider moves that capture a piece
        while (attacks > 0) // loop over all possible moves and add to
                            // legalmoves if move does not put player in check
        {
            to = PopLsb(attacks);
            move = Move(from, to);
            engine.MakeSimpleMove(move);
            if (MoveGenerator::IsSquareAttacked(engine, kingIndex, ncolor)) {
                engine.UndoLastSimpleMove();
                continue;
            }
            engine.UndoLastSimpleMove();
            moveHolder[moveHolderIndex] = move;
            moveHolderIndex++;
        }
    }
    // -------------------------------------------------------------------------------------------------------------------

    // Bishops
    // ---------------------------------------------------------------------------------------------------------------------
    bitboard bishopBoard =
        currentBoard.pieceBoards[BISHOP + colorIndex] |
        currentBoard.pieceBoards[QUEEN + colorIndex]; // Diagonal slider board
                                                      // (bishop + queen)

    while (bishopBoard > 0) // Loop over all diagonal sliders
    {
        from = PopLsb(bishopBoard);
        attacks = (GetDiagonalAttacks(from, combinedColors) |
                   GetAntiDiagonalAttacks(from, combinedColors)) &
                  notThisBoard &
                  otherBoard; // Fetch kindergarten attacks on diagonal and anti
                              // diagonal, only consider moves that capture an
                              // enemy piece
        while (attacks > 0)   // loop over all possible moves and add to
                              // legalmoves if move does not put player in check
        {
            to = PopLsb(attacks);
            move = Move(from, to);
            engine.MakeSimpleMove(move);
            if (MoveGenerator::IsSquareAttacked(engine, kingIndex, ncolor)) {
                engine.UndoLastSimpleMove();
                continue;
            }
            engine.UndoLastSimpleMove();
            moveHolder[moveHolderIndex] = move;
            moveHolderIndex++;
        }
    }
    // -----------------------------------------------------------------------------------------------------------------------
}

bool MoveGenerator::IsSquareAttacked(Engine &engine, const int &index,
                                     const bool &attackingColor) {
    if (index < 0) {
        return false;
    }
    // Initialisation of some helpful quantities shared by all pieces
    Board &currentBoard = engine.CurrentBoard(); // Reference to current board
    auto colorIndex = attackingColor
                          ? WHITEPIECES
                          : BLACKPIECES; // Index offset into piece boards
                                         // depending on attacking color

    // Bitboards containing slider pieces
    bitboard horAndVertSliderBoard =
        currentBoard.pieceBoards[ROOK + colorIndex] |
        currentBoard.pieceBoards[QUEEN + colorIndex];
    bitboard diagonalSliderBoard =
        currentBoard.pieceBoards[BISHOP + colorIndex] |
        currentBoard.pieceBoards[QUEEN + colorIndex];

    // Bitboards containing all pieces which cant attack by sliding
    bitboard horAndVertBlockerBoard =
        (engine.gameHistory[engine.gameHistoryIndex]
             .colorBoards[!attackingColor] &
         ~engine.gameHistory[engine.gameHistoryIndex]
              .pieceBoards[ROOK + colorIndex] &
         ~engine.gameHistory[engine.gameHistoryIndex]
              .pieceBoards[QUEEN + colorIndex]) |
        engine.CurrentBoard().colorBoards[attackingColor];
    bitboard diagonalBlockerBoard =
        (engine.gameHistory[engine.gameHistoryIndex]
             .colorBoards[!attackingColor] &
         ~engine.gameHistory[engine.gameHistoryIndex]
              .pieceBoards[BISHOP + colorIndex] &
         ~engine.gameHistory[engine.gameHistoryIndex]
              .pieceBoards[QUEEN + colorIndex]) |
        engine.CurrentBoard().colorBoards[attackingColor];

    // Diagonal sliders: can the piece on index attack a attackingcolor diagonal
    // slider like a diagonal slider
    bitboard attacks = (GetDiagonalAttacks(index, diagonalBlockerBoard) |
                        GetAntiDiagonalAttacks(index, diagonalBlockerBoard)) &
                       diagonalSliderBoard;
    if (attacks > 0) {
        return true;
    }

    // Knights: can the piece on index attack a attackingcolor knight like a
    // knight
    attacks =
        knightMoves[index] & currentBoard.pieceBoards[KNIGHT + colorIndex];
    if (attacks > 0) {
        return true;
    }

    // Horizontal and vertical sliders: can the piece on index attack a
    // attacking color hor and vert slider like a hor and vert slider
    attacks = (GetFileAttacks(index, horAndVertBlockerBoard) |
               GetRankAttacks(index, horAndVertBlockerBoard)) &
              horAndVertSliderBoard;
    if (attacks > 0) {
        return true;
    }

    // Pawns: can the piece on index attack an attackingcolor pawn like a pawn
    attacks = (pawnAttacks[attackingColor][0][index] |
               pawnAttacks[attackingColor][1][index]) &
              currentBoard.pieceBoards[colorIndex];
    if (attacks > 0) {
        return true;
    }

    // King: can the piece on index attack a attackingcolor king like a king
    attacks = kingMoves[index] & currentBoard.pieceBoards[KING + colorIndex];
    if (attacks > 0) {
        return true;
    }

    return false;
}

void MoveGenerator::PreComputeMoves() {
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
void PreComputeKnightMoves() {
    bitboard localMoves = 0;
    for (int i = A8; i <= H1; i++) {
        localMoves = ONE << i;
        localMoves = (localMoves & ~fileMasks[H]) << SOSOEA |
                     (localMoves & ~(fileMasks[G] | fileMasks[H])) << SOEAEA |
                     (localMoves & ~(fileMasks[G] | fileMasks[H])) >> NOEAEA |
                     (localMoves & ~fileMasks[H]) >> NONOEA |
                     (localMoves & ~fileMasks[A]) >> SOSOEA |
                     (localMoves & ~(fileMasks[A] | fileMasks[B])) >> SOEAEA |
                     (localMoves & ~(fileMasks[A] | fileMasks[B])) << NOEAEA |
                     (localMoves & ~fileMasks[A]) << NONOEA;
        knightMoves[i] = localMoves;
    }
}

void PreComputeKingMoves() {
    bitboard localMoves = 0;
    for (int i = A8; i <= H1; i++) {
        localMoves = ONE << i;
        localMoves = localMoves << NO | (localMoves & ~fileMasks[H]) << SOEA |
                     (localMoves & ~fileMasks[H]) << EA |
                     (localMoves & ~fileMasks[H]) >> NOEA | localMoves >> NO |
                     (localMoves & ~fileMasks[A]) >> SOEA |
                     (localMoves & ~fileMasks[A]) >> EA |
                     (localMoves & ~fileMasks[A]) << NOEA;
        kingMoves[i] = localMoves;
    }
}

void PreComputePawnAttacks() {
    bitboard localMoves = 0;
    for (int i = A8; i <= H1; i++) {
        localMoves = ONE << i;
        // white left
        pawnAttacks[0][0][i] = (localMoves & ~fileMasks[A]) >> SOEA;
        // white right
        pawnAttacks[0][1][i] = (localMoves & ~fileMasks[H]) >> NOEA;
        // black left
        pawnAttacks[1][0][i] = (localMoves & ~fileMasks[A]) << NOEA;
        // black right
        pawnAttacks[1][1][i] = (localMoves & ~fileMasks[H]) << SOEA;
    }
}

void PreComputeFillUpAttacks() {
    bitboard localmoves = 0;
    for (int file = A; file <= H; file++) // loop over all 8 files
    {
        for (bitboard occ = 0; occ < 64;
             occ++) // loop over all 64 blocker configurations (blockers on the
                    // border do not count)
        {
            bitboard left = 0;
            bitboard right = 0;
            bitboard decompressedOcc = occ << 1; // get 8 bit occupation
            if (file < H) {
                left = 1 << (file + 1); // left rank direction
                for (auto shift = 0; shift < 6 - file;
                     shift++) // loop till the end of the rank
                {
                    left |= (left & ~decompressedOcc)
                            << EA; // kind of flood fill; tries to push bits
                                   // onto squares but if blocker is on square,
                                   // no bits can pass the blocker
                }
            }
            if (file > A) {
                right = 1 << (file - 1); // right rank direction
                for (auto shift = 0; shift < file; shift++) {
                    right |= (right & ~decompressedOcc) >> EA;
                }
            }
            left |= right; // combine left and right
            localmoves = left;
            localmoves |= localmoves << NO | localmoves << 2 * NO |
                          localmoves << 3 * NO | localmoves << 4 * NO |
                          localmoves << 5 * NO | localmoves << 6 * NO |
                          localmoves << 7 * NO; // create 8 copies
            fillUpAttacks[file][occ] = localmoves;
        }
    }
}

void PreComputeAFileAttacks() {
    for (auto rank = 0; rank < 8; rank++) // loop over all 8 ranks
    {
        for (bitboard occ = 0; occ < 64;
             occ++) // loop over all 64 blocker configurations (blockers on the
                    // border do not count)
        {
            bitboard up = 0;
            bitboard down = 0;
            bitboard decompressedOcc = 0;
            for (auto i = 1; i < 8; i++) {
                decompressedOcc |=
                    (occ & fileMasks[i - 1])
                    << (i * 8 -
                        (i - 1)); // Convert occupation into file occupation
            }
            if (rank < 7) {
                up = ONE << (rank + 1) * 8;
                for (auto shift = 0; shift < 6 - rank; shift++) {
                    up |= (up & ~decompressedOcc) << 8;
                }
            }
            if (rank > 0) {
                down = ONE << (rank - 1) * 8;
                for (auto shift = 0; shift < rank; shift++) {
                    down |= (down & ~decompressedOcc) >> 8;
                }
            }
            up |= down;
            aFileAttacks[rank][occ] = up;
        }
    }
}

// Fetch kindergarten slider attacks
bitboard GetDiagonalAttacks(const int &index, const bitboard &occ) {
    bitboard compressedOcc =
        ((occ & diagonalAttackMasks[index]) * fileMasks[1]) >> 58;
    return fillUpAttacks[index & 7][compressedOcc] & diagonalAttackMasks[index];
}

bitboard GetAntiDiagonalAttacks(const int &index, const bitboard &occ) {
    bitboard compressedOcc =
        ((occ & antiDiagonalAttackMasks[index]) * fileMasks[1]) >> 58;
    return fillUpAttacks[index & 7][compressedOcc] &
           antiDiagonalAttackMasks[index];
}

bitboard GetRankAttacks(const int &index, const bitboard &occ) {
    bitboard compressedOcc =
        ((occ & rankAttackMasks[index]) * fileMasks[1]) >> 58;
    return fillUpAttacks[index & 7][compressedOcc] & rankAttackMasks[index];
}

bitboard GetFileAttacks(const int &index, const bitboard &occ) {
    bitboard compressedOcc = fileMasks[0] & (occ >> (index & 7));
    compressedOcc = (antiDiac7h2 * compressedOcc) >> 58;
    return aFileAttacks[index >> 3][compressedOcc] << (index & 7);
}
// -------------------------------------------------------------------
