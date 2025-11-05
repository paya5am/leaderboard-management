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
    int score;
    int height;
    int size;
    PlayerNode *players;
    struct Node *left;
    struct Node *right;
} Node;


Node* insert(Node *root, int id, const char *team, int score);
int find_player_score(Node *root, int id);


int get_player_rank(Node *root, int id);


void top_k(Node *root, int *k);
void range_query(Node *root, int low, int high);

#endif
