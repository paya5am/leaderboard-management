# Real-Time Leaderboard

A real-time leaderboard system that supports efficient score updates, rank retrieval, top-K queries, and range queries. The implementation focuses on maintaining correct ordering and handling ties.

## Key Concepts

- Balanced Binary Search Tree (AVL / Red-Black Tree)
  Used to maintain scores in sorted order and support rank queries in logarithmic time.

- Doubly Linked List / Array
  Maintains a list of players and allows quick traversal when needed.

- Order Statistics
  Subtree sizes are used to compute ranks efficiently.

## Features

- Add or update a player's score.
- Get the current rank of any player.
- Retrieve top-K players.
- Range query: list players within a score interval.
- Handles ties correctly.

## Requirements

- GCC or any standard C compiler
- Standard C Libraries (stdio.h, stdlib.h, string.h, etc.)
- No external libraries required.

## File Structure

leaderboard.h      // Data structure definitions and function declarations  
leaderboard.c      // Balanced BST and leaderboard operations  
main.c             // Driver / I/O handling  

## Compilation

gcc main.c leaderboard.c -o leaderboard

## Run

./leaderboard

The program will display menu options to add scores, query ranks, retrieve top-K results, and perform range searches.

## Notes

- Average time complexity for operations is O(log n), due to balanced BST usage.
- Suitable for systems with frequent score updates where accurate ranking must be maintained.
