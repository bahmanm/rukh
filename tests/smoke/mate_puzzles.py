#!/usr/bin/env -S uv run --script
# /// script
# requires-python = ">=3.8"
# dependencies = ["chess"]
# ///

import sys

import chess
import chess.engine


PUZZLES = [
    (
        "mate in 1 (black)",
        "8/2p5/p4k2/1p2Q2p/1P1KP3/P1P2r2/7q/8 b - - 0 1",
        1,
        ["h2e5"],
    ),
    (
        "mate in 1 (white)",
        "r1b1k2r/ppp1bppp/8/4Q3/2P1n3/BP1q4/P2P1PPP/RN2K2R w KQkq - 0 1",
        1,
        ["e5e7"],
    ),
    (
        "mate in 2 (queen sacrifice)",
        "r3kbnr/p1p2p1p/1pp5/3qp3/8/1QP3PN/PP1PPP1P/RNB1K2R b KQkq - 0 1",
        2,
        ["d5h1", "h3g1", "h1g1"],
    ),
    (
        "mate in 2 (king hunt)",
        "B6r/3Q1pkp/6p1/8/3P1Pn1/4P3/3q1N1P/R3R1K1 b - - 0 1",
        2,
        ["d2f2", "g1h1", "f2h2"],
    ),
    (
        "mate in 2 (queen check)",
        "4Q3/pp3Npk/4R2p/1p6/q7/7P/5PPK/3r4 b - - 0 1",
        2,
        ["a4f4", "g2g3", "f4f2"],
    ),
    (
        "mate in 2 (rook exchange)",
        "r3r1k1/2p2ppp/p7/4R2Q/Pp1q1n2/6N1/P4PPP/4R1K1 w - - 0 1",
        2,
        ["e5e8", "a8e8", "e1e8"],
    ),
    (
        "mate in 3 (queen exchange)",
        "r2qr1k1/1p3ppp/3p4/1ppP4/6b1/P1P1R3/1P2QPPP/4R1K1 w - - 0 1",
        3,
        ["e3e8", "d8e8", "e2e8", "a8e8", "e1e8"],
    ),
    (
        "mate in 3 (queen and rook)",
        "4r1k1/Q5pp/6p1/8/4q3/4P3/PP4PP/5RK1 w - - 0 1",
        3,
        ["a7f7", "g8h8", "f7f8", "e8f8", "f1f8"],
    ),
]


def run_puzzles(executable):
    with chess.engine.SimpleEngine.popen_uci(executable, timeout=60) as engine:
        engine.configure({"Threads": 1, "Hash": 16})
        for name, fen, mate_in, solution in PUZZLES:
            board = chess.Board(fen)
            result = engine.analyse(board, chess.engine.Limit(depth=10))
            bestmove = result["pv"][0]
            if bestmove != chess.Move.from_uci(solution[0]):
                raise AssertionError(f"{name}: expected bestmove {solution[0]}, got {bestmove}")

            mate_score = result["score"].pov(board.turn).mate()
            if mate_score != mate_in:
                raise AssertionError(f"{name}: expected mate score {mate_in}, got {mate_score}")

            for move in solution:
                board.push_uci(move)
            if not board.is_checkmate():
                raise AssertionError(f"{name}: supplied solution does not end in checkmate")

            print(f"PASS {name}: {bestmove}, mate {mate_in}; supplied line reaches checkmate")


if __name__ == "__main__":
    if len(sys.argv) != 2:
        raise SystemExit("usage: mate_puzzles.py ENGINE")
    run_puzzles(sys.argv[1])
