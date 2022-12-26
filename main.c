#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <ctype.h> //tolower func
#include <conio.h>


/*
    Not only one game, need a while (for stop game) (offer a stop and exit casino)       Done
    Fix second hand > 21                                                                 Done
    Fix negative bets                                                                    Done
    Save logs to file:                                                                   Done
    ACE IS 1/11                                                                          Done
    All scanf Buffer Overflow issue fix                                                  Done
    Fix bets                                                                             Done
        Now max str length = 9, u cant enter value more than 9 symbols in it
        then it checked for correct form of int value, and if first 9 symbols can be
        interpreted as int value it proceed
*/


typedef struct card {
    int num;
    int suit;
} card;

typedef struct node {
    card card;
    struct node* next;
} node;


// Function list for fast navigating
void push(node **head, card data);
void index_push(node *head, card data, int index);
card pop(node **head);
card index_pop(node *head, int index);
void shuffle(node *head);
void deck_init(node **head);
int get_hand_total(node *head);
void print_list(node *head);
void print_list_to_logs(node *head, FILE *game_logs);
void to_lower(char *string);
int len_list(node *head);
void card_return(node **player, node **player_split, node **dealer, node **deck, int split_flag);
void show_cards(node *p1, node *p2, node *d, int split_flag);
void delete_list(node** head);
int check(char *str);



void push(node **head, card data) {
    // Create a new node
    node *new_node = malloc(sizeof(node));
    new_node -> card = data;
    new_node -> next = NULL;

    // Bind pointer to the new node of list
    new_node -> next = *head;
    *head = new_node;
}


card pop(node **head) {
    // Find last node in list
    node *current = *head;
    node *prev = NULL;
    while (current->next != NULL) {
        prev = current;
        current = current->next;
    }

    // Remove the last node from list
    prev -> next = NULL;

    // Return the data from removed node
    card data = current -> card;
    free(current);
    return data;
}


void index_push(node *head, card data, int index) {
    // Create a new node
    node *new_node = malloc(sizeof(node));
    new_node -> card = data;
    new_node -> next = NULL;

    if (index == 0) {
        // Add new node
        new_node -> next = head;
        head = new_node;
    } else {
        // Find node at specified index
        node *current = head;
        for (int i = 0; i < index-1; i++) {
            current = current -> next;
        }

        // Insert the new node after specified index
        new_node -> next = current -> next;
        current -> next = new_node;
    }
}

card index_pop(node *head, int index) {
    // Find node at specified index
    node *current = head;
    node *prev = NULL;
    for (int i = index; i > 0; i++) {
        prev = current;
        current = current->next;
    }

    // Remove node at specified index
    prev->next = current->next;

    // Return data from removed node
    card data = current->card;
    free(current);
    return data;
}


void shuffle(node *head) {
    srand((unsigned int) time(NULL));
    node *current = head;

    while (current != NULL) {
        // Choose random node
        int index = (rand() % 9 + 1);
        node *random = head;
        for (int i = 0; i < index; i++) {
            random = random -> next;
        }

        // Swap current and random
        card data = current -> card;
        current -> card = random -> card;
        random -> card = data;

        current = current->next;
    }
}


void print_list(node *head) {
    node* current = head;
    // Each iteration change pointer, count all nodes from head to the NULL pointer
    while (current != NULL) {
        if (current->card.num == 1) {
            printf("1/11[%c] ", current->card.suit);
        } else {
            printf("%d[%c] ", current->card.num, current->card.suit);
        }
        current = current -> next;
    }
    printf("\n");
}

void print_list_to_logs(node *head, FILE *game_logs) {
    node* current = head;
    while (current != NULL) {
        if (current->card.num == 1) {
            fprintf(game_logs, "1/11[%d] ", current->card.suit);
        } else {
            fprintf(game_logs, "%d[%d] ", current->card.num, current->card.suit);
        }
        current = current -> next;
    }
    fprintf(game_logs, "\n");
}


void deck_init(node **head) {
    // 4 suits * 10 nums + (3 pics = 10) + 1 ace cards
    for (int i = 3; i < 7; i++) {
        for (int j = 10; j > 0; j--) {
            card card;
            card.num = j; card.suit = i;
            push(head, card);
            if (j == 10) {
                push(head, card);
                push(head, card);
                push(head, card);
            }
        }
    }
}


