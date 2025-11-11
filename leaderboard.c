#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdio.h>
#include "leaderboard.h"

int max(int a, int b) { return a > b ? a : b; }

int player_count(PlayerNode *head) {
    int c = 0;
    while (head) { c++; head = head->next; }
    return c;
}

int get_size(Node *n) {
    return n ? n->size : 0;
}

static int get_height(Node *n) { return n ? n->height : 0; }

static void update(Node *n) {
    if (!n) return;
    n->height = 1 + max(get_height(n->left), get_height(n->right));
    n->size = player_count(n->players) + get_size(n->left) + get_size(n->right);
}

static void add_or_update_player(PlayerNode **head, int id, const char *team) {
    PlayerNode *p = *head;
    while (p) {
        if (p->id == id) {
            /* update team */
            strncpy(p->team, team, sizeof(p->team) - 1);
            p->team[sizeof(p->team) - 1] = '\0';
            return;
        }
        p = p->next;
    }
    /* insert at head */
    PlayerNode *n = malloc(sizeof(PlayerNode));
    assert(n);
    n->id = id;
    strncpy(n->team, team, sizeof(n->team) - 1);
    n->team[sizeof(n->team) - 1] = '\0';
    n->prev = NULL;
    n->next = *head;
    if (*head) (*head)->prev = n;
    *head = n;
}

static Node* rotate_right(Node *y) {
    Node *x = y->left;
    Node *T2 = x->right;
    x->right = y;
    y->left = T2;
    update(y);
    update(x);
    return x;
}

static Node* rotate_left(Node *x) {
    Node *y = x->right;
    Node *T2 = y->left;
    y->left = x;
    x->right = T2;
    update(x);
    update(y);
    return y;
}

static int get_balance(Node *n) {
    return n ? (get_height(n->left) - get_height(n->right)) : 0;
}

/*  players stored in DLL inside node  */

Node* insert(Node *root, int id, const char *team, int score) {
    if (!root) {
        Node *n = malloc(sizeof(Node));
        assert(n);
        n->score = score;
        n->height = 1;
        n->size = 0; /* to be updated */
        n->left = n->right = NULL;
        n->players = NULL;
        add_or_update_player(&n->players, id, team);
        update(n);
        return n;
    }

    if (score < root->score) {
        root->left = insert(root->left, id, team, score);
    } else if (score > root->score) {
        root->right = insert(root->right, id, team, score);
    } else {
        /* same score bucket: insert or update player in DLL */
        add_or_update_player(&root->players, id, team);
    }

    update(root);

    int balance = get_balance(root);

    if (balance > 1 && score < root->left->score)
        return rotate_right(root);
    if (balance < -1 && score > root->right->score)
        return rotate_left(root);
    if (balance > 1 && score > root->left->score) {
        root->left = rotate_left(root->left);
        return rotate_right(root);
    }
    if (balance < -1 && score < root->right->score) {
        root->right = rotate_right(root->right);
        return rotate_left(root);
    }
    return root;
}

int find_player_score(Node *root, int id) {
    if (!root) return -1;
    PlayerNode *p = root->players;
    while (p) {
        if (p->id == id) return root->score;
        p = p->next;
    }
    int left = find_player_score(root->left, id);
    if (left != -1) return left;
    return find_player_score(root->right, id);
}

const char *get_player_team(Node *root, int id) {
    if (!root) return NULL;
    PlayerNode *p = root->players;
    while (p) {
        if (p->id == id) return p->team;
        p = p->next;
    }
    const char *left = get_player_team(root->left, id);
    if (left) return left;
    return get_player_team(root->right, id);
}

static int count_leq(Node *root, int score) {
    if (!root) return 0;
    if (score < root->score) {
        return count_leq(root->left, score);
    } else if (score > root->score) {
        return get_size(root->left) + player_count(root->players) + count_leq(root->right, score);
    } else {
        return get_size(root->left) + player_count(root->players);
    }
}

int get_player_rank(Node *root, int id) {
    int score = find_player_score(root, id);
    if (score == -1) return -1;
    int total = get_size(root);
    int leq = count_leq(root, score);
    /* rank 1 == highest */
    return total - leq + 1;
}

static void topk_rec(Node *root, int *k_left, int *printed) {
    if (!root || *k_left <= 0) return;
    topk_rec(root->right, k_left, printed);
    if (*k_left <= 0) return;
    PlayerNode *p = root->players;
    while (p && *k_left > 0) {
        int rank = get_player_rank(root, p->id);
        printf("Rank %d | ID %d | Team %s | Score %d\n",
               rank, p->id, p->team, root->score);
        p = p->next;
        (*k_left)--;
        (*printed)++;
    }
    if (*k_left <= 0) return;
    topk_rec(root->left, k_left, printed);
}

int top_k(Node *root, int k) {
    if (!root || k <= 0) return 0;
    int printed = 0;
    topk_rec(root, &k, &printed);
    return printed;
}

static int range_rec(Node *root, int low, int high) {
    if (!root) return 0;
    int count = 0;
    if (root->score > low) count += range_rec(root->left, low, high);
    if (root->score >= low && root->score <= high) {
        PlayerNode *p = root->players;
        while (p) {
            int rank = get_player_rank(root, p->id);
            printf("Rank %d | ID %d | Team %s | Score %d\n",
                   rank, p->id, p->team, root->score);
            p = p->next;
            count++;
        }
    }
    if (root->score < high) count += range_rec(root->right, low, high);
    return count;
}

int range_query(Node *root, int low, int high) {
    if (low > high) return 0;
    return range_rec(root, low, high);
}

static int display_all_rec(Node *root) {
    if (!root) return 0;
    int count = 0;
    count += display_all_rec(root->right);
    PlayerNode *p = root->players;
    while (p) {
        int rank = get_player_rank(root, p->id);
        printf("Rank %d | ID %d | Team %s | Score %d\n",
               rank, p->id, p->team, root->score);
        p = p->next;
        count++;
    }
    count += display_all_rec(root->left);
    return count;
}

int display_all(Node *root) {
    return display_all_rec(root);
}

