#include <stdio.h>
#include "leaderboard.h"

int main() {
    Node *root = NULL;
    int choice, id, score, low, high, k;
    char team[64];

    while (1) {
        printf("\n--- Leaderboard Menu ---\n");
        printf("1. Add / Update player\n");
        printf("2. Get rank of player by ID\n");
        printf("3. Top-K players\n");
        printf("4. Range query by score\n");
        printf("5. Exit\n");
        printf("Enter choice: ");
        if (scanf("%d", &choice) != 1) break;

        if (choice == 1) {
            printf("Enter ID, Team, Score: ");
            scanf("%d %63s %d", &id, team, &score);
            root = insert(root, id, team, score);
        }
        else if (choice == 2) {
            printf("Enter player ID: ");
            scanf("%d", &id);
            int rank = get_player_rank(root, id);
            if (rank == -1) printf("Player not found.\n");
            else printf("Rank: %d\n", rank);
        }
        else if (choice == 3) {
            printf("Enter K: ");
            scanf("%d", &k);
            top_k(root, &k);
        }
        else if (choice == 4) {
            printf("Enter score range (low high): ");
            scanf("%d %d", &low, &high);
            range_query(root, low, high);
        }
        else if (choice == 5) break;
        else printf("Invalid option.\n");
    }
    return 0;
}

