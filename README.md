# Guess_The_Phrase_Game
### A console based phrase guessing game that could give you hours of enjoyment and head scratching :)

## Overview

**Guess the Phrase** is a console-based game written entirely in the C programming language. Players attempt to uncover a hidden phrase by guessing individual letters or the complete phrase before running out of attempts.

The game features multiple categories, visual themes, hints, a scoring system, a scoreboard, and a colourful console interface.

## Features

- **Multiple Categories:** Sports, Life, Fantasy, and Random.
- **Multiple Visual Themes:** Hangman, Sinking Ship, and Dying Robot.
- **Letter Guessing:** Guess individual letters to reveal them in the hidden phrase.
- **Phrase Guessing:** Attempt to guess the complete phrase at any point.
- **Limited Attempts:** Players have 7 attempts to solve the phrase.
- **Hint System:** Reveal a hint at the cost of points.
- **Scoring System:** Earn points for correct guesses and winning bonuses.
- **Scoreboard:** View your top scores from previous games.
- **Player Profiles:** Scores are stored separately for each player.
- **Random Colour Schemes:** The console interface uses randomly selected colour schemes for each round.
- **Replay Option:** Play multiple rounds without restarting the program.

## How to Play

1. Enter your player name.
2. Select an option from the main menu.
3. Choose a phrase category or select Random.
4. Select a visual theme.
5. Guess letters or the complete phrase.
6. Type `?` to reveal the hint.
7. Solve the phrase before you run out of attempts.
8. Your score is saved if you win.

### Game Controls

| Input | Action |
|---|---|
| A–Z | Guess a letter |
| Complete phrase | Guess the entire phrase |
| `?` | Reveal the hint |
| 1–4 | Navigate the main menu |
| 1–3 | Select a visual theme |

## Scoring System

| Action | Score |
|---|---:|
| Correct letter | +10 per letter revealed |
| Wrong letter | -5 |
| Correct phrase | +15 per hidden letter revealed |
| Wrong phrase | -15 |
| Request hint (first time) | -20 |
| Win bonus | +50 |
| No-hint bonus | +50 |
| Attempts remaining | +10 per attempt |

The score can never fall below zero.

### Winning Example

If a player finishes a round with a score of 80 before bonuses, has 5 attempts remaining, and has used a hint:

- Round score: 80
- Win bonus: +50
- Remaining attempts bonus: +50
- No-hint bonus: +0

**Final score: 180 points**

## Game Themes

The player can choose from three visual themes:

1. **Hangman** – A hangman figure is progressively drawn with incorrect guesses.
2. **Sinking Ship** – A ship progressively sinks with incorrect guesses.
3. **Dying Robot** – A robot progressively deteriorates with incorrect guesses.

Each theme has 8 stages, corresponding to 0–7 incorrect guesses.

## Project Structure

```text
guessthephrase/
│
├── main.c
├── game.c
├── game.h
├── game_types.h
│
├── data_manager.c
├── data_manager.h
│
├── game_logic.c
├── game_logic.h
│
├── ui.c
├── ui.h
│
├── input.c
├── input.h
│
└── data/
    ├── categories.txt
    ├── sports.txt
    ├── life.txt
    ├── fantasy.txt
    └── scores.txt
```

## Module Description

### Module 1 – Data & Phrase Management

Responsible for:
- Managing phrase categories.
- Loading phrases and hints from files.
- Selecting random phrases.
- Saving player scores.
- Loading and sorting scoreboard entries.

### Module 2 – Core Game Logic

Responsible for:
- Initialising game rounds.
- Hiding and revealing letters.
- Checking letter and phrase guesses.
- Managing attempts.
- Calculating scores.
- Determining whether the player has won or lost.

### Module 3 – UI & Output

Responsible for:
- Displaying menus and game screens.
- Drawing the hidden phrase.
- Displaying hints, scores, and attempts.
- Drawing the selected visual theme.
- Applying console colours.
- Displaying win and loss screens.

### Module 4 – Input & Validation

Responsible for:
- Reading player input.
- Validating names and menu choices.
- Accepting letter and phrase guesses.
- Rejecting invalid or repeated guesses.

### Module 5 – Integration

Responsible for:
- Starting and controlling the program.
- Connecting all modules.
- Managing the main menu.
- Controlling the game loop.
- Handling transitions between screens and rounds.

## Data Files

| File | Purpose |
|---|---|
| `categories.txt` | Contains available phrase categories |
| `sports.txt` | Sports-related phrases and hints |
| `life.txt` | Life-related phrases and hints |
| `fantasy.txt` | Fantasy-related phrases and hints |
| `scores.txt` | Stores player scores |

Phrase files use the following format:

```text
phrase|hint
```

Example:

```text
Break a leg|Wishing luck to a performer
Piece of cake|Something very easy
```

## Compilation

The project uses GCC to compile all source files together.

```bash
gcc -Wall main.c game.c data_manager.c game_logic.c ui.c input.c -o guessthephrase
```

The header files are included through `#include` directives and do not need to be listed separately in the compilation command.

## Running the Game

### macOS / Linux

```bash
./guessthephrase
```

### Windows

```bash
guessthephrase.exe
```

## Technologies Used

- **Programming Language:** C
- **Compiler:** GCC
- **Libraries:** Standard C libraries
- **Interface:** Terminal / Console
- **File Handling:** Text files

---
