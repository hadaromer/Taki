// 207388331 - Omer Hadar
#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

// General definitions
#define MAX_CHARS_NAME 21 // maximum chars for name of player
#define NUM_CARDS_START 4 // number of cards for player in the beginning of the game
// Card colors definitions
#define NO_COLOR 0
#define YELLOW 1
#define RED 2
#define BLUE 3
#define GREEN 4
#define COLORS_CHAR (char[]) {' ','Y','R','B','G'} // array of char representations for colors
// Cards definitions
#define REGULAR_CARDS_NUMBER 9
#define ALL_CARDS_NUMBER 14
#define SPECIAL_CARDS_START_INDEX 9
#define CARDS_NAME (char*[]){"1","2","3","4","5","6","7","8","9","+","STOP","<->","TAKI","COLOR"}
#define CARDS_NAME_LEN (int[]){1,1,1,1,1,1,1,1,1,1,4,3,4,5}
#define PLUS 9
#define STOP 10
#define CHANGE_DIRECTION 11
#define TAKI 12
#define CHANGE_COLOR 13
#define UPPER_CARD -1
// Options
#define TAKE_CARD 0
#define FORWARD_DIRECTION 1
#define BACKWARD_DIRECTION -1
#define REGULAR_TURN 1
#define STOP_NEXT 2
#define CHANGE_GAME_DIRECTION 3

// structs declarations

typedef struct Card {
    int value; // value of the card
    int color; // color of the card
} CARD;

typedef struct PackOfCards {
    CARD *cards; // array of cards
    int logSizeCards; // logical size of cards array
    int physSizeCards; // physical size of cards array
} PACK_OF_CARDS;

typedef struct Stat_Card {
    int value; // value of the card
    int counter; // counter for how many times the card has been in the game
} STAT_CARD;

typedef struct Player {
    char name[MAX_CHARS_NAME]; // name of the player
    PACK_OF_CARDS packOfCards; // pack of cards for player
} PLAYER;


// functions declarations

void printWelcomeMessage();

PLAYER *initGame(int *pNumPlayers);

void createPlayer(PLAYER *player, int playerNumber);

void getPlayerName(char *name, int playerNumber);

void setPlayerInitCards(PLAYER *player);

void getInitCards(CARD *playerCards);

CARD shuffleCard(bool isFirstCard);

char *startGame(PLAYER *players, int numPlayers);

void printCard(CARD card, int cardNumber);

void switchConsoleColor(int color);

int playTurn(PLAYER *pPlayer, CARD *pUpperCard);

void printPackOfCards(PACK_OF_CARDS cardsPack);

int getChoice(int packSize, bool taki);

bool isValidCard(CARD card, CARD *pUpperCard, int restrictColor);

void removeCardFromPackOfPlayer(PACK_OF_CARDS *pPackOfCards, int indexToRemove);

void swapCards(CARD *x, CARD *y);

bool isValidChoice(int choice, int sizeCards);

void takeCard(PACK_OF_CARDS *pPackOfCards);

void expandPackOfCards(PACK_OF_CARDS *pPackOfCards);

void chooseColor(CARD *pCard);

void announceWinner(char *winner);

void initStats(STAT_CARD *stats, int size);

void printGameStats();

void getPadding(int *pLeft, int *pRight, int stringLength, int length);

STAT_CARD *merge(STAT_CARD *a, int size1, STAT_CARD *b, int size2);

void mergeSort(STAT_CARD *cards, int size);

void copyArr(STAT_CARD *dest, STAT_CARD *src, int size);

void release(PLAYER *pPlayers, int numPlayers);

STAT_CARD statsCardsArr[ALL_CARDS_NUMBER];

void main() {
    PLAYER *players; // will hold the players of the game
    int numPlayers; // will hold the number of player playing the game
    char *winner; // will hold the winner name
    srand(time(NULL)); // enable random with seed of the time

    printWelcomeMessage();
    initStats(statsCardsArr, ALL_CARDS_NUMBER);
    players = initGame(&numPlayers);
    winner = startGame(players, numPlayers);
    announceWinner(winner);
    mergeSort(statsCardsArr, ALL_CARDS_NUMBER);
    printGameStats();
    release(players, numPlayers);
}