int len_list(node *head) {
    int n = 0;
    while (head != NULL) {
        n++;
        head = head -> next;
    }
    return n;
}


void to_lower(char *string) {
    for(int i = 0; i < strlen(string); i++) {    // for(int i = 0; string[i]; i++) {} is possible
        string[i] = tolower(string[i]);
    }
}


int get_hand_total(node *head) {
    int ace_flag = 0;
    node *current = head;
    int sum = 0;
    while (current != NULL) {
        if (current -> card.num == 1) {
            ace_flag = 1;
        }
        sum += current -> card.num;
        current = current -> next;
    }
    if ((ace_flag == 1) && ((sum - 1 + 11) <= 21)) {
        return (sum - 1 + 11);
    } else { return sum; }
}


// Can be optimized somehow
void show_cards(node *p1, node *p2, node *d, int split_flag) {
    if (split_flag == 1) {
        printf("Dealer hand: (%d) : ", get_hand_total(d)); print_list(d);
        printf("First player hand: (%d) : ", get_hand_total(p1)); print_list(p1);
        printf("Second player hand: (%d) : ", get_hand_total(p2)); print_list(p2);

    } else {
        printf("Dealer hand (%d) : ", get_hand_total(d)); print_list(d);
        printf("Player hand (%d) : ", get_hand_total(p1)); print_list(p1);
    }
}


void card_return(node **player, node **player_split, node **dealer, node **deck, int split_flag) {
    //Dealer return
    if (len_list(*dealer) == 1) {
        push(deck, (**dealer).card);
        delete_list(dealer);
    } else {
        for (int i = 0; i < len_list(*dealer); i++) {
            push(deck, pop(dealer));
        }
        push(deck, (**dealer).card);
        delete_list(dealer);
    }
    //Player return
    if (len_list(*player) == 1) {
        push(deck, (**player).card);
        delete_list(player);
    } else {
        for (int i = 0; i < len_list(*player); i++) {
            push(deck, pop(player));
        }
        push(deck, (**player).card);
        delete_list(player);
    }
    //Player split return case
    if (split_flag == 1) {
        if (len_list(*player_split) == 1) {
            push(deck, (**player_split).card);
            delete_list(player_split);
        } else {
            for (int i = 0; i < len_list(*player_split); i++) {
                push(deck, pop(player_split));
            }
            push(deck, (**player_split).card);
            delete_list(player_split);
        }
    }
}


void delete_list(node** head) {
    /* deref head_ref to get the real head */
    node* current = *head;
    node* next;

    while (current != NULL)
    {
        next = current->next;
        free(current);
        current = next;
    }

    *head = NULL;
}

int check(char *str) {
    for (int i = 0; i < strlen(str); i++) {
        if (str[i] == ' ' || isdigit(str[i]) == 0) { return 0; }
    }
    return 1;
}



