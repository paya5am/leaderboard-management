#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "leaderboard.h"

static int read_line(char *buf, size_t n) {
    if (!fgets(buf, n, stdin)) return 0;
    buf[strcspn(buf, "\n")] = 0;
    return 1;
}
static int read_int(const char *prompt, int *out) {
    char line[256];
    char *end;
    long val;

    printf("%s", prompt);
    if (!read_line(line, sizeof(line))) return 0;
    if (line[0] == '\0') return 0;

    val = strtol(line, &end, 10);
    /* skip spaces after number */
    while (*end == ' ') end++;
    if (*end != '\0') {
        printf("Invalid input.\n");
        return 0;
    }
    *out = (int)val;
    return 1;
}

int main(void) {
    Node *root = NULL;
    char line[256];
    int choice;

    for (;;) {
        printf("\n--- Leaderboard Menu ---\n");
        printf("1. Add / Update player\n");
        printf("2. Get rank of player by ID\n");
        printf("3. Top-K players\n");
        printf("4. Range query by score\n");
        printf("5. Display all players\n");
        printf("6. Exit\n");

        if (!read_int("Enter choice: ", &choice)) continue;

        if (choice == 1) {
            printf("Enter: ID Team Score  (example: 7 Alpha 120)\n");
            if (!read_line(line, sizeof(line))) continue;

            char *p = line;
            char *end;
            long id_l = strtol(p, &end, 10);
            if (end == p) { printf("Invalid input.\n"); continue; }
            while (*end == ' ') end++;

            /* read team as next token (no spaces in name) */
            char team[64];
            int i = 0;
            while (*end && !isspace((unsigned char)*end) && i < (int)sizeof(team) - 1) {
                team[i++] = *end++;
            }
            team[i] = '\0';
            if (i == 0) { printf("Invalid input.\n"); continue; }
            while (*end == ' ') end++;

            long score_l = strtol(end, &end, 10);
            while (*end == ' ') end++;
            if (*end != '\0') { printf("Invalid input.\n"); continue; }

            int id = (int)id_l;
            int score = (int)score_l;

            root = insert(root, id, team, score);
            printf("Added/Updated player: ID=%d Team=%s Score=%d\n", id, team, score);
        }

        else if (choice == 2) {
            int id;
            if (!read_int("Enter Player ID: ", &id)) continue;
            int rank = get_player_rank(root, id);
            if (rank == -1) {
                printf("No record found.\n");
                continue;
            }
            int score = find_player_score(root, id);
            const char *team = get_player_team(root, id);
            printf("Player ID: %d | Team: %s | Score: %d | Rank: %d\n",
                   id, team ? team : "Unknown", score, rank);
        }

        else if (choice == 3) {
            int k;
            if (!read_int("Enter K: ", &k)) continue;
            if (k <= 0) { printf("Invalid input.\n"); continue; }
            int printed = top_k(root, k);
            if (printed == 0) printf("No records found.\n");
        }

        else if (choice == 4) {
            printf("Enter low high (example: 50 200):\n");
            if (!read_line(line, sizeof(line))) continue;
            int low, high;
            if (sscanf(line, "%d %d", &low, &high) != 2) { printf("Invalid input.\n"); continue; }
            int printed = range_query(root, low, high);
            if (printed == 0) printf("No records found.\n");
        }

        else if (choice == 5) {
            int printed = display_all(root);
            if (printed == 0) printf("No records found.\n");
        }

        else if (choice == 6) {
            printf("Exiting.\n");
            break;
        }

        else {
            printf("Invalid option.\n");
        }
    }

    return 0;
}

