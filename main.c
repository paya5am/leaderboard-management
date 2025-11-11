#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "leaderboard.h"
#include <ctype.h>

static int read_line(char *buf, size_t n) {
    if (!fgets(buf, n, stdin)) return 0;
    buf[strcspn(buf, "\n")] = 0;
    return 1;
}

/* Fixed: read_int handles invalid (non-numeric) input safely */
static int read_int(const char *prompt, int *out) {
    char line[256];
    char *end;
    long val;

    printf("%s", prompt);
    if (!read_line(line, sizeof(line))) return 0;

    if (line[0] == '\0') return 0;
    for (int i = 0; line[i]; i++) {
        if (!isdigit(line[i]) && line[i] != '-' && line[i] != '+') {
            printf("Invalid input.\n");
            return 0;
        }
    }

    val = strtol(line, &end, 10);
    if (*end != '\0') {
        printf("Invalid input.\n");
        return 0;
    }

    *out = (int)val;
    return 1;
}

int main() {
    Node *root = NULL;
    int choice, id, score, low, high, k;
    char team[64];
    char line[256];

    for (;;) {
        printf("\n--- Leaderboard Menu ---\n");
        printf("1. Add / Update player\n");
        printf("2. Get rank of player by ID\n");
        printf("3. Top-K players\n");
        printf("4. Range query by score\n");
        printf("5. Display all\n");
        printf("6. Exit\n");

        if (!read_int("Enter choice: ", &choice)) continue;

        if (choice == 1) {
            printf("Enter: ID Team/Player Score\n");
            if (!read_line(line, sizeof(line))) continue;

            char *p = line;
            char *end;
            long id_l = strtol(p, &end, 10);
            if (end == p) { printf("\n**Invalid input.**\n"); continue; }
            while (*end == ' ') end++;

            int i = 0;
            while (*end && !isspace(*end) && i < 63) team[i++] = *end++;
            team[i] = '\0';
            if (i == 0) {  printf("\n**Invalid input.**\n"); continue; }
            while (*end == ' ') end++;

            long score_l = strtol(end, &end, 10);
            while (*end == ' ') end++;
            if (*end != '\0') { printf("\n**Invalid input.**\n"); continue; }

            id = (int)id_l;
            score = (int)score_l;

            root = insert(root, id, team, score);
            printf("**ACCEPTED**\n");
        }

        else if (choice == 2) {
            if (!read_int("Enter ID: ", &id)) continue;
            int r = get_player_rank(root, id);
            if (r == -1) printf("No records found.\n");
            else printf("Rank: %d\n", r);
        }

        else if (choice == 3) {
            if (!read_int("Enter K: ", &k)) continue;
            if (k <= 0) { printf("\n**Invalid input.**\n"); continue; }
            printf("\n-- Top %d Players --\n", k);
            top_k(root, &k);
        }

        else if (choice == 4) {
            printf("Enter low high:\n");
            if (!read_line(line, sizeof(line))) continue;

            int items = sscanf(line, "%d %d", &low, &high);
            if (items != 2) {
                printf("\n**Invalid input.**\n");
                continue;
            }

            int count = range_query(root, low, high);
            if (count == 0)
                printf("No records found.\n");
        }

        else if (choice == 5) {
            int count = display_all(root);
            if (count == 0)
                printf("No records found.\n");
        }

        else if (choice == 6) {
            return 0;
        }

        else {
            printf("\n**Invalid option.**\n");
        }
    }
}