/*
 * Releases the memory allocated for the game
 * in this case all cards array and array of players
 * pPlayers - pointer to array of players
 * numPlayers - number of players in the game
 */
void release(PLAYER *pPlayers, int numPlayers) {
    int i = 0;
    for (i = 0; i < numPlayers; i++) {
        free(pPlayers[i].packOfCards.cards);
    }
    free(pPlayers);
}

/*
 * Prints game stats about frequency of cards
 */
void printGameStats() {
    int i, rightPadCard, leftPadCard, rightPadFreq, leftPadFreq;
    printf("\n************ Game Statistics ************");
    printf("\nCard # | Frequency");
    printf("\n__________________");
    for (i = 0; i < ALL_CARDS_NUMBER; i++) {
        getPadding(&leftPadCard, &rightPadCard, CARDS_NAME_LEN[statsCardsArr[i].value], 7);
        getPadding(&leftPadFreq, &rightPadFreq, 1, 8);
        printf("\n%*s%*s%c%*s%d", leftPadCard,
               CARDS_NAME[statsCardsArr[i].value], rightPadCard, "", '|', leftPadFreq, "", statsCardsArr[i].counter);

    }
}

/*
 * Inits stats array and resets counters to 0
 * stats - array of stats cards
 * size - logical size of stats
 */
void initStats(STAT_CARD *stats, int size) {
    int i = 0;
    for (i = 0; i < size; i++) {
        stats[i].value = i;
        stats[i].counter = 0;
    }
}

/*
 * Announces the winner
 * winner - char array holds name of the winner
 */
void announceWinner(char *winner) {
    printf("The winner is... %s! Congratulations!", winner);
}


/*
 * Prints welcome message
 */
void printWelcomeMessage() {
    printf("************  Welcome to TAKI game !!! ************");
}

/*
 * Inits the game by creating array of players and create each player
 * returns array of players
 * numPlayers - pointer to int that holds number of players
 */
PLAYER *initGame(int *pNumPlayers) {
    PLAYER *players;
    printf("\nPlease enter the number of players: \n");
    scanf("%d", pNumPlayers);
    // allocation of memory
    players = (PLAYER *) malloc(*pNumPlayers * sizeof(PLAYER));
    if (players == NULL) {
        exit(1);
    }
    for (int i = 0; i < *pNumPlayers; i++) {
        createPlayer(&players[i], i + 1);
    }
    return players;
}

/*
 * Creates a player by getting his name and "give" him cards
 * pPlayer - pointer to the player
 * playerNumber - the number (index) of the player in the array
 */
void createPlayer(PLAYER *pPlayer, int playerNumber) {
    getPlayerName(pPlayer->name, playerNumber);
    setPlayerInitCards(pPlayer);
}


/*
 * Gets the player name
 * name - array of chars that will hold the name
 * playerNumber - number of the player in the game
 */
void getPlayerName(char *name, int playerNumber) {
    printf("\nPlease enter the first name of player #%d:\n", playerNumber);
    scanf("%s", name);
}

/*
 * Inits the cards for a player
 * pPlayer - pointer to the player
 */
void setPlayerInitCards(PLAYER *pPlayer) {
    // memory allocation for array of cards
    pPlayer->packOfCards.cards = (CARD *) malloc(NUM_CARDS_START * sizeof(CARD));
    if (pPlayer->packOfCards.cards == NULL) {
        exit(1);
    }
    pPlayer->packOfCards.logSizeCards = NUM_CARDS_START;
    pPlayer->packOfCards.physSizeCards = NUM_CARDS_START;
    getInitCards(pPlayer->packOfCards.cards);
}


/*
 * Sets the initial cards of a player
 * playerCards - array of player's cards
 */
void getInitCards(CARD *playerCards) {
    int i;
    for (i = 0; i < NUM_CARDS_START; i++) {
        playerCards[i] = shuffleCard(false);
    }
}

/*
 * Returns a random card
 * isFirstCard - flag for detecting if this is the first card for the game
 * first card for the game can't be a special card
 */
CARD shuffleCard(bool isFirstCard) {
    CARD card;
    int numberOfCards = isFirstCard ? REGULAR_CARDS_NUMBER : ALL_CARDS_NUMBER;
    // rand will return a number between 0 to 32,767 so we will get a random number from 0 to (numberOfCards - 1)
    card.value = rand() % numberOfCards;
    if (card.value == CHANGE_COLOR) {
        card.color = NO_COLOR;
    } else {
        card.color = rand() % 4 + 1;
    }
    statsCardsArr[card.value].counter++;
    return card;
}

