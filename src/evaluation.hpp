//
// Created by metta on 11/17/24.
//

#ifndef EVALUATION_HPP
#define EVALUATION_HPP

#include "board.hpp"

class Evaluation {
    double eval(const Board& board) {
        bool color = board.getCurrentPlayer();
        double myMaterial = board.getMaterial(color);
        double enemyMaterial = board.getMaterial(!color);
        double materialDifference = myMaterial - enemyMaterial;

        return materialDifference - 0.5 * (board.doubledPawns(color) - board.doubledPawns(!color));
    }
};



#endif //EVALUATION_HPP
