//
// Created by metta on 11/19/24.
//

#ifndef UCI_HPP
#define UCI_HPP

#include <thread>
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
        auto startTime = std::chrono::steady_clock::now();
        [[maybe_unused]] int64 movesToGo = 0;
        int64 maxDepth = MAX_DEPTH;
        uint64 maxNodes = std::numeric_limits<uint64>::max();
        uint64 time = std::numeric_limits<uint64>::max();

        for (int i = 1; i < int(strings.size()) - 1; i += 2)
        {
            const int64 value = std::max<int64>(std::stoll(strings[i + 1]), 0);

            if ((strings[i] == "wtime" && game.getCurrentPlayer() == WHITE)
            ||  (strings[i] == "btime" && game.getCurrentPlayer() == BLACK)) {
                time = (value / 20);
            }

            else if ((strings[i] == "winc" && game.getCurrentPlayer() == WHITE)
            ||       (strings[i] == "binc" && game.getCurrentPlayer() == BLACK))
                time += value/2;

            else if (strings[i] == "movestogo")
                movesToGo = std::max<int64>(value, 1);
            else if (strings[i] == "movetime")
            {
                time = value;
            }
            else if (strings[i] == "depth")
                maxDepth = value;
            else if (strings[i] == "nodes")
                maxNodes = value;
        }

        auto bestMove = Search::iterativeDeepening(game,maxDepth,maxNodes,startTime, time);
        std::cout << "bestmove " << bestMove << std::endl;
    }

    static void runCommands() {
        std::string command;
        std::thread searchThread;
        while (true) {
            getline(std::cin, command);
            const auto strings = splitString(command);
            if (strings.empty()) continue;

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
                if (searchThread.joinable()) {
                    stop = true;
                    searchThread.join();
                }
                stop = false;
                searchThread = std::thread(go,strings);
            }
            else if (strings[0] == "position") {
                position(strings);
            }
            else if (strings[0] == "quit") {
                stop = true;
                if (searchThread.joinable()) {
                    searchThread.join();
                }
                return;
            }
            else if (strings[0] == "stop") {
                stop = true;
                if (searchThread.joinable()) {
                    searchThread.join();
                }
            }
        }
    }
};

#endif //UCI_HPP
