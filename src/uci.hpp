//
// Created by metta on 11/19/24.
//

#ifndef UCI_HPP
#define UCI_HPP

#include <chrono>
#include "search.hpp"

#define ENGINE_NAME "Talento 0.1"

constexpr int32 MAX_DEPTH = 99;

inline Board game;

class UCI {
public:
    static void uci() {
        std::cout << "id name " << ENGINE_NAME << std::endl;
        std::cout << "id author Tiago Daniel" << std::endl;
        std::cout << "uciok" << std::endl;
    }

    static void isReady() {
        Evaluation::init_tables();
        std::cout << "readyok" << std::endl;
    }

    static void uciNewGame() {
        game = Board();
    }

    static void position(const std::vector<std::string> & strings) {

        int movesIndex = -1;

        if (strings[1] == "startpos") {
            game = Board();
            movesIndex = 2;
        }

        else if (strings[1] == "fen") {
            std::string fen = "";
            int i = 0;

            for (i = 2; i < strings.size() && strings[i] != "moves"; i++) {
                fen += strings[i] + " ";
            }

            fen.pop_back();     //remove trailing whitespace
            game = Board(fen);
            movesIndex = i;
        }

        for (int i = movesIndex + 1; i < strings.size(); i++) {
            game.makeMove(game.stringToMove(strings[i]));
        }
    }

    /**
     * @brief Execute UCI go command.
     * @param strings UCI line received separated by words.
     *
     * Code is basically copy pasted from zzzzz151/Starzix.
     *
     */
    static void go(const std::vector<std::string> & strings) {
        const std::chrono::time_point<std::chrono::steady_clock> startTime = std::chrono::steady_clock::now();

        int64 milliseconds = std::numeric_limits<int64>::max();
        [[maybe_unused]] int64 incrementMs = 0;
        [[maybe_unused]] int64 movesToGo = 0;
        bool isMoveTime = false;
        int32 maxDepth = MAX_DEPTH;
        int64 maxNodes = std::numeric_limits<int64>::max();

        for (int i = 1; i < int(strings.size()) - 1; i += 2)
        {
            const int64 value = std::max<int64>(std::stoll(strings[i + 1]), 0);

            if ((strings[i] == "wtime" && game.getCurrentPlayer() == WHITE)
            ||  (strings[i] == "btime" && game.getCurrentPlayer() == BLACK))
                milliseconds = value;

            else if ((strings[i] == "winc" && game.getCurrentPlayer() == WHITE)
            ||       (strings[i] == "binc" && game.getCurrentPlayer() == BLACK))
                incrementMs = value;

            else if (strings[i] == "movestogo")
                movesToGo = std::max<int64>(value, 1);
            else if (strings[i] == "movetime")
            {
                isMoveTime = true;
                milliseconds = value;
            }
            else if (strings[i] == "depth")
                maxDepth = value;
            else if (strings[i] == "nodes")
                maxNodes = value;
        }

        // Calculate search time limits

        int64 hardMs = std::max<int64>(0, milliseconds - 10);
        int64 softMs = std::numeric_limits<int64>::max();

        /* TIME MANAGEMENT
        if (!isMoveTime) {
            hardMs *= hardTimePercentage();
            softMs = hardMs * softTimePercentage();
        }

        const auto [bestMove, score] = Search::search(maxDepth, maxNodes, startTime, hardMs, softMs, true);
        */

        std::cout << "bestmove " << Search::rootNegaMax(game,4) << std::endl;
    }

    static bool runCommand(std::string &command) {
        const auto strings = splitString(command);
        if (strings.empty()) return true;

        if (strings[0] == "uci") {
            uci();
        }
        else if (strings[0] == "ucinewgame") {
            uciNewGame();
        }
        else if (strings[0] == "isready") {
            isReady();
        }
        else if (strings[0] == "go") {
            go(strings);
        }
        else if (strings[0] == "position") {
            position(strings);
        }
        else if (strings[0] == "quit") {
            return false;
        }
        return true;
    }
};

#endif //UCI_HPP