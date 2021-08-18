import subprocess
import time
#NOTE: chess ai needs to compiled in a particular way for this to work
def perft_test(fen: str, max_depth: int, depth_results: list[int]):
    for i in range(max_depth):
        proc = subprocess.run(["/home/cryptic/Coding/wxChess/bin/chessAiRewritev2", fen, str(i + 1)], stdout=subprocess.PIPE, text=True)
        num = proc.stdout
        if int(num) == depth_results[i]:
            print("Depth", i + 1, "passed (", int(num), ")")
        else:
            print("Depth", i + 1, "failed (", int(num), "vs expected", depth_results[i], ")")

def main():
    # from https://www.chessprogramming.org/Perft_Results
    print("Starting position:")
    start = time.time()
    perft_test("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq -", 6, [20, 400, 8902, 197281, 4865609, 119060324]) # starting position
    end = time.time()
    print("Time:", end - start)

    print("\nPosition 2:")
    start = time.time()
    perft_test("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq -", 5, [48, 2039, 97862, 4085603, 193690690, 8031647685]) # "position 2"
    end = time.time()
    print("Time:", end - start)

    print("\nPosition 3:")
    start = time.time()
    perft_test("8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - -", 6, [14, 191, 2812, 43238, 674624, 11030083])
    end = time.time()
    print("Time:", end - start)

    print("\nPosition 4:")
    start = time.time()
    perft_test("r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1RK1 w kq -", 6, [6, 264, 9467, 422333, 15833292, 706045033])
    end = time.time()
    print("Time:", end - start)

    print("\nPosition 5:")
    start = time.time()
    perft_test("rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ -", 5, [44, 1486, 62379, 2103487, 89941194])
    end = time.time()
    print("Time:", end - start)

    print("\nPosition 6:")
    start = time.time()
    perft_test("r4rk1/1pp1qppp/p1np1n2/2b1p1B1/2B1P1b1/P1NP1N2/1PP1QPPP/R4RK1 w - -", 5, [46, 2079, 89890, 3894594, 164075551, 6923051137])
    end = time.time()
    print("Time:", end - start)

if __name__ == "__main__":
    main()