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

 int get_height(Node *n) { return n ? n->height : 0; }

 void update(Node *n) {
    if (!n) return;
    n->height = 1 + max(get_height(n->left), get_height(n->right));
    n->size = player_count(n->players) + get_size(n->left) + get_size(n->right);
}

 Node* rotate_right(Node *y) {
    Node *x = y->left;
    Node *T2 = x->right;
    x->right = y;
    y->left = T2;
    update(y);
    update(x);
    return x;
}

 Node* rotate_left(Node *x) {
    Node *y = x->right;
    Node *T2 = y->left;
    y->left = x;
    x->right = T2;
    update(x);
    update(y);
    return y;
}

 int get_balance(Node *n) {
    return n ? (get_height(n->left) - get_height(n->right)) : 0;
}

/* remove player from a DLL by id */
 void remove_player(PlayerNode **head, int id) {
    PlayerNode *p = *head;
    while (p) {
        if (p->id == id) {
            if (p->prev) p->prev->next = p->next;
            if (p->next) p->next->prev = p->prev;
            if (p == *head) *head = p->next;
            free(p);
            return;
        }
        p = p->next;
    }
}

/* move player: remove old score node if empty */
 Node* delete_player(Node *root, int id, int score) {
    if (!root) return NULL;

    if (score < root->score)
        root->left = delete_player(root->left, id, score);
    else if (score > root->score)
        root->right = delete_player(root->right, id, score);
    else {
        remove_player(&root->players, id);
        if (!root->players) {
            if (!root->left) {
                Node *r = root->right;
                free(root);
                return r;
            } else if (!root->right) {
                Node *l = root->left;
                free(root);
                return l;
            } else {
                Node *succ = root->right;
                while (succ->left) succ = succ->left;
                root->score = succ->score;
                root->players = succ->players;
                succ->players = NULL;
                root->right = delete_player(root->right, id, root->score);
            }
        }
    }

    update(root);
    int balance = get_balance(root);
    if (balance > 1 && get_balance(root->left) >= 0)
        return rotate_right(root);
    if (balance > 1 && get_balance(root->left) < 0) {
        root->left = rotate_left(root->left);
        return rotate_right(root);
    }
    if (balance < -1 && get_balance(root->right) <= 0)
        return rotate_left(root);
    if (balance < -1 && get_balance(root->right) > 0) {
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

 void add_player(PlayerNode **head, int id, const char *team) {
    PlayerNode *n = malloc(sizeof(PlayerNode));
    n->id = id;
    strncpy(n->team, team, sizeof(n->team) - 1);
    n->team[sizeof(n->team) - 1] = '\0';
    n->prev = NULL;
    n->next = *head;
    if (*head) (*head)->prev = n;
    *head = n;
}

Node* insert(Node *root, int id, const char *team, int score) {
    int existing_score = find_player_score(root, id);
    if (existing_score != -1 && existing_score != score)
        root = delete_player(root, id, existing_score);

    if (!root) {
        Node *n = malloc(sizeof(Node));
        n->score = score;
        n->height = 1;
        n->size = 0;
        n->left = n->right = NULL;
        n->players = NULL;
        add_player(&n->players, id, team);
        update(n);
        return n;
    }

    if (score < root->score)
        root->left = insert(root->left, id, team, score);
    else if (score > root->score)
        root->right = insert(root->right, id, team, score);
    else {
        remove_player(&root->players, id);
        add_player(&root->players, id, team);
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

 int count_greater(Node *root, int score) {
    if (!root) return 0;
    if (score < root->score)
        return get_size(root->right) + player_count(root->players) + count_greater(root->left, score);
    if (score > root->score)
        return count_greater(root->right, score);
    return get_size(root->right);
}

int get_player_rank(Node *root, int id) {
    int score = find_player_score(root, id);
    if (score == -1) return -1;
    return count_greater(root, score) + 1;
}

 void topk_rec(Node *root, int *k_left, Node *root_ref) {
    if (!root || *k_left <= 0) return;
    topk_rec(root->right, k_left, root_ref);
    PlayerNode *p = root->players;
    while (p && *k_left > 0) {
        int rank = get_player_rank(root_ref, p->id);
        printf("Rank %d | ID %d | Team %s | Score %d\n", rank, p->id, p->team, root->score);
        p = p->next;
        (*k_left)--;
    }
    topk_rec(root->left, k_left, root_ref);
}

int top_k(Node *root, int k) {
    if (!root || k <= 0) return 0;
    int left = k;
    topk_rec(root, &left, root);
    return k - left;
}

 int display_all_rec(Node *root, Node *root_ref) {
    if (!root) return 0;
    int c = 0;
    c += display_all_rec(root->right, root_ref);
    PlayerNode *p = root->players;
    while (p) {
        int rank = get_player_rank(root_ref, p->id);
        printf("Rank %d | ID %d | Team %s | Score %d\n", rank, p->id, p->team, root->score);
        p = p->next;
        c++;
    }
    c += display_all_rec(root->left, root_ref);
    return c;
}

int display_all(Node *root) {
    return display_all_rec(root, root);
}

int range_query(Node *root, int low, int high, Node *global_root) {
    if (!root) return 0;
    int count = 0;
    if (root->score > low)
        count += range_query(root->left, low, high, global_root);
    if (root->score >= low && root->score <= high) {
        PlayerNode *p = root->players;
        while (p) {
            int rank = get_player_rank(global_root, p->id); 
            printf("Rank %d | ID %d | Team %s | Score %d\n",
                   rank, p->id, p->team, root->score);
            p = p->next;
            count++;
        }
    }
    if (root->score < high)
        count += range_query(root->right, low, high, global_root);
    return count;
}
void save_rec(Node *node, FILE *file) {
        if (!node) return;
        save_rec(node->right, file);
        PlayerNode *p = node->players;
        while (p) {
            fprintf(file, "%d,%s,%d\n", p->id, p->team, node->score);
            p = p->next;
        }
        save_rec(node->left, file);
    }

Node* load_from_csv(const char *filename) {
    FILE *fp = fopen(filename, "r");
    if (!fp) {
        printf("No existing data file found: %s\n", filename);
        return NULL;
    }

    Node *root = NULL;
    int id, score;
    char team[64];
    int found = 0;

    while (fscanf(fp, "%d,%63[^,],%d", &id, team, &score) == 3) {
        root = insert(root, id, team, score);
        found = 1;
    }

    fclose(fp);

    if (!found) {
        printf("File %s is empty or invalid.\n", filename);
        return NULL;
    }

    printf("Data loaded successfully from %s.\n", filename);
    return root;
}

int save_to_csv(Node *root, const char *filename) {
    if (!root) {
        printf("Nothing available to save.\n");
        return 0;
    }

    FILE *fp = fopen(filename, "a"); // append mode
    if (!fp) {
        printf("Error: Unable to open %s for writing.\n", filename);
        return 0;
    }
    
    save_rec(root, fp);
    fclose(fp);
    printf("Snapshot saved successfully to %s.\n", filename);
    return 1;
}
int clear_csv(const char *filename) {
    FILE *fp = fopen(filename, "w");  
    if (!fp) {
        printf("Error: Unable to clear %s.\n", filename);
        return 0;
    }
    fclose(fp);
    printf("%s cleared successfully.\n", filename);
    return 1;
}

