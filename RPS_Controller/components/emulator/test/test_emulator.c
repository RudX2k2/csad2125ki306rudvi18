#include "unity.h"
#include "../include/emulator.h"
#include "../include/emulator_private.h"
#include "esp_log.h"
#include <string.h>

void test_rock_beats_scissors(void);
void emulator_reset(void);

emulator_t emulator = {
    .current_round = 0,
    .players_score[EMULATOR_PLAYER_1] = 0,
    .players_score[EMULATOR_PLAYER_2] = 0,
};

// Implement the test function
TEST_CASE("Rock beats Scissors", "[emulator]")
{
    emulator_reset(); // Reset state before test

    emulator.players_choice[EMULATOR_PLAYER_1] = EMULATOR_CHOICE_ROCK;
    emulator.players_choice[EMULATOR_PLAYER_2] = EMULATOR_CHOICE_SCISSORS;

    emulator_turnresult_winner_t result = EMULATOR_ChoseWinner();

    TEST_ASSERT_EQUAL_INT(EMULATOR_TURNRESULT_WINNER_ROUND_FIRST, result);
    TEST_ASSERT_EQUAL_INT(1, emulator.players_score[EMULATOR_PLAYER_1]);
    TEST_ASSERT_EQUAL_INT(0, emulator.players_score[EMULATOR_PLAYER_2]);
}

void emulator_reset(void)
{
    emulator.current_round = 0;
    emulator.game_state = EMULATOR_GAMESTATE_WAIT_CONFIG;
    emulator.players_score[EMULATOR_PLAYER_1] = 0;
    emulator.players_score[EMULATOR_PLAYER_2] = 0;
    memset(emulator.players_choice, 0, sizeof(emulator.players_choice));
}

TEST_CASE("Rock beats Scissors", "[emulator]")
{
    emulator_reset(); // Reset state before test

    emulator.players_choice[EMULATOR_PLAYER_1] = EMULATOR_CHOICE_ROCK;
    emulator.players_choice[EMULATOR_PLAYER_2] = EMULATOR_CHOICE_SCISSORS;

    emulator_turnresult_winner_t result = EMULATOR_ChoseWinner();

    TEST_ASSERT_EQUAL_INT(EMULATOR_TURNRESULT_WINNER_ROUND_FIRST, result);
    TEST_ASSERT_EQUAL_INT(1, emulator.players_score[EMULATOR_PLAYER_1]);
    TEST_ASSERT_EQUAL_INT(0, emulator.players_score[EMULATOR_PLAYER_2]);
}

TEST_CASE("Scissors beats Paper", "[emulator]")
{
    // Scissors beats Paper
    emulator.players_choice[EMULATOR_PLAYER_1] = EMULATOR_CHOICE_SCISSORS;
    emulator.players_choice[EMULATOR_PLAYER_2] = EMULATOR_CHOICE_PAPER;

    emulator_turnresult_winner_t result = EMULATOR_ChoseWinner();

    TEST_ASSERT_EQUAL_INT(EMULATOR_TURNRESULT_WINNER_ROUND_FIRST, result);
    TEST_ASSERT_EQUAL_INT(1, emulator.players_score[EMULATOR_PLAYER_1]);
    TEST_ASSERT_EQUAL_INT(0, emulator.players_score[EMULATOR_PLAYER_2]);
}

TEST_CASE("Paper beats Rock", "[emulator]")
{
    // Paper beats Rock
    emulator.players_choice[EMULATOR_PLAYER_1] = EMULATOR_CHOICE_PAPER;
    emulator.players_choice[EMULATOR_PLAYER_2] = EMULATOR_CHOICE_ROCK;

    emulator_turnresult_winner_t result = EMULATOR_ChoseWinner();

    TEST_ASSERT_EQUAL_INT(EMULATOR_TURNRESULT_WINNER_ROUND_FIRST, result);
    TEST_ASSERT_EQUAL_INT(1, emulator.players_score[EMULATOR_PLAYER_1]);
    TEST_ASSERT_EQUAL_INT(0, emulator.players_score[EMULATOR_PLAYER_2]);
}

TEST_CASE("Draw Round", "[emulator]")
{
    // Test draw scenario
    emulator.players_choice[EMULATOR_PLAYER_1] = EMULATOR_CHOICE_ROCK;
    emulator.players_choice[EMULATOR_PLAYER_2] = EMULATOR_CHOICE_ROCK;

    emulator_turnresult_winner_t result = EMULATOR_ChoseWinner();

    TEST_ASSERT_EQUAL_INT(EMULATOR_TURNRESULT_WINNER_ROUND_DRAW, result);
    TEST_ASSERT_EQUAL_INT(0, emulator.players_score[EMULATOR_PLAYER_1]);
    TEST_ASSERT_EQUAL_INT(0, emulator.players_score[EMULATOR_PLAYER_2]);
}

TEST_CASE("Last Round First Player Wins", "[emulator]")
{
    // Simulate last round with first player winning
    emulator.current_round = emulator.max_rounds - 1;
    emulator.players_score[EMULATOR_PLAYER_1] = 2;
    emulator.players_score[EMULATOR_PLAYER_2] = 1;

    emulator.players_choice[EMULATOR_PLAYER_1] = EMULATOR_CHOICE_ROCK;
    emulator.players_choice[EMULATOR_PLAYER_2] = EMULATOR_CHOICE_SCISSORS;

    emulator_turnresult_winner_t result = EMULATOR_ChoseWinner();

    TEST_ASSERT_EQUAL_INT(EMULATOR_TURNRESULT_WINNER_GAME_FIRST, result);
}

TEST_CASE("Last Round Draw", "[emulator]")
{
    // Simulate last round with a draw
    emulator.current_round = emulator.max_rounds - 1;
    emulator.players_score[EMULATOR_PLAYER_1] = 1;
    emulator.players_score[EMULATOR_PLAYER_2] = 1;

    emulator.players_choice[EMULATOR_PLAYER_1] = EMULATOR_CHOICE_ROCK;
    emulator.players_choice[EMULATOR_PLAYER_2] = EMULATOR_CHOICE_ROCK;

    emulator_turnresult_winner_t result = EMULATOR_ChoseWinner();

    TEST_ASSERT_EQUAL_INT(EMULATOR_TURNRESULT_WINNER_DRAW, result);
}

// void app_main(void)
// {
//     // This is required by ESP-IDF for the test runner
//     UNITY_BEGIN();
//     // Tests are automatically run by the Unity test framework
//     UNITY_END();
// }
