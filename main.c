#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <ctype.h> //tolower func

/*
Notes
    pop_index(node, 0) not working help
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
void to_lower(char *string);
int len_list(node *head);
void show_cards(node *p1, node *p2, node *d, int split_flag);



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

// 0 index is not working
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
        printf("%d[%c] ", current -> card.num, current -> card.suit);
        current = current -> next;
    }
    printf("\n");
}


void deck_init(node **head) {
    // 3 suits, 10 nums + (3 pics = 10) + 1 ace cards
    for (int i = 3; i < 7; i++) {
        for (int j = 11; j > 1; j--) {
            card card;
            card.num = j; card.suit = i;
            push(head, card);
            if (j == 10) {
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
    node *current = head;
    int sum = 0, ace = 0;
    while (current != NULL) {
        sum += current -> card.num;
        current = current -> next;
    }

    return sum;
}

// Can be optimized somehow (
void show_cards(node *p1, node *p2, node *d, int split_flag) {
    if (split_flag == 1) {
        printf("Dealer hand: (%d) : ", get_hand_total(d));
        print_list(d);
        printf("First player hand: (%d) : ", get_hand_total(p1));
        print_list(p1);
        printf("Second player hand: (%d) : ", get_hand_total(p2));
        print_list(p2);

    } else {
        printf("Dealer hand (%d) : ", get_hand_total(d));
        print_list(d);
        printf("Player hand (%d) : ", get_hand_total(p1));
        print_list(p1);
    }
}


int main(void) {
    puts("Enter your bet:");
    int bet;
    scanf("%d", &bet);
    fflush (stdin);

    //Deck initialize, shuffle, print
    node *deck = NULL;
    deck_init(&deck);
    shuffle(deck);

    //Dealer hand initialize
    node *hand_dealer = NULL;
    push(&hand_dealer, pop(&deck));
    puts("Dealer hand"); print_list(hand_dealer);

    //Player hand initialize
    node *hand_player = NULL;
    push(&hand_player, pop(&deck));
    push(&hand_player, pop(&deck));
    puts("Player hand"); print_list(hand_player);

    // Split hand initialize
    node *hand_player_split = NULL;

    /*                  Game logic starts
    Ace is always 11
    Exit code 1 - Player win
    Exit code -1 - Dealer win
    Exit code 0 - Push case, nobody wins
    No insurance
    No soft 17, means if calc(dealer hand) < 16 then + 1 card
    HIT              push(&hand, pop(&deck));
    STAND            Player while break
    SPLIT            Get another player hand, split logic
    DOUBLE           x2 bet, +1 card, player while break
    SURRENDER        game over (2 cards, bet/2)             */

    char move[9]; int split_flag = 0;
    while (strcmp(move, "stand") != 0 || get_hand_total(hand_player) < 21) {

        puts("Hit, split, double, surrender, stand?");

        scanf("%s", move); to_lower(move);

        // HIT
        if (strcmp(move, "hit") == 0) {

            // If player have 2 hands case
            if (split_flag == 1) {
                puts("Choose on which hand u want to hit (Enter 1 for the first, 2 for the second)");
                int split_value = 0;
                scanf("%d", &split_value);
                if (split_value != 2 && split_value != 1) split_value = 1;  //Case if user enter incorrect value

                // Choose which hand
                if (split_value == 1) {
                    push(&hand_player, pop(&deck));
                } else if (split_value == 2) {
                    push(&hand_player_split, pop(&deck));
                }
                show_cards(hand_player, hand_player_split, hand_dealer, split_flag);

            } else {
                // Just get a new card
                push(&hand_player, pop(&deck));
                puts("U got a new card");
                show_cards(hand_player, hand_player_split, hand_dealer, split_flag);
            }
        }
        // DOUBLE
        else if (strcmp(move, "double") == 0) {
            if (split_flag ==1) {
                puts("Choose on which hand u want to double (Enter 1 for the first, 2 for the second)");
                int split_value = 0;
                scanf("%d", &split_value);
                if (split_value != 2 && split_value != 1) split_value = 1;  //Case if user enter incorrect value

                bet *=2;
                // Choose which hand
                if (split_value == 1) {
                    push(&hand_player, pop(&deck));
                    break;
                } else if (split_value == 2) {
                    push(&hand_player_split, pop(&deck));
                    break;
                }
                show_cards(hand_player, hand_player_split, hand_dealer, split_flag);

            }
            else if (split_flag == 0) {
                bet *= 2;
                printf("Player take a new card, bet is doubled - %d, no more player moves\n", bet);
                push(&hand_player, pop(&deck));
                show_cards(hand_player, hand_player_split, hand_dealer, split_flag);
                puts("");
                break;
            }
        }
        else if (strcmp(move, "surrender") == 0) {
            // If player have more than 2 cards case
            if (split_flag != 0 || len_list(hand_player) > 2) {
                puts("cant surrender, invalid case (needs value cards <= 2 and non split case)");
                continue;
            }
            // Player gets back half of his bet, game over
            bet /= 2;
            printf("Surrender, half of bet is saved - %d", bet);
            exit(-1);
        }
        // SPLIT
        else if (strcmp(move, "split") == 0) {
            // If player have 2 cards exactly
            if (len_list(hand_player) != 2) {
                puts("cant split, invalid value of cards (needs 2)");
                continue;
            }
            // Second player hand get one card from main player hand
            push(&hand_player_split, pop(&hand_player));
            split_flag = 1;
            show_cards(hand_player, hand_player_split, hand_dealer, split_flag); puts("");
        }
        else if (strcmp(move, "stand") == 0) {
            puts("Player stand, dealers move");
            break;
        }
        else {
            puts("wtf are u typing here, jesus, its blackjack we are dont printing tests here");
        }


        // If player takes his Blackjack:
        if ((split_flag == 0 && get_hand_total(hand_player) == 21) || (split_flag == 1 && (get_hand_total(hand_player) == 21 || get_hand_total(hand_player_split) == 21))) {
            printf("Blackjack!, take yours %d", bet * 1.5);
            exit(1);
        }


        // Player got more than 21, player lose
        if ((get_hand_total(hand_player) > 21) || (get_hand_total(hand_player_split)) > 21) {
            puts("U got more than 21, u lose");
            exit(-1);
        }
    }

    // When all player moves is done, dealer gets his cards
    while (get_hand_total(hand_dealer) < 16) {
        push(&hand_dealer, pop(&deck));
    }
    
  
    // Int values init for reduce function calls later
    show_cards(hand_player, hand_player_split, hand_dealer, split_flag);
    int hand_dealer_sum, hand_player_sum, hand_player_split_sum;
    hand_dealer_sum = get_hand_total(hand_dealer);
    hand_player_sum = get_hand_total(hand_player);
    hand_player_split_sum = get_hand_total(hand_player_split);


    //Dealer got blackjack, if u too - ure win, if not - u lose.
    if (hand_dealer_sum == 21) {
        if ((split_flag == 1 && (hand_player_sum == 21 || hand_player_split_sum == 21)) || (split_flag == 0 && hand_player_sum == 21)) {
            printf("Dealer = Player, take yours %d", bet*1.5);
        } else {
            puts("Dealer got Blackjack. Shit happens, ure lose");
            exit(-1);
        }
    }

    //If dealer got  more than 21 u win
    if (hand_dealer_sum > 21) {
        printf("Dealer got more than 21, u win %d", bet*2);
        exit(1);
    }

    // If u have more than dealer
    if ((split_flag == 0 && (hand_player_sum > hand_dealer_sum)) ||
    (split_flag == 1 && (hand_player_sum > hand_dealer_sum || hand_player_split_sum > hand_dealer_sum))) {
        printf("U got more than dealer, u win %d", bet*2);
        exit(1);
    }

    // If dealer == Player  (Push case)
    if ((split_flag == 0 && (hand_dealer_sum == hand_player_sum)) ||
        (split_flag == 1 && ((hand_dealer_sum == hand_player_sum) || (hand_dealer_sum == hand_player_split_sum)))) {
        puts("Push case, nobody wins");
        exit(0);
    }

    // If dealer have more than player
    if ((split_flag == 0 && (hand_dealer_sum > hand_player_sum)) ||
    (split_flag == 1 && ((hand_dealer_sum > hand_player_sum) || (hand_dealer_sum > hand_player_split_sum)))) {
        puts("Dealer have more than player, player lose");
        exit(-1);
    }
}