/*
 * Starts the game and returns the winner of it
 * players - array of players
 * numPlayers - number of players
 */
char *startGame(PLAYER *players, int numPlayers) {
    CARD upperCard; // the upper card of the game
    char *winner; // name of the winner
    bool play = true; // indicates wheater the game is still running
    int playerTurn = 0; // index of the player that this is his turn to play
    int direction = FORWARD_DIRECTION; // the direction of the game
    int move = 1; // holds the number of steps to get next player turn
    upperCard = shuffleCard(true);
    while (play) {
        move = playTurn(&players[playerTurn], &upperCard);
        if (move == CHANGE_GAME_DIRECTION) {
            move = REGULAR_TURN;
            direction *= -1; // switch between FORWARD_DIRECTION and BACKWARD_DIRECTION
        }
        // handle last card is stop and 2 players
        if (players[playerTurn].packOfCards.logSizeCards == 0 && move == STOP_NEXT && numPlayers == 2) {
            takeCard(&players[playerTurn].packOfCards);
        }
        if (players[playerTurn].packOfCards.logSizeCards == 0) {
            play = false;
            strcpy(winner, players[playerTurn].name);
        }
        // moves according to last player turn
        // if it's get to the corners (start or end of the array) it will go to the other corner
        // to simulate a circular order
        for (int i = 0; i < move; i++) {
            playerTurn += direction;
            if (playerTurn > numPlayers - 1) {
                playerTurn = 0;
            } else if (playerTurn < 0) {
                playerTurn = numPlayers - 1;
            }
        }
    }
    return winner;
}


/*
 * Prints a card
 * if card number is -1 (upper card) will print upper card
 * else the card number
 * card - the card to print
 * cardNumber - number of the card or UPPER_CARD
 */
void printCard(CARD card, int cardNumber) {
    int rightPad, leftPad;
    getPadding(&rightPad, &leftPad, CARDS_NAME_LEN[card.value], 7);
    if (cardNumber == UPPER_CARD) {
        printf("Upper card:");
    } else {
        printf("Card #%d:", cardNumber);
    }
    getPadding(&leftPad, &rightPad, CARDS_NAME_LEN[card.value], 7);
    switchConsoleColor(card.color);
    printf("\n*********");
    printf("\n*       *");
    printf("\n*%*s%*s*", leftPad, CARDS_NAME[card.value], rightPad, "");
    printf("\n*   %c   *", COLORS_CHAR[card.color]);
    printf("\n*       *");
    printf("\n*********");
    switchConsoleColor(NO_COLOR);
}

/*
 * Returns padding for a specific length
 * pLeft - pointer for left padding
 * pRight - pointer for right padding
 * stringLength - the length of the string to print
 * length - wanted size
 */
void getPadding(int *pLeft, int *pRight, int stringLength, int length) {
    *pLeft = length / 2 + length % 2 + stringLength / 2;
    *pRight = length - *pLeft;
}

/*
 * Switches the color of printf in console
 * color - the desired color
 */
void switchConsoleColor(int color) {
    switch (color) {
        case NO_COLOR:
            printf("\033[0m");
            break;
        case YELLOW:
            printf("\033[1;33m");
            break;
        case RED:
            printf("\033[1;31m");
            break;
        case BLUE:
            printf("\033[0;34m");
            break;
        case GREEN:
            printf("\033[0;32m");
            break;
        default:
            printf("\033[0m");
            break;
    }
}

/*
 * Plays a turn of a player
 * pPlayer - pointer to a player
 * pUpperCard - pointer to the upper card of the game
 */
