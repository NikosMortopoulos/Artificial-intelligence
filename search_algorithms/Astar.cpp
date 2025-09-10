/*
    A* Search Implementation
    Author: Nikos Mortopoulos
*/

#include <iostream>
#include <queue>
#include <vector>
#include <set>
#include <string>
#include <cmath>

using namespace std;

struct node {
    int state[7];
    struct node* parent;
    int cost;
    int astar_cost;  // f = g + h
    int depth;
};

int total_nodes;

// Encode state into a string for visited set
string encode(int s[7]) {
    string k; k.reserve(7);
    for (int i = 0; i < 7; ++i) k.push_back(char('0' + s[i]));
    return k;
}
set<string> visited;

// Goal check: [1,1,1,0,2,2,2]
bool checkgoal(struct node* node) {
    int goal[7] = {1,1,1,0,2,2,2};
    for (int i = 0; i < 7; ++i) {
        if (node->state[i] != goal[i]) return false;
    }
    return true;
}

// Comparator for priority queue
struct CompareCost {
    bool operator()(const node* a, const node* b) const {
        if (a->astar_cost != b->astar_cost) {
            return a->astar_cost > b->astar_cost;
        } else {
            return rand() % 2;
        }
    }
};

// Print solution path
void displayPath(struct node* goalNode) {
    if (goalNode == nullptr) return;
    displayPath(goalNode->parent);
    for (int i = 0; i < 7; i++) {
        cout << goalNode->state[i] << " ";
    }
    cout << "| Cost: " << goalNode->cost << endl;
}

// Heuristic: distance of white pieces needing to move left of first black
int calculate_astar_cost(struct node* currentnode) {
    int heuristic = 0;
    int leftmost_black_index = -1;

    for (int i = 0; i < 7; i++) {
        if (currentnode->state[i] == 2) {
            leftmost_black_index = i;
            break;
        }
    }
    if (leftmost_black_index == -1) return INT_MAX;

    for (int i = 0; i < 7; i++) {
        if (currentnode->state[i] == 1 && i >= leftmost_black_index) {
            heuristic += (i - leftmost_black_index + 1);
        }
    }
    return heuristic;
}

// Generate successors
void expand(struct node* currentnode, priority_queue<node*, vector<node*>, CompareCost>& frontier) {
    int thesi_kenou = -1;
    for (int i = 0; i < 7; i++) {
        if (currentnode->state[i] == 0) {
            thesi_kenou = i;
            break;
        }
    }

    // Left moves
    for (int i = 1; i <= 3; i++) {
        int new_pos = thesi_kenou - i;
        int cost = i;
        if (new_pos >= 0) {
            node* new_node = new node;
            for (int j = 0; j < 7; j++) new_node->state[j] = currentnode->state[j];
            new_node->state[thesi_kenou] = new_node->state[new_pos];
            new_node->state[new_pos] = 0;
            new_node->parent = currentnode;
            new_node->cost = currentnode->cost + cost;
            new_node->depth = currentnode->depth + 1;
            new_node->astar_cost = calculate_astar_cost(new_node) + new_node->cost;

            string key = encode(new_node->state);
            if (!visited.count(key)) {
                visited.insert(key);
                frontier.push(new_node);
                total_nodes++;
            } else {
                delete new_node;
            }
        }
    }

    // Right moves
    for (int i = 1; i <= 3; i++) {
        int new_pos = thesi_kenou + i;
        int cost = i;
        if (new_pos < 7) {
            node* new_node = new node;
            for (int j = 0; j < 7; j++) new_node->state[j] = currentnode->state[j];
            new_node->state[thesi_kenou] = new_node->state[new_pos];
            new_node->state[new_pos] = 0;
            new_node->parent = currentnode;
            new_node->cost = currentnode->cost + cost;
            new_node->depth = currentnode->depth + 1;
            new_node->astar_cost = calculate_astar_cost(new_node) + new_node->cost;

            string key = encode(new_node->state);
            if (!visited.count(key)) {
                visited.insert(key);
                frontier.push(new_node);
                total_nodes++;
            } else {
                delete new_node;
            }
        }
    }
}

// A* search
void astar() {
    node* initial_state = new node;
    initial_state->state[0] = 2;
    initial_state->state[1] = 2;
    initial_state->state[2] = 2;
    initial_state->state[3] = 1;
    initial_state->state[4] = 1;
    initial_state->state[5] = 1;
    initial_state->state[6] = 0;
    initial_state->parent = nullptr;
    initial_state->cost = 0;
    initial_state->depth = 0;
    initial_state->astar_cost = calculate_astar_cost(initial_state);

    priority_queue<node*, vector<node*>, CompareCost> frontier;
    visited.clear();
    visited.insert(encode(initial_state->state));
    frontier.push(initial_state);

    while (!frontier.empty()) {
        node* current_node = frontier.top();
        frontier.pop();

        if (checkgoal(current_node)) {
            cout << "Solution found with cost: " << current_node->cost << endl;
            displayPath(current_node);
            cout << "Total nodes in search tree: " << total_nodes << endl;
            return;
        }
        expand(current_node, frontier);
    }

    cout << "No solution found." << endl;
    cout << "Total nodes in search tree: " << total_nodes << endl;
}

int main() {
    astar();
    return 0;
}