int main(void) {

    FILE *game_logs = fopen("logs.txt", "w");
    fprintf(game_logs, "Logs initialization\n\n");
    unsigned int pool = 0, bet = 0;
    char pool_str[10], bet_str[10];
    puts("Enter your pool of money:");
    for (;;) {
        scanf("%9s", pool_str);
        if (check(pool_str) == 1) {
            pool = atoi(pool_str);
        }
        if (pool <= 0) {
            printf("Invalid value, try again (min = 1, max = 999999999)\n");
            fflush(stdin);
        } else {
            break;
        }
    }
    printf("%d\n", pool);
    fflush(stdin);

    //Deck initialize, shuffle, print
    node *deck = NULL;
    deck_init(&deck);
    shuffle(deck);

    //Dealer hand initialize
    node *hand_dealer = NULL;

    //Player hand initialize
    node *hand_player = NULL;

    // Split hand initialize
    node *hand_player_split = NULL;

    /*                  Game logic starts
    Ace is always 11
    No insurance
    No soft 17, means if calc(dealer hand) < 16 then + 1 card
    HIT                 push(&hand, pop(&deck));
    STAND               Player while break
    SPLIT               Get another player hand, split logic
    DOUBLE              x2 bet, +1 card, player while break
    SURRENDER           game over (2 cards, bet/2)             */

    char move[9], answer[4];
    int split_flag = 0, total_games_value = 0, stop_flag = 0, wins_value = 0, loses_value = 0, pushes_value = 0;
    for (;;) {
        fflush(stdin);
        puts("Want to play? Enter yes/no");
        scanf("%3s", answer);
        fflush(stdin);
        to_lower(answer);
        if (strcmp(answer, "yes") == 0) {
            if (bet == 0) {
                if (pool == 0) {
                    puts("U have no money, - 10000000000000 social credit!");
                    exit(-100);
                }
                printf("Enter your bet (Should be < than your pool)\n");
                for (;;) {
                    scanf("%9s", bet_str);
                    fflush(stdin);
                    if (check(bet_str) == 1) {
                        bet = atoi(bet_str);
                    } //printf("%d\n", bet);
                    if (bet > pool || bet <= 0) {
                        puts("Incorrect value, try again");
                        fflush(stdin);
                        continue;
                    } else break;
                }
                fflush(stdin);
                pool -= bet;
                fprintf(game_logs, "POOL = %d\nBET = %d\n", pool, bet);
            }
            printf("Pool - %d\n", pool);
            total_games_value++; stop_flag = 0;
            // +1 dealer card, +2 player card every game
            push(&hand_dealer, pop(&deck));
            push(&hand_player, pop(&deck));
            push(&hand_player, pop(&deck));
            fprintf(game_logs, "Game №%d\n", total_games_value);
            fprintf(game_logs, "New game starts, cards initialized:\nDealer hand (%d) : ", get_hand_total(hand_dealer));
            print_list_to_logs(hand_dealer, game_logs);
            fprintf(game_logs, "Player hand (%d) : ", get_hand_total(hand_player));
            print_list_to_logs(hand_player, game_logs); fprintf(game_logs, "\n");

            show_cards(hand_player, hand_player_split, hand_dealer, split_flag);
            for (;;) {
                if (get_hand_total(hand_player) == 21) {
                    bet *= 1.5;
                    printf("Player got Blackjack, wins %d\n", bet);
                    fprintf(game_logs, "BLACKJACK: PLAYER, Player wins %d\n\n");
                    card_return(&hand_player, &hand_player_split, &hand_dealer, &deck, split_flag);
                    wins_value++;
                    break;
                }
                puts("Hit, split, double, surrender, stand?");
                scanf("%9s", move);
                fflush(stdin);
                to_lower(move);
                // HIT
                if (strcmp(move, "hit") == 0) {

                    // If player have 2 hands case
                    if (split_flag == 1) {
                        puts("Choose on which hand u want to hit (Enter 1 for the first, 2 for the second, either way u'll hit 1)");
                        int split_value = 0;
                        char split_answer[1];
                        for (;;) {
                            scanf("%1s", split_answer);
                            if (check(split_answer) == 1) {
                                split_value = atoi(split_answer);
                                break;
                            } else {
                                printf("Incorrect input, try again\n");
                                fflush(stdin);
                                continue;
                            }
                        }
                        fflush(stdin);
                        if (split_value != 2 && split_value != 1) split_value = 1;  //Case if user enter incorrect value

                        // Choose which hand
                        if (split_value == 1) {
                            push(&hand_player, pop(&deck));
                        } else if (split_value == 2) {
                            push(&hand_player_split, pop(&deck));
                        }
                        fprintf(game_logs, "SPLIT HIT: Hand - %d, New card - %d[%d]\n", split_value, hand_player->card.num, hand_player->card.suit);
                        fprintf(game_logs, "Dealer hand (%d) : ", get_hand_total(hand_dealer)); print_list_to_logs(hand_dealer, game_logs);
                        fprintf(game_logs, "Player hand №1 (%d) : ", get_hand_total(hand_player)); print_list_to_logs(hand_player, game_logs);
                        fprintf(game_logs, "Player hand №2 (%d) : ", get_hand_total(hand_player_split)); print_list_to_logs(hand_player_split, game_logs);
                        fprintf(game_logs, "\n"); split_value = 1;
                        show_cards(hand_player, hand_player_split, hand_dealer, split_flag);

                    } else {
                        // Just get a new card
                        push(&hand_player, pop(&deck));
                        puts("U got a new card");
                        show_cards(hand_player, hand_player_split, hand_dealer, split_flag);
                        fprintf(game_logs, "HIT: New card - %d[%d]\n", hand_player->card.num, hand_player->card.suit);
                        fprintf(game_logs, "Dealer hand (%d) : ", get_hand_total(hand_dealer)); print_list_to_logs(hand_dealer, game_logs);
                        fprintf(game_logs, "Player hand (%d) : ", get_hand_total(hand_player)); print_list_to_logs(hand_player, game_logs);
                        fprintf(game_logs, "\n");
                    }
                }


                    //STAND
                else if (strcmp(move, "stand") == 0) {
                    puts("Player stand, dealers move");
                    fprintf(game_logs, "STAND. Dealers move:\n");
                    break;
                }


                    // DOUBLE
                else if (strcmp(move, "double") == 0) {
                    if (pool - bet < 0) {
                        puts("Not enough money to double ur bet");
                        continue;
                    }
                    pool -= bet;
                    if (split_flag == 1) {
                        puts("Choose on which hand u want to double (Enter 1 for the first, 2 for the second, either way u'll hit first)");
                        int split_value = 0;
                        char split_answer[1];
                        for (;;) {
                            scanf("%1s", split_answer);
                            if (check(split_answer) == 1) {
                                split_value = atoi(split_answer);
                                break;
                            } else {
                                printf("Incorrect input, try again\n");
                                fflush(stdin);
                                continue;
                            }
                        } //я устал помогите
                        fflush(stdin);
                        if (split_value != 2 && split_value != 1) split_value = 1;  //Case if user enter incorrect value

                        bet *= 2;
                        // Choose which hand
                        if (split_value == 1) {
                            push(&hand_player, pop(&deck));
                            fprintf(game_logs, "DOUBLE: New card - %d[%d]\n", hand_player->card.num, hand_player->card.suit);
                            fprintf(game_logs, "Dealer hand (%d) : ", get_hand_total(hand_dealer)); print_list_to_logs(hand_dealer, game_logs);
                            fprintf(game_logs, "Player hand №1 (%d) : ", get_hand_total(hand_player)); print_list_to_logs(hand_player, game_logs);
                            fprintf(game_logs, "Player hand №2 (%d) : ", get_hand_total(hand_player_split)); print_list_to_logs(hand_player_split, game_logs);
                            fprintf(game_logs, "\n"); split_value = 0;
                            break;
                        } else if (split_value == 2) {
                            push(&hand_player_split, pop(&deck));
                            fprintf(game_logs, "DOUBLE: New card - %d[%d]\n", hand_player->card.num, hand_player->card.suit);
                            fprintf(game_logs, "Dealer hand (%d) : ", get_hand_total(hand_dealer)); print_list_to_logs(hand_dealer, game_logs);
                            fprintf(game_logs, "Player hand №1 (%d) : ", get_hand_total(hand_player)); print_list_to_logs(hand_player, game_logs);
                            fprintf(game_logs, "Player hand №2 (%d) : ", get_hand_total(hand_player_split)); print_list_to_logs(hand_player_split, game_logs);
                            fprintf(game_logs, "\n"); split_value = 1;
                            break;
                        }
                        show_cards(hand_player, hand_player_split, hand_dealer, split_flag);
                    } else if (split_flag == 0) {
                        bet *= 2;
                        printf("Player take a new card, bet is doubled - %d, no more player moves\n", bet);
                        push(&hand_player, pop(&deck));
                        show_cards(hand_player, hand_player_split, hand_dealer, split_flag);
                        fprintf(game_logs, "DOUBLE: New card - %d[%d]\n", hand_player -> card.num, hand_player -> card.suit);
                        fprintf(game_logs, "Dealer hand (%d) : ", get_hand_total(hand_dealer)); print_list_to_logs(hand_dealer, game_logs);
                        fprintf(game_logs, "Player hand (%d) : ", get_hand_total(hand_player)); print_list_to_logs(hand_player, game_logs);
                        fprintf(game_logs, "\n");
                        break;
                    }
                }


                    //SURRENDER
                else if (strcmp(move, "surrender") == 0) {
                    // If player have more than 2 cards case
                    if (split_flag != 0 || len_list(hand_player) > 2) {
                        puts("cant surrender, invalid case (needs value cards <= 2 and non split case)");
                        continue;
                    }
                    // Player gets back half of his bet, game over
                    pool += bet / 2; bet /= 2;
                    fprintf(game_logs, "SURRENDER: Half of bet is saved - %d\n", bet);
                    printf("Surrender, half of bet is saved - %d", bet);
                    bet = 0;
                    stop_flag = 1;
                    break;
                }


                    // SPLIT
                else if (strcmp(move, "split") == 0) {
                    // If player doesn't have 2 cards exactly
                    if (len_list(hand_player) != 2) {
                        puts("cant split, invalid value of cards (needs 2)");
                        continue;
                    }
                    // Second player hand get one card from main player hand
                    push(&hand_player_split, pop(&hand_player));
                    fprintf(game_logs, "SPLIT: Card has been moved\n");
                    fprintf(game_logs, "Dealer hand (%d) : ", get_hand_total(hand_dealer)); print_list_to_logs(hand_dealer, game_logs);
                    fprintf(game_logs, "Player hand №1 (%d) : ", get_hand_total(hand_player)); print_list_to_logs(hand_player, game_logs);
                    fprintf(game_logs, "Player hand №2 (%d) : ", get_hand_total(hand_player_split)); print_list_to_logs(hand_player_split, game_logs);
                    fprintf(game_logs, "\n");
                    split_flag = 1;
                    show_cards(hand_player, hand_player_split, hand_dealer, split_flag);
                } else {
                    puts("Incorrect input");
                    continue;
                }



                // If player takes his Blackjack:
                if ((split_flag == 0 && get_hand_total(hand_player) == 21) ||
                (split_flag == 1 && (get_hand_total(hand_player) == 21 || get_hand_total(hand_player_split) == 21))) {
                    pool += bet * 2; bet *= 1.5; wins_value++;
                    printf("Blackjack! Take yours %d\n", bet);
                    fprintf(game_logs, "PLAYER BLACKJACK1: Player win %d\n\n", bet);
                    card_return(&hand_player, &hand_player_split, &hand_dealer, &deck, split_flag);
                    stop_flag = 1; bet = 0;
                    break;
                }


                // Player got more than 21, player lose
                if ((get_hand_total(hand_player) > 21) || (get_hand_total(hand_player_split)) > 21) {
                    printf("Player got more than 21, player lose\n");
                    fprintf(game_logs, "PLAYER BUST: Player lose his bet\n\n");
                    card_return(&hand_player, &hand_player_split, &hand_dealer, &deck, split_flag);
                    bet = 0; loses_value++;
                    stop_flag = 1;
                    break;
                }
            }
            if (stop_flag == 1) {
                split_flag = 0;
                continue;
            }

            // If player takes his Blackjack:
            if ((split_flag == 0 && get_hand_total(hand_player) == 21) ||
                (split_flag == 1 && (get_hand_total(hand_player) == 21 || get_hand_total(hand_player_split) == 21))) {
                pool += bet * 2; bet *= 1.5; wins_value++;
                printf("Blackjack!, take yours %d", bet);
                fprintf(game_logs, "PLAYER BLACKJACK2: Player win %d\n\n", bet);
                card_return(&hand_player, &hand_player_split, &hand_dealer, &deck, split_flag);
                bet = 0;
                continue;
            }


            // Player got more than 21, player lose
            if ((get_hand_total(hand_player) > 21) || (get_hand_total(hand_player_split)) > 21) {
                printf("Player got more than 21, player lose\n");
                fprintf(game_logs, "PLAYER BUST: Player lose his bet\n\n");
                card_return(&hand_player, &hand_player_split, &hand_dealer, &deck, split_flag);
                bet = 0; loses_value++;
                continue;
            }

            // When all player moves is done, dealer gets his cards
            while (get_hand_total(hand_dealer) < 16) {
                push(&hand_dealer, pop(&deck));
            }

            puts("");
            show_cards(hand_player, hand_player_split, hand_dealer, split_flag);
            fprintf(game_logs, "Dealer getting his cards:\n");
            fprintf(game_logs, "Dealer hand (%d) : ", get_hand_total(hand_dealer)); print_list_to_logs(hand_dealer, game_logs);
            if (split_flag == 1) {
                fprintf(game_logs, "Player hand №1 (%d) : ", get_hand_total(hand_player)); print_list_to_logs(hand_player, game_logs);
                fprintf(game_logs, "Player hand №2 (%d) : ", get_hand_total(hand_player_split)); print_list_to_logs(hand_player_split, game_logs);
            } else {
                fprintf(game_logs, "Player hand (%d) : ", get_hand_total(hand_player)); print_list_to_logs(hand_player, game_logs);
            }
            fprintf(game_logs, "\n"); puts("");


            // Int values init for reduce function calls later
            int hand_dealer_sum, hand_player_sum, hand_player_split_sum;
            hand_dealer_sum = get_hand_total(hand_dealer);
            hand_player_sum = get_hand_total(hand_player);
            hand_player_split_sum = get_hand_total(hand_player_split);


            //Dealer got blackjack, if u too - ure win, if not - u lose.
            if (hand_dealer_sum == 21) {
                if ((split_flag == 1 && (hand_player_sum == 21 || hand_player_split_sum == 21)) ||
                    (split_flag == 0 && hand_player_sum == 21)) {
                    pool += bet * 1.5; bet *= 1.5;
                    printf("Blackjack: Dealer = Player, take yours %d\n", bet);
                    fprintf(game_logs, "BLACKJACK: PLAYER == DEALER, Player wins %d\n\n", bet);
                    card_return(&hand_player,&hand_player_split, &hand_dealer, &deck, split_flag);
                    split_flag = 0; wins_value++; bet = 0;
                    continue;
                } else {
                    bet = 0;
                    puts("Dealer got Blackjack. Shit happens, ure lose");
                    fprintf(game_logs, "BLACKJACK: DEALER, Player loses his bet\n\n");
                    card_return(&hand_player, &hand_player_split, &hand_dealer, &deck, split_flag);
                    split_flag = 0; loses_value++;
                    continue;
                }
            }

            //If dealer got  more than 21 u win
            if (hand_dealer_sum > 21) {
                pool += bet * 2; bet *= 2;
                printf("Dealer got more than 21, u win %d\n", bet);
                fprintf(game_logs, "DEALER > 21, Player wins - %d\n\n", bet);
                bet = 0;
                card_return(&hand_player, &hand_player_split, &hand_dealer, &deck, split_flag);
                split_flag = 0; wins_value++;
                continue;
            }

            // If u have more than dealer
            if ((split_flag == 0 && (hand_player_sum > hand_dealer_sum)) ||
                (split_flag == 1 && (hand_player_sum > hand_dealer_sum || hand_player_split_sum > hand_dealer_sum))) {
                pool += bet * 2; bet *= 2;
                printf("U got more than dealer, u win %d\n", bet);
                fprintf(game_logs, "PLAYER > DEALER: Player wins %d\n\n", bet);
                bet = 0;
                card_return(&hand_player, &hand_player_split, &hand_dealer, &deck, split_flag);
                split_flag = 0; wins_value++;
                continue;
            }

            // If dealer == Player  (Push case)
            if ((split_flag == 0 && (hand_dealer_sum == hand_player_sum)) ||
                (split_flag == 1 &&
                 ((hand_dealer_sum == hand_player_sum) || (hand_dealer_sum == hand_player_split_sum)))) {
                puts("Push case, nobody wins");
                pool += bet;
                fprintf(game_logs, "PUSH: Player save his bet %d\n\n", bet);
                bet = 0;
                card_return(&hand_player, &hand_player_split, &hand_dealer, &deck, split_flag);
                split_flag = 0; pushes_value++;
                continue;
            }

            // If dealer have more than player
            if ((split_flag == 0 && (hand_dealer_sum > hand_player_sum)) ||
                (split_flag == 1 &&
                 ((hand_dealer_sum > hand_player_sum) || (hand_dealer_sum > hand_player_split_sum)))) {
                bet = 0;
                fprintf(game_logs, "PLAYER < DEALER: Player loses\n\n");
                puts("Dealer have more than player, player lose");
                card_return(&hand_player, &hand_player_split, &hand_dealer, &deck, split_flag);
                split_flag = 0; loses_value++;
                continue;
            }
        } else if (strcmp(answer, "no") == 0) {
            fprintf(game_logs, "\nTotal games played - %d\n", total_games_value);
            fprintf(game_logs, "WINS: %d\n", wins_value);
            fprintf(game_logs, "LOSES: %d\n", loses_value);
            fprintf(game_logs, "PUSHES: %d\n", pushes_value);
            fprintf(game_logs, "REMAINING POOL: %d\n", pool);
            fclose(game_logs);
            exit(0);
        } else {
            printf("%s - invalid answer, try again\n", answer);
        }
    }
}