int playTurn(PLAYER *pPlayer, CARD *pUpperCard) {
    bool turnEnded = false; // indicates if the turn has ended
    bool validChoice = false; // indicates if the choice is valid
    bool taki = false; // indicated if it's "taki mode" (when a player puts taki card)
    int restrictColor = NO_COLOR; // if it's "taki mode" a player can only put cards from same color as the taki card
    // so that indicates if there is a restriction for that
    int choice; // will hold the choice of the player
    bool isPlayerHasPutCard = false; // indicated if the player has put a card
    while (!turnEnded) {
        validChoice = false;
        printCard(*pUpperCard, UPPER_CARD);
        printf("\n%s's turn:\n\n", pPlayer->name);
        printPackOfCards(pPlayer->packOfCards);
        while (!validChoice) {
            choice = getChoice(pPlayer->packOfCards.logSizeCards, restrictColor);
            validChoice = true;
            if (choice == TAKE_CARD) {
                // TAKE_CARD in taki mode means end taki mode
                // if the player has put PLUS as last card in taki mode he needs to take a card
                if (!taki || (taki && pUpperCard->value != PLUS)) {
                    takeCard(&pPlayer->packOfCards);
                }
                turnEnded = true;
            } else if (isValidChoice(choice - 1, pPlayer->packOfCards.logSizeCards) &&
                       isValidCard(pPlayer->packOfCards.cards[choice - 1], pUpperCard, restrictColor)) {
                *pUpperCard = pPlayer->packOfCards.cards[choice - 1];
                isPlayerHasPutCard = true;
                removeCardFromPackOfPlayer(&pPlayer->packOfCards, choice - 1);
                turnEnded = true;
                // in "taki mode" the turn isn't end until the player asks for it or doesn't have more cards
                if (taki && pPlayer->packOfCards.logSizeCards > 0) {
                    turnEnded = false;
                }
                // the player has put change color card
                if (pUpperCard->value == CHANGE_COLOR) {
                    chooseColor(pUpperCard);
                    turnEnded = true;
                } else if (pUpperCard->value == TAKI) { // enter "taki mode"
                    taki = true;
                    restrictColor = pUpperCard->color;
                    if (pPlayer->packOfCards.logSizeCards != 0)
                        turnEnded = false;
                } else if (pUpperCard->value == PLUS) { // in plus card the player has one more turn
                    if (pPlayer->packOfCards.logSizeCards == 0) {
                        takeCard(&pPlayer->packOfCards);
                        turnEnded = true;
                    } else {
                        turnEnded = false;
                    }
                }

            } else {
                validChoice = false;
                printf("\nInvalid card! Try again.");
            }
        }
    }
    if (isPlayerHasPutCard) {
        // if player has put special card return flag for it
        if (pUpperCard->value == STOP) {
            return STOP_NEXT;
        } else if (pUpperCard->value == CHANGE_DIRECTION) {
            return CHANGE_GAME_DIRECTION;
        }
    } else return REGULAR_TURN;
}

/*
 * Gets a color for color switch card and change the card color
 * pCard - pointer to change color card
 */
void chooseColor(CARD *pCard) {
    printf("\nPlease enter your color choice:");
    printf("\n1 - Yellow");
    printf("\n2 - Red");
    printf("\n3 - Blue");
    printf("\n4 - Green");
    scanf("%d", &pCard->color);
}


/*
 * Prints pack of cards
 * cardsPack - the pack of card to print
 */
void printPackOfCards(PACK_OF_CARDS cardsPack) {
    int i;
    for (i = 0; i < cardsPack.logSizeCards; i++) {
        printCard(cardsPack.cards[i], i + 1);
        printf("\n");
    }
}

/*
 * Takes a card
 * if the physical size of the array isn't big enough the function will expand the array
 * pPackOfCards - pack of cards of the player that need to take a card
 */
void takeCard(PACK_OF_CARDS *pPackOfCards) {
    if (pPackOfCards->logSizeCards == pPackOfCards->physSizeCards) {
        expandPackOfCards(pPackOfCards);
    }
    pPackOfCards->cards[pPackOfCards->logSizeCards] = shuffleCard(false);
    pPackOfCards->logSizeCards++;
}

/*
 * Expands pack of cards by allocate memory for new array of cards
 * copy the old data and free the old array
 * we double the size of the array because it's more efficient
 * pPackOfCards - the pack of cards to expand
 */
void expandPackOfCards(PACK_OF_CARDS *pPackOfCards) {
    int i;
    pPackOfCards->physSizeCards *= 2;
    CARD *newCards = (CARD *) malloc(pPackOfCards->physSizeCards * sizeof(CARD));
    if (newCards == NULL) {
        exit(1);
    }
    for (i = 0; i < pPackOfCards->logSizeCards; i++) {
        newCards[i] = pPackOfCards->cards[i];
    }
    free(pPackOfCards->cards);
    pPackOfCards->cards = newCards;
}

