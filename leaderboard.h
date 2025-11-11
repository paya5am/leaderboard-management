#ifndef LEADERBOARD_H
#define LEADERBOARD_H

#include <stdio.h>

typedef struct PlayerNode {
    int id;
    char team[64];
    struct PlayerNode *prev;
    struct PlayerNode *next;
} PlayerNode;

typedef struct Node {
    int score;              /* key */
    int height;             /* for AVL balancing */
    int size;               /* total players in subtree (sum of player_count in nodes) */
    PlayerNode *players;    /* head of doubly linked list of players with this score */
    struct Node *left;
    struct Node *right;
} Node;


Node* insert(Node *root, int id, const char *team, int score);

int find_player_score(Node *root, int id);          /* returns score or -1 */
const char *get_player_team(Node *root, int id);    /* returns pointer to team string in tree or NULL */
int get_player_rank(Node *root, int id);            /* returns 1-based rank (1 = highest) or -1 if not found */

int top_k(Node *root, int k);
int range_query(Node *root, int low, int high);
int display_all(Node *root);

int get_size(Node *root);

#endif
