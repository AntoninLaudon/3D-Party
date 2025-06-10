#pragma once

#include "IGame.hpp"

namespace Game {

// ################################################################
// ######  GAME TOOLS  ############################################
// ################################################################

bool areGamesNamesUnique(Game::IGame *games[], int numGames) {
    for (int i = 0; i < numGames; i++) {
        for (int j = i + 1; j < numGames; j++) {
            if (games[i]->getName() == games[j]->getName()) return false;
        }
    }
    return true;
}

} // namespace Game