#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "leaderboard.h"

int max(int a, int b) { return a > b ? a : b; }
int get_height(Node *n) { return n ? n->height : 0; }

int player_count(PlayerNode *head) {
    int c = 0;
    while (head) { c++; head = head->next; }
    return c;
}

int get_size(Node *n) {
    return n ? n->size : 0;
}

void update(Node *n) {
    if (n)
        n->height = 1 + max(get_height(n->left), get_height(n->right)),
        n->size = get_size(n->left) + get_size(n->right) + player_count(n->players);
}

void add_or_update_player(PlayerNode **head, int id, const char *team) {
    PlayerNode *p = *head;
    while (p) {
        if (p->id == id) {
            strncpy(p->team, team, 63);
            p->team[63] = '\0';
            return;
        }
        p = p->next;
    }
    p = malloc(sizeof(PlayerNode));
    assert(p);
    p->id = id;
    strncpy(p->team, team, 63);
    p->team[63] = '\0';
    p->prev = NULL;
    p->next = *head;
    if (*head) (*head)->prev = p;
    *head = p;
}

Node* rotate_right(Node *y) {
    Node *x = y->left;
    y->left = x->right;
    x->right = y;
    update(y);
    update(x);
    return x;
}

Node* rotate_left(Node *x) {
    Node *y = x->right;
    x->right = y->left;
    y->left = x;
    update(x);
    update(y);
    return y;
}

int get_balance(Node *n) {
    return n ? get_height(n->left) - get_height(n->right) : 0;
}

Node* insert(Node *root, int id, const char *team, int score) {
    if (!root) {
        Node *n = malloc(sizeof(Node));
        assert(n);
        n->score = score;
        n->height = 1;
        n->left = n->right = NULL;
        n->players = NULL;
        add_or_update_player(&n->players, id, team);
        update(n);
        return n;
    }

    if (score < root->score)
        root->left = insert(root->left, id, team, score);
    else if (score > root->score)
        root->right = insert(root->right, id, team, score);
    else
        add_or_update_player(&root->players, id, team);

    update(root);

    int bal = get_balance(root);
    if (bal > 1 && score < root->left->score) return rotate_right(root);
    if (bal < -1 && score > root->right->score) return rotate_left(root);
    if (bal > 1 && score > root->left->score) {
        root->left = rotate_left(root->left);
        return rotate_right(root);
    }
    if (bal < -1 && score < root->right->score) {
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

int get_rank(Node *root, int score) {
    if (!root) return 0;
    if (score < root->score)
        return get_rank(root->left, score);
    else if (score > root->score)
        return get_size(root->left) + player_count(root->players) + get_rank(root->right, score);
    else
        return get_size(root->left) + player_count(root->players);
}

int get_player_rank(Node *root, int id) {
    int score = find_player_score(root, id);
    if (score == -1) return -1;
    int total = get_size(root);
    int lower_or_equal = get_rank(root, score);
    return total - lower_or_equal + 1;
}

void top_k(Node *root, int *k) {
    if (!root || *k <= 0) return;
    top_k(root->right, k);
    PlayerNode *p = root->players;
    while (p && *k > 0) {
        printf("ID %d | Team %s | Score %d\n", p->id, p->team, root->score);
        p = p->next;
        (*k)--;
    }
    top_k(root->left, k);
}

void range_query(Node *root, int low, int high) {
    if (!root) return;
    if (root->score > low)
        range_query(root->left, low, high);
    if (root->score >= low && root->score <= high) {
        PlayerNode *p = root->players;
        while (p) {
            printf("ID %d | Team %s | Score %d\n", p->id, p->team, root->score);
            p = p->next;
        }
    }
    if (root->score < high)
        range_query(root->right, low, high);
}

