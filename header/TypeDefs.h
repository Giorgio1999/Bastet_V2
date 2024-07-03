#pragma once
#include <cstdint>
#include <string>

// Declaration of types
// ----------------------------------------
using bitboard = uint64_t;   // 64 bits representing the squares of a chessboard
using line = uint8_t;        // 8 bits representing one line of the board
using move = uint16_t;       // 16 bits for representing moves
                             // (promotion:1,convertTo:3,targetIndex:6,startIndex:6)
using flag = uint8_t;        // 8 bits for bundling together different booleans
using square = uint_fast8_t; // 8 bits for representing a specific square
// -----------------------------------------

// Definiton of helpful constants
// -----------------------------------------
const bitboard ONE = (bitboard)1;
const bitboard ZERO = (bitboard)0;
const flag PTM = (flag)1;            // Player to move
const flag KCW = (flag)0b00000010;   // Kingside castling white
const flag QCW = (flag)0b00000100;   // Queenside castling white
const flag KCB = (flag)0b00001000;   // Kingside castling black
const flag QCB = (flag)0b00010000;   // Queenside castling black
const flag nKCW = (flag)0b00011101;  // ~KCW
const flag nQCW = (flag)0b00011011;  // ~QCW
const flag nKCB = (flag)0b00010111;  // ~KCB
const flag nQCB = (flag)0b00001111;  // ~QCB
const move PROMOTION = (move)0x8000; // promotion
const move CNVTO = (move)0x7000;     // convert to
const move START = (move)0x003F;     // Start index
const move TARGET = (move)0x0FC0;    // Target index
const int NO = 8;                    // Offset for north (1 -> 8) movement
const int NONO = 16;                 // Offset for 2north movement
const int EA = 1;                    // Offset for east (a -> h) movement
const int EAEA = 2;                  // Offset for 2east movement
const int NOEA = 7;                  // Offset for northeast (a1 -> h8) movement
const int SOEA = 9;                  // Offset for southeast (a8 -> h1) movement
const int NONOEA = 15;               // Offset for moving like a knight 2north 1east
const int NOEAEA = 6;                // Offset for moving like a knight 1north 2east
const int SOEAEA = 10;               // Offset for moving like a knight 1south 2east
const int SOSOEA = 17;               // Offset for moving like a knight 2south 1east
const int WHITEPIECES = 0;           // Offset of white pieces in pieceboards
const int BLACKPIECES = 6;           // Offset of black pieces in pieceboard
// -----------------------------------------

// Declaration of lookup tables
// -----------------------------------------
extern bitboard fileMasks[8];
extern bitboard rankMasks[8];
extern bitboard diagonalAttackMasks[64];
extern bitboard antiDiagonalAttackMasks[64];
extern bitboard rankAttackMasks[64];
extern bitboard hashes[781]; // Hash table for: pieces*square + black to move +
                             // castling rights + enpassant file

extern bitboard knightMoves[64];       // lookup table for all knight moves indexed by: square
extern bitboard kingMoves[64];         // lookup table for all king moves indexed by: square
extern bitboard pawnAttacks[2][2][64]; // lookup table for all pawn attackes
                                       // indexed by: color,direction,index
extern bitboard fillUpAttacks[8][64];  // lookup table for fill up attacks for kindergarten
                                       // bitboards indexed by: file,occupation, contains
                                       // bitboards consisting of 8 copies of first rank
                                       // attacks for the given file and occupation
extern bitboard aFileAttacks[8][64];   // lookup table for attacks on the a file
                                       // indexed by: rank,occupation
// -----------------------------------------

// Constants to convert between representation and human readable notation of
// squares and piecetypes
// -----------------------------------------------------------
const std::string rows = "87654321";      // Hardcoded translations of the rows or y
const std::string cols = "abcdefgh";      // Hardcoded translations of the cols or x
const std::string types = "-pnbrqk";      // Hardcoded translations from int to piecetypes
const std::string whiteTypes = "PNBRQKG"; // Fen representation
const std::string blackTypes = "pnbrqkg";
// -----------------------------------------------------------

// Starting positions
// -------------------------------------------------------------------
const std::string startingPosition
    = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"; // hardcoded
                                                                  // starting
                                                                  // position
// const std::string startingPosition = "2b5/1p4k1/p2R2P1/4Np2/1P3Pp1/1r6/5K2/8
// w - - 0 1"; // hardcoded starting position const std::string startingPosition
// = "8/k7/8/8/8/8/3B3p/3K24 b - - 0 1"; // illegal move d2c3 const std::string
// startingPosition = "8/3k1p2/p1pP4/1p2P3/1P6/P7/3B3b/3K3q"; const std::string
// startingPosition = "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w
// KQkq - 0 1"; //temporary testing const std::string startingPosition =
// "r7/7k/8/8/8/8/8/K7 w - - 0 1"; const std::string startingPosition =
// "r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1RK1 w kq - 0 1"; const
// std::string startingPosition =
// "rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ - 1 8"; const
// std::string startingPosition = "8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - 0 1";
// -------------------------------------------------------------------

// Fen notation constants
// -------------------------------------------------------------------
const std::string whitePieceCodings = "pnbrqk";
const std::string blackPieceCodings = "PNBRQK";
// -------------------------------------------------------------------

// Constant masks
// -------------------------------------------------------------------
const bitboard castleMasks[2][2]
    = { { 0x6000000000000000, 0x0E00000000000000 },
        { 0x0000000000000060, 0x000000000000000E } }; // white kingside    whitequeenside      black
                                                      // kingside      black queenside
const bitboard antiDiac7h2
    = 0x0204081020408000; // anti Diagonal c7h2
                          // -------------------------------------------------------------------

// Enums
// -------------------------------------------------------------------
enum Square
{
    A8,
    B8,
    C8,
    D8,
    E8,
    F8,
    G8,
    H8,
    A7,
    B7,
    C7,
    D7,
    E7,
    F7,
    G7,
    H7,
    A6,
    B6,
    C6,
    D6,
    E6,
    F6,
    G6,
    H6,
    A5,
    B5,
    C5,
    D5,
    E5,
    F5,
    G5,
    H5,
    A4,
    B4,
    C4,
    D4,
    E4,
    F4,
    G4,
    H4,
    A3,
    B3,
    C3,
    D3,
    E3,
    F3,
    G3,
    H3,
    A2,
    B2,
    C2,
    D2,
    E2,
    F2,
    G2,
    H2,
    A1,
    B1,
    C1,
    D1,
    E1,
    F1,
    G1,
    H1
};

enum File
{
    A,
    B,
    C,
    D,
    E,
    F,
    G,
    H
};

enum PieceTypes
{
    NONE = -1,
    PAWN,
    KNIGHT,
    BISHOP,
    ROOK,
    QUEEN,
    KING
};

enum HashOffsets
{
    PIECES,
    BLACKTOMOVE = 768,
    KCWHASH,
    QCWHASH,
    KCBHASH,
    QCBHASH,
    ENPASSANT
};
// -------------------------------------------------------------------
// Macros
// -------------------------------------------------------------------
#ifndef CERR_ENABLED
#define CERR_ENABLED_VAL true
#else
#define CERR_ENABLED_VAL false
#endif
#define CERR                                                                                       \
    if (CERR_ENABLED_VAL)                                                                          \
        {                                                                                          \
        }                                                                                          \
    else                                                                                           \
        std::cerr