/*
 * Gets choice for player turn
 * packSize - the size of player's pack of cards
 * taki - detects if it's "taki mode"
 */
int getChoice(int packSize, bool taki) {
    int choice;
    if (taki) {
        printf("Please enter 0 if you want to finish your turn");
    } else {
        printf("Please enter 0 if you want to take a card from the deck");
    }
    printf("\nor 1-%d if you want to put one of your cards in the middle:", packSize);
    scanf("%d", &choice);
    return choice;
}

/*
 * Returns if the choice is valid
 * choice - the choice of the player
 * sizeCards - the size of player's pack of cards
 */
bool isValidChoice(int choice, int sizeCards) {
    return choice >= 0 && choice < sizeCards;
}


/*
 * Returns if a card can be put over an upper card
 * it can be done if:
 * the card has no color (change color card)
 * same value
 * same color
 * card - a card, the one to put on the upper card
 * pUpperCard - a pointer to the upper card
 */
bool isValidCard(CARD card, CARD *pUpperCard, int restrictColor) {
    if (restrictColor == NO_COLOR) {
        return card.color == NO_COLOR || card.value == pUpperCard->value || card.color == pUpperCard->color;
    }
    return card.color == NO_COLOR || card.color == restrictColor;
}

/*
 * Removes card from pack of cards
 * doing it by moving the card to the end (swap with last one)
 * and decrease the logical size of the array
 * pPackOfCards - pointer to the pack of cards
 * indexToRemove - the index of the card to remove
 */
void removeCardFromPackOfPlayer(PACK_OF_CARDS *pPackOfCards, int indexToRemove) {
    if (indexToRemove != pPackOfCards->logSizeCards - 1) {
        swapCards(&pPackOfCards->cards[indexToRemove], &pPackOfCards->cards[pPackOfCards->logSizeCards - 1]);
    }
    pPackOfCards->logSizeCards--;

}

/*
 * Swaps between two cards
 * x - pointer to first card
 * y - pointer to second card
 */
void swapCards(CARD *x, CARD *y) {
    CARD temp;
    temp = *x;
    *x = *y;
    *y = temp;
}

/*
 * Merges sort the stats array
 * cards - array of stat cards
 * size - logical size of cards
 */
void mergeSort(STAT_CARD *cards, int size) {
    STAT_CARD *tmp;
    if (size <= 1)
        return;
    mergeSort(cards, size / 2);
    mergeSort(cards + size / 2, size - size / 2);
    tmp = merge(cards, size / 2, cards + size / 2, size - size / 2);
    if (tmp) {
        copyArr(cards, tmp, size);
        free(tmp);
    } else {
        exit(1);
    }
}

/*
 * Copies one array to another
 * dest - destination array
 * src - source array
 * size - logical size of src
 */
void copyArr(STAT_CARD *dest, STAT_CARD *src, int size) {
    int i = 0;
    for (i = 0; i < size; i++) {
        dest[i] = src[i];
    }
}

/*
 * Merges between 2 STAT_CARD arrays
 * returns a sorted merged array
 * a - first STAT_CARD array
 * size1 - logical size of a
 * b - second array STAT_CARD
 * size2 - logical size of b
 */
STAT_CARD *merge(STAT_CARD *a, int size1, STAT_CARD *b, int size2) {
    int ind1, ind2, resInd;
    // memory allocation for the res array
    STAT_CARD *res = (STAT_CARD *) malloc((size1 + size2) * sizeof(STAT_CARD));

    if (res) {
        ind1 = ind2 = resInd = 0;

        while ((ind1 < size1) && (ind2 < size2)) {
            if (a[ind1].counter >= b[ind2].counter) {
                res[resInd] = a[ind1];
                ind1++;
            } else {
                res[resInd] = b[ind2];
                ind2++;
            }
            resInd++;
        }

        while (ind1 < size1) {
            res[resInd] = a[ind1];
            ind1++;
            resInd++;
        }

        while (ind2 < size2) {
            res[resInd] = b[ind2];
            ind2++;
            resInd++;
        }
    }
    return res;
}