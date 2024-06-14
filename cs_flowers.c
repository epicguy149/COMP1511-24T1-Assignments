// cs_flowers.c
//
// This program was written by [Chai Jengyi] (z5524645)
// on [2024-03-05]
//
// TODO: Assignment 1

#include <stdio.h>
#include <math.h>

//------------------------------------------------------------------------------
// Constants
//------------------------------------------------------------------------------

#define MAX_ROW 11
#define MAX_COL 11
#define TRUE 1
#define FALSE 0

// TODO: put your constants here 

//------------------------------------------------------------------------------
// Struct & Enum
//------------------------------------------------------------------------------

enum flower_state {
    NONE,
    DORMANT,
    DISTURBED,
    AWAKENED,
    ANXIOUS,
    ENERGETIC
};

struct flower {
    enum flower_state state;
    int root_count; 
    int cooldown; 
    int anxious_timer; // to track no. of turns to turn anxious
    int energy_count; // to track no. of turns to turn energetic
};

enum tile_type {
    EMPTY,
    BUSH,
    BRANCH,
    DANGER,
    ROOT,
    POLLEN,
    SEEDLING,
    FLOWER,
};

struct tile {
    enum tile_type type;
    struct flower flower;
    int seedling_growth; 
    int interacted; // to mark a tile as interacted (e.g. cut)
    int interacted2;  // to handle flowers turning anxious right after awakening
    int root_type; // to keep track of every 3rd root in underground
};

//------------------------------------------------------------------------------
// Your Function Prototypes
//------------------------------------------------------------------------------

// check boundaries
int check_boundary(char boundary_type, 
                   int start_row, 
                   int start_col);

// check win/loss conditions every round
int win_loss_check(struct tile map[MAX_ROW][MAX_COL],
                    int player_row,
                    int player_col); 

// adds foliage to map based on user input
int add_foliage(struct tile map[MAX_ROW][MAX_COL], 
                char foliage_type,
                int start_row, 
                int start_col, 
                char orientation, 
                int length);

// adds flowers to map based on user input
void add_flowers(struct tile map[MAX_ROW][MAX_COL], 
                int start_row,
                int start_col, 
                int flower_count);

// executes move commands
int move_command(struct tile map[MAX_ROW][MAX_COL], 
                int player_row, 
                int player_col, 
                char command, 
                int *new_row, 
                int *new_col);

// executes cut command, returns 1 if energetic flower is cut
int cut_command(struct tile map[MAX_ROW][MAX_COL], 
                int player_row, 
                int player_col, 
                char direction, 
                int *energetic_row, 
                int *energetic_col);

// alerts flowers
void alert(struct tile map[MAX_ROW][MAX_COL], 
           int target_row,
           int target_col);

// sets flower state for alert
void trigger_flower(struct tile map[MAX_ROW][MAX_COL],
                    int row,
                    int col);

// executes flower action
void flower_action(struct tile map[MAX_ROW][MAX_COL], int player_row,
                int player_col);

// places bush around flowers
void place_bush(struct tile map[MAX_ROW][MAX_COL], 
                int player_row,
                int player_col);

// loop that sets tile to BUSH 
void set_bush_tile(struct tile map[MAX_ROW][MAX_COL],
                   int row,
                   int col,
                   int player_row,
                   int player_col);

// executes upkeep, to be called every turn
void upkeep(struct tile map[MAX_ROW][MAX_COL]);

// handles upkeep for SEEDLING tiles
void seedling_upkeep(struct tile map[MAX_ROW][MAX_COL],
                     int row,
                     int col);

// handles upkeep for ANXIOUS FLOWERS
void anxious_upkeep(struct tile map[MAX_ROW][MAX_COL],
                    int row,
                    int col);

// checks if tile meets conditions to become ANXIOUS and 
// sets to ANXIOUS if yes
void set_anxious(struct tile map[MAX_ROW][MAX_COL],
                 int row,
                 int col ,
                 int player_row,
                 int player_col);

// turns flower(s) into anxious state
void anxious_flower(struct tile map[MAX_ROW][MAX_COL],
                    int player_row, 
                    int player_col);

// spawn seedlings
void spawn_seedlings(struct tile map [MAX_ROW][MAX_COL], 
                    int flower_row,
                    int flower_col, 
                    int player_row, 
                    int player_col);

void spawn_pollen(struct tile map [MAX_ROW][MAX_COL],
                  int row,
                  int col);

// rotates pollen around anxious flower
void rotate_pollen(struct tile map [MAX_ROW][MAX_COL],
                   int flower_row,
                   int flower_col);

// remove pollen once anxious flower is cut
void remove_pollen(struct tile map [MAX_ROW][MAX_COL],
                   int flower_row,
                   int flower_col);

// checks if flowers meet condition to turn energetic
void energetic_check(struct tile map [MAX_ROW][MAX_COL]);

// puts player into underground map
int start_underground(struct tile underground[MAX_ROW][MAX_COL],
                      int flower_row,
                      int flower_col,
                      int player_row,
                      int player_col
);

// handles spawning of roots in underground map
int underground_roots(struct tile underground[MAX_ROW][MAX_COL],
                      int player_row,
                      int player_col,
                      int root_count,
                      int direction
);

// handles removal of roots/flower after player leaves underground
void remove_roots(struct tile underground[MAX_ROW][MAX_COL]);

// prompts for foliage and flower input
void input_elements(struct tile underground[MAX_ROW][MAX_COL],
                    int player_row,
                    int player_col);

// handles player commands
int player_command(struct tile map[MAX_ROW][MAX_COL],
                   int player_row,
                   int player_col,
                   struct tile underground[MAX_ROW][MAX_COL]);

//------------------------------------------------------------------------------
// Provided Function Prototypes
//------------------------------------------------------------------------------

void initialise_map(struct tile map[MAX_ROW][MAX_COL]);
void print_map(
    struct tile map[MAX_ROW][MAX_COL], 
    int player_row, 
    int player_col
);
void print_tile(struct tile tile);
void print_flower(struct flower flower);

//------------------------------------------------------------------------------
// Main Function
//------------------------------------------------------------------------------

int main(void) {
    struct tile map[MAX_ROW][MAX_COL];
    initialise_map(map);
    struct tile underground[MAX_ROW][MAX_COL];
    initialise_map(underground);

    printf("Welcome to CS Flowers!\n");

    // handle player spawning
    int player_row, player_col;
    int position_valid = 0;

    printf("Player's starting position: ");
    scanf("%d %d", &player_row, &player_col);

    while (position_valid != 1) {
        // check if player row&col is in the outer ('o') boundary 
        if (check_boundary('o', player_row, player_col)) {
            // if it is in outer boundary, position is accepted, map printed
            position_valid = 1;
            print_map(map, player_row, player_col);
        } else {
            printf("Invalid starting position!\n");
            printf("Re-enter starting position: ");
            scanf("%d %d", &player_row, &player_col);
        }
    }

    // input foliage and flowers
    input_elements(map, player_row, player_col);

    printf("Game Started!\n");
    
    // handles player commands each turn and checks win/loss conditions after each turn
    player_command(map, player_row, player_col, underground);
    
    return 0;
}

//------------------------------------------------------------------------------
// Your Function
//------------------------------------------------------------------------------

// TODO: Put your functions here

// executes move/cut command based on user input, and change map based on effects 
// of command also checks if win/loss conditions met at the end of the turn
int player_command(struct tile map[MAX_ROW][MAX_COL],
                   int player_row,
                   int player_col,
                   struct tile underground[MAX_ROW][MAX_COL]
) {
    char command, direction;
    int new_row, new_col;
    int energetic_row = 0;
    int energetic_col = 0;
    int outcome = 0;

    while (1) {
        printf("Enter command: ");
        // check for EOF
        if (scanf(" %c", &command) != 1) {
            return 0;
        }
        // calls functions based on user command, cut or move
        if (command == 'c') {
            // cut
            if (scanf(" %c", &direction) == 1) {
                // cut_command returns 1 only if energetic flower is cut, so if
                // 1, start underground phase
                if (cut_command(map, player_row, player_col, direction, 
                                &energetic_row, &energetic_col)) {

                    outcome = start_underground(underground, energetic_row, energetic_col, 
                                                player_row, player_col);

                    // outcome is 1 or 2 if player dies or EOF input by user
                    // which are both game ending conditions, so end game 
                    if (outcome == 1 || outcome == 2) {
                        return 0;
                    }
                }
            }
        } else {
            // move
            if (move_command(map, player_row, player_col, command, &new_row, &new_col)) {
                // move is valid, update player position
                player_row = new_row;
                player_col = new_col;
            }
        }

        // upkeep functions
        energetic_check(map);
        flower_action(map, player_row, player_col);
        place_bush(map, player_row, player_col);
        upkeep(map);
        print_map(map, player_row, player_col);

        // win_loss_check returns 2 if game not over, so return if 1/0, ending game
        if (win_loss_check(map, player_row, player_col) != 2) {
            return 0;
        }
    }
}

// prompts user to input flower and foliage
void input_elements(struct tile map[MAX_ROW][MAX_COL],
                    int player_row,
                    int player_col) {

    int foliage_count = 0;
    int flower_count = 0;
    int start_row = 0;
    int start_col = 0;
    int i = 0;
    int length;
    char orientation, foliage_type;

    printf("How many tiles of foliage: ");
    scanf("%d", &foliage_count);

    while (i < foliage_count) {
        // scans first character of user input, if b or u, call add_foliage
        // else clear unscanned characters
        scanf(" %c", &foliage_type);    

        if (foliage_type == 'b') { 
            // handles b (branch) input
            scanf(" %d %d", &start_row, &start_col);
            add_foliage(map, foliage_type, start_row, start_col, '0', 0);

        } else if (foliage_type == 'u') { 
            // handles u (bushes) input
            scanf(" %d %d %c %d", &start_row, &start_col, &orientation, &length);
            add_foliage(map, foliage_type, start_row, start_col, orientation, length);

        } else {
            // clear any 'unscanned' characters
            while (getchar() != '\n'); 
            printf("Invalid foliage position!\n");
        }
        i++;
    }

    print_map(map, player_row, player_col);
    i = 0;

    // prompts users to input number and position of flower(s)
    printf("How many flowers: ");
    scanf("%d", &flower_count);

    while (i < flower_count) {
        // check if flower count above 25, handle based on flower count
        if (flower_count >= 25) {
            i = flower_count;
            add_flowers(map, 0, 0, flower_count);
        } else {
            scanf("%d %d", &start_row, &start_col);
            add_flowers(map, start_row, start_col, 1);
        }
        i++;
    }

    print_map(map, player_row, player_col);
}

// checks if win/loss condition met, if not, return 2 to continue game
int win_loss_check(struct tile map[MAX_ROW][MAX_COL],
                    int player_row,
                    int player_col
) {
    if (map[player_row][player_col].type == ROOT || 
        map[player_row][player_col].type == POLLEN) {
        printf("The flowers have beaten us, and UNSW is lost forever!\n");
        return 0;
    } else {
        // checks if there is flower on the map, if yes keep game running
        for (int row = 0; row < MAX_ROW; row++) {
            for (int col = 0; col < MAX_COL; col++) {
                if (map[row][col].type == FLOWER) {
                    return 2;
                }
            }
        }
    }
    // player position not ROOT or POLLEN and there are no flowers, 
    // so end game with win
    printf("All flowers are eradicated and UNSW has been saved!\n");
    return 1;
}

// handles flower attacks and conversion of DANGER to ROOT
void flower_action(struct tile map[MAX_ROW][MAX_COL], 
                int player_row,
                int player_col
) {
    // convert danger to ROOT
    for (int row = 0; row < MAX_ROW; row++) {
        for (int col = 0; col < MAX_COL; col++) {
            if (map[row][col].type == DANGER) {
                map[row][col].type = ROOT;
            }
        }
    }

    // goes through the 3x3 area around the player 
    for (int row = player_row - 1; row <= player_row + 1; row++) {
        for (int col = player_col - 1; col <= player_col + 1; col++) {
            // if the tile is not outside map area, keep going
            if (check_boundary('a', row, col)) {
                // check flower types, if DORMANT ANXIOUS ENERGETIC, ignore as 
                // they have other actions instead of ROOT attacks
                if (map[row][col].type == FLOWER && 
                    map[row][col].flower.state != DORMANT &&
                    map[row][col].flower.state != ANXIOUS && 
                    map[row][col].flower.state != ENERGETIC &&
                    map[row][col].flower.root_count != 0) {

                    // if player position is already ROOT, do not change to danger
                    if (map[player_row][player_col].type != ROOT) {
                        map[ player_row][player_col].type = DANGER;
                    }

                    map[row][col].flower.cooldown++;
                    map[row][col].flower.root_count--;

                    // end function after one tile is turned to danger
                    return;
                }
            }
        }
    }
}

void place_bush(struct tile map[MAX_ROW][MAX_COL],
                int player_row,
                int player_col
) {

    // go through each tile and look for flowers
    for (int row = 0; row < MAX_ROW; row++) {
        for (int col = 0; col < MAX_COL; col++) {
            // check cooldown and flower type
            if (map[row][col].type == FLOWER && 
                map[row][col].flower.cooldown == 0 &&
                map[row][col].flower.state != DORMANT && 
                map[row][col].flower.state != ANXIOUS &&
                map[row][col].flower.state != ENERGETIC) {
                    
                set_bush_tile(map, row, col, player_row, player_col);

            // reduce cooldown of flowers that have cooldown
            } else if (map[row][col].flower.cooldown > 0) {
                map[row][col].flower.cooldown--;
            }
        }

    }
}

void set_bush_tile(struct tile map[MAX_ROW][MAX_COL],
                   int row,
                   int col,
                   int player_row,
                   int player_col
) { 
    // go through down, right, up, left direction in order
    int direction[4][2] = {{1, 0}, {0, 1}, {-1, 0}, {0, -1}};
    int target_row, target_col;
    
    // go through loop until all 4 checked or bush placed on EMPTY tile
    for (int i = 0; i < 4 && map[row][col].flower.cooldown == 0; i++) {
        target_row = row + direction[i][0];
        target_col = col + direction[i][1];
        // ignore iteration if position is player position
        if (!(target_row == player_row && target_col == player_col) && 
            map[target_row][target_col].interacted == 0) {
            // place BUSH if tile is BRANCH, do not set cooldown
            if (map[target_row][target_col].type == BRANCH) {
                map[target_row][target_col].type = BUSH;
            // place BUSH and set cooldown once EMPTY tile is turned to BUSH
            } else if (map[target_row][target_col].type == EMPTY) {
                map[target_row][target_col].type = BUSH;
                map[row][col].flower.cooldown = 1;
            }
        }
    }
}

void alert(struct tile map[MAX_ROW][MAX_COL], 
           int target_row,
           int target_col
) {
    // check if target tile is FLOWER or BRANCH, if not, end function as 
    // there is no need to alert
    if (!(map[target_row][target_col].type == FLOWER || 
        map[target_row][target_col].type == BRANCH )) {
        return;
    }

    // go through 5x5 area around target tile
    for (int row = target_row - 2; row <= target_row + 2; row++) {
        for (int col = target_col - 2; col <= target_col + 2; col++) {
            // alert flower(s) and set flower state
            trigger_flower(map, row, col);
        }
    }    
}

void trigger_flower(struct tile map[MAX_ROW][MAX_COL],
                    int row,
                    int col
) {
    // check if target tile in the iteration is in the map boundary
    // and is a flower
    if (check_boundary('a', row, col) && map[row][col].type == FLOWER) {
        // set cooldown, state and root_count based on current flower state
        if (map[row][col].flower.state == DORMANT) {
            map[row][col].flower.state = DISTURBED;
            map[row][col].flower.cooldown = 1;
            map[row][col].flower.root_count++;
        } else if (map[row][col].flower.state == DISTURBED) { 
            map[row][col].flower.state = AWAKENED;
            map[row][col].interacted2 = 1;
            map[row][col].flower.root_count += 2;
        } else if (map[row][col].flower.state == AWAKENED) {
            map[row][col].flower.root_count += 2;
        } 
    }
}

// resets all tiles interacted value to 0 (called very turn)
// also calls seedlings_upkeep
void upkeep(struct tile map[MAX_ROW][MAX_COL]) {
    // goes through all tiles and looks for seedlings
    for (int row = 0; row < MAX_ROW; row++) {
        for (int col = 0; col < MAX_COL; col++) {
            
            // also sets all interacted values back to 0
            map[row][col].interacted = 0;
            map[row][col].interacted2 = 0;

            if (map[row][col].type == SEEDLING) {
                seedling_upkeep(map, row, col);
            }
        }
    }

    // goes through all tiles and looks for ANXIOUS FLOWERS
    for (int row = 0; row < MAX_ROW; row++) {
        for (int col = 0; col < MAX_COL; col++) {
            if (map[row][col].type == FLOWER &&
                map[row][col].flower.state == ANXIOUS) {
                // execute upkeep for anxious flower
                anxious_upkeep(map, row, col);
            }
        }
    }
}

void seedling_upkeep(struct tile map[MAX_ROW][MAX_COL],
                     int row,
                     int col
) {
    int seedling_alone = 1;

    // check if there is a flower nurturing the seedling in 5x5 area
    for (int i = row - 2;i <= row + 2; i++) {
        for (int j = col - 2; j <= col + 2; j++) {
            // check if tile is in map boundary and if it is ANXIOUS FLOWER
            if (check_boundary('a', i, j) && 
                map[i][j].type == FLOWER && 
                map[i][j].flower.state == ANXIOUS) {
                // if ANXIOUS FLOWER found, seedling is not alone
                seedling_alone = 0;
            }
        }
    }
    // if seedling not alone, will be nurtured by ANXIOUS FLOWER
    if (seedling_alone == 0) {
        map[row][col].seedling_growth++;
    }
    
    // once seedling has grown for 8 turns, turn into DORMANT flower
    // '9' because seedling_growth incremented on turn that 
    // seedling is spawned
    if (map[row][col].seedling_growth == 9) {
        map[row][col].type = FLOWER;
        map[row][col].flower.state = DORMANT;
        map[row][col].seedling_growth = 0;
        map[row][col].flower.root_count = 0;
    }
}

// goes through each flower on the map, and sets to anxious and 
// spawns seedlings if no adjacent flowers
void anxious_flower(struct tile map[MAX_ROW][MAX_COL],
                    int player_row, 
                    int player_col
) {
    // goes through each tile on map
    for (int row = 0; row < MAX_ROW; row++) {
        for (int col = 0; col < MAX_COL; col ++) {
            // check if tile is flower and awakened
            if (map[row][col].type == FLOWER &&
                map[row][col].flower.state == AWAKENED &&
                map[row][col].interacted2 == 0) {
                // sets flower to anxious if conditions are met
                set_anxious(map, row, col, player_row, player_col);
            }
        }
    }
}

// checks if flower meets conditions for it to become anxious and  
// sets to ANXIOUS if yes
void set_anxious(struct tile map[MAX_ROW][MAX_COL],
                 int row,
                 int col ,
                 int player_row,
                 int player_col
) {      
    int flower_alone = 1;

    // check if flower is alone (no flower on adjacent tiles)
    for (int i = row - 1;i < row + 1; i++) {
        for (int j = col - 1; j < col + 1; j++) {
            if (map[i][j].type == FLOWER && (i != row && j != col)) {
                flower_alone = 0;
            }
        }
    }

    // if flower is alone, set to anxious and spawn SEEDLING & POLLEN
    if (flower_alone == 1) {
        map[row][col].flower.state = ANXIOUS;
        map[row][col].flower.anxious_timer = 9;
        spawn_seedlings(map, row, col, player_row, player_col);
        spawn_pollen(map, row, col);
    }
}

// handles upkeep for anxious flowers 
void anxious_upkeep(struct tile map[MAX_ROW][MAX_COL],
                    int row,
                    int col
) {
    // call rotate pollen if it wasn't spawned on the same round
    if (map[row][col].flower.anxious_timer != 9) {
        rotate_pollen(map, row, col);
    }
    // decrement timer for anxious flowers 
    map[row][col].flower.anxious_timer--;
    // once timer is 0, turn back into AWAKENED and remove pollen from map
    if (map[row][col].flower.anxious_timer == 0) {
        remove_pollen(map, row, col);
        map[row][col].flower.state = AWAKENED;
        // map[row][col].flower.cooldown++;
        map[row][col].flower.root_count += 2;
    }
}

void rotate_pollen (struct tile map[MAX_ROW][MAX_COL],
                    int flower_row,
                    int flower_col
) {
    // array for the 8 tiles surrounding flower, starting from top left,
    // going in clockwise direction
    int positions[8][2] = {
        {flower_row - 1, flower_col - 1}, {flower_row - 1, flower_col},
        {flower_row - 1, flower_col + 1}, {flower_row, flower_col + 1},
        {flower_row + 1, flower_col + 1}, {flower_row + 1, flower_col},
        {flower_row + 1, flower_col - 1}, {flower_row, flower_col - 1}
    };

    int empty_tiles[2][2] = {0};
    int empty_found = 0;
    int empty1_index = 0;
    int empty2_index = 0;
    int new_empty_row = 0;
    int new_empty_col = 0;
    int new_pollen_row = 0;
    int new_pollen_col = 0;
    int to_continue = 0;

    // HOW FUNCTION WORKS:
    // after finding 2 'empty' tiles, set the 1st 'empty' tile to POLLEN and 
    // set tile after second 'empty' tile to EMPTY, which basically moves the 
    // two 'empty' tiles forward by 1 tile in a clockwise direction
    
    // look for tiles that do not have pollen (2 at any moment)
    for (int i = 0; i < 8; i++) {
        // if tile of iterration is outside of map, continue (continue not allowed)
        if (check_boundary('a', positions[i][0], positions[i][1])) {
            to_continue = 1;
        }
        // if tile of iteration is in map and not a pollen, remember position and index
        if (map[positions[i][0]][positions[i][1]].type != POLLEN &&
            to_continue == 1) {
            if (empty_found == 0) {
                empty_tiles[0][0] = positions[i][0];
                empty_tiles[0][1] = positions[i][1];
                empty1_index = i;
                empty_found = 1;
            } else {
                empty_tiles[1][0] = positions[i][0];
                empty_tiles[1][1] = positions[i][1];
                empty2_index = i;
            }
        }
        // resets to_continue after each tile
        to_continue = 0;
    }

    // because the search for empty tiles start from top left, there is case
    // where the second 'empty' tile in the clockwise sequence is found 
    // before the first, therefore, check if this happens then handle
    // accordingly, else, default logic

    if (empty1_index == 7) {
        // empty2_index is actually the first 'empty' tile in a clockwise 
        // direction, so set empty2 tile to POLLEN, set tile after empty1 to EMPTY
        new_pollen_row = positions[empty2_index][0];
        new_pollen_col = positions[empty2_index][1];
        new_empty_row = positions[(empty1_index + 1) % 8][0];
        new_empty_col = positions[(empty1_index + 1) % 8][1];
    } else {
        // set the 1st 'empty' tile to POLLEN, set tile after 2nd 'empty' tile
        // to EMPTY
        new_pollen_row = empty_tiles[0][0];
        new_pollen_col = empty_tiles[0][1];
        new_empty_row = positions[(empty2_index + 1) % 8][0];
        new_empty_col = positions[(empty2_index + 1) % 8][1];
    }
    // sets tiles to POLLEN and EMPTY based on logic above
    map[new_pollen_row][new_pollen_col].type = POLLEN;
    map[new_empty_row][new_empty_col].type = EMPTY;
}

void spawn_pollen(struct tile map [MAX_ROW][MAX_COL],
                  int row,
                  int col
) {
    // array for the 8 tiles surrounding a flower, starting from top right
    int positions[8][2] = {
        {row - 1, col + 1}, {row, col + 1},
        {row + 1, col + 1}, {row + 1, col}, 
        {row + 1, col - 1}, {row, col - 1},
        {row - 1, col - 1}, {row - 1, col}
    };

    // make all tiles EMPTY
    for (int i = 0; i < 8; i++) {
        map[positions[i][0]][positions[i][1]].type = EMPTY;
    }

    // then set 6 tiles, starting from top right, 
    // leaving two tiles EMPTY
    for (int i = 0; i < 6; i++) {
        map[positions[i][0]][positions[i][1]].type = POLLEN;
    }
}

// function that removes POLLEN surrounding a flower
void remove_pollen(struct tile map [MAX_ROW][MAX_COL],
                    int flower_row,
                    int flower_col
) {
    // array for the 8 tiles surrounding a flower
    int positions[8][2] = {
        {flower_row - 1, flower_col - 1}, {flower_row - 1, flower_col},
        {flower_row - 1, flower_col + 1}, {flower_row, flower_col + 1},
        {flower_row + 1, flower_col + 1}, {flower_row + 1, flower_col},
        {flower_row + 1, flower_col - 1}, {flower_row, flower_col - 1}
    };

    // make all tiles with POLLEN, EMPTY
    for (int i = 0; i < 8; i++) {
        if (map[positions[i][0]][positions[i][1]].type == POLLEN) {
            map[positions[i][0]][positions[i][1]].type = EMPTY;
        }
    }
}

void spawn_seedlings(struct tile map [MAX_ROW][MAX_COL],
                    int flower_row,
                    int flower_col,
                    int player_row,
                    int player_col) {

    // check all desired tiles in 5x5 range
    for (int row = flower_row - 2; row <= flower_row + 2; row += 2) {
        for (int col = flower_col - 2; col <= flower_col + 2; col += 2) {
            // check if tile is in map boundary 
            if (check_boundary('a', row, col)) {
                // if tile is not player tile and not a flower, change tile to SEEDLING
                if (!(row == player_row && col == player_col) &&
                    map[row][col].type != FLOWER &&
                    map[row][col].type != SEEDLING) {
                        
                    map[row][col].type = SEEDLING;
                    map[row][col].seedling_growth = 0;
                }
            }
        }
    }

}

// loops 
void energetic_check(struct tile map [MAX_ROW][MAX_COL]) {
    // up, right, down, left
    int direction[4][2] = {{-1, 0}, 
                           {0, 1},
                           {1, 0}, 
                           {0, -1}};
    int surrounded = 1;
    int to_continue = 0;
    // go through all tiles
    for (int row = 0; row < MAX_ROW; row++) {
        for (int col = 0; col < MAX_COL; col++) {
            // look for AWAKENED FLOWER
            if (map[row][col].type == FLOWER && map[row][col].flower.state == AWAKENED) {
                surrounded = 1;
                // loop through adjacent tiles, up down left right
                for (int i = 0; i < 4; i++) {
                    int target_row = row + direction[i][0];
                    int target_col = col + direction[i][1];
                    // check if target tile is in map boundary
                    if (check_boundary('a', target_row, target_col)) {
                        to_continue = 1;
                    }
                    // check if target tile is ROOT or BUSH, if not, then flower
                    // is not surrounded
                    if (map[target_row][target_col].type != ROOT &&
                        map[target_row][target_col].type != BUSH &&
                        to_continue == 1) {
                        surrounded = 0;
                    }
                    to_continue = 0;
                }
                // if surrounded, increment energy_count
                if (surrounded != 0) {
                    map[row][col].flower.energy_count++;
                }
                // once energy_count reaches 3, change flower state to ENERGETIC
                if (map[row][col].flower.energy_count == 3) {
                    map[row][col].flower.state = ENERGETIC;
                }
            }    
        }
    }
}

// find corner on map furthest away from player tile
int find_corner(struct tile underground[MAX_ROW][MAX_COL],
                int player_row,
                int player_col
) {
    // coordinates of each corner on the map
    int corners[4][2] = {{0, 0}, 
                         {0, MAX_COL - 1},
                         {MAX_ROW - 1, MAX_COL - 1}, 
                         {MAX_ROW - 1, 0}};
    double distance = 0;
    double max_distance = 0;
    int corner = 0;
    
    // loop through each corner and check distance between corner and player
    for (int i = 0; i < 4; i++) {
        // calculate distance between corner and player
        distance = sqrt(pow(corners[i][0] - player_row, 2) + 
                        pow(corners[i][1] - player_col, 2));

        if (distance > max_distance) {
            // keep track of corner with furthest distance
            max_distance = distance;
            corner = i;
        }
    }
    return corner;
}

// similar to player_command but tailored to undeground phase
int start_underground(struct tile underground[MAX_ROW][MAX_COL],
                      int flower_row,
                      int flower_col,
                      int player_row,
                      int player_col
) {
    int corners[4][2] = {{0, 0}, 
                         {0, MAX_COL - 1},
                         {MAX_ROW - 1, MAX_COL - 1}, 
                         {MAX_ROW - 1, 0}};
    char command, direction;
    int new_row, new_col;
    int i = 0;
    int root_count = 0;
    int energetic_row, energetic_col;
    
    // set i to be index of corner found in find_corner
    i = find_corner(underground, player_row, player_col);

    // sets corner to ENERGETIC FLOWER
    underground[corners[i][0]][corners[i][1]].type = FLOWER;
    underground[corners[i][0]][corners[i][1]].flower.state = ENERGETIC;

    print_map(underground, player_row, player_col);

    // player commands
    while (1) {
        printf("Enter command: ");

        // check for EOF 
        if (scanf(" %c", &command) != 1) {
            return 2;
        }
        
        if (command == 'c') {
            if (scanf(" %c", &direction) == 1) {
                // cut_command returns 1 if flower cut is ENERGETIC
                if (cut_command(underground, player_row, player_col, direction,
                                &energetic_row, &energetic_col)) {
                    // once ENERGETIC FLOWER cut, remove roots except for every
                    // 3rd root and exit underground phase
                    remove_roots(underground);
                    return 0;
                }
            }
        } else {
            direction = command;
            if (move_command(underground, player_row, player_col, command, 
                             &new_row, &new_col)) {
                // if move is valid, update player position
                player_row = new_row;
                player_col = new_col;
            }
        }

        // keep track of roots spawned 
        root_count += underground_roots(underground, player_row, player_col, 
                                        root_count, direction);
        // loop root_count every 6 roots
        if (root_count == 6) {
            root_count = 0;
        }

        print_map(underground, player_row, player_col);

        // checks loss condition returning 1 here also ends loop 
        // in main, ending the game
        if (underground[player_row][player_col].type == ROOT || 
            underground[player_row][player_col].type == POLLEN) {
            printf("The flowers have beaten us, and UNSW is lost forever!\n");
            return 1;
        } 
    }
    return 0;
}

// spawns roots in underground map
int underground_roots(struct tile underground[MAX_ROW][MAX_COL],
                       int player_row,
                       int player_col,
                       int root_count,
                       int direction
) {
    // loops through all tiles in map and looks for DANGER tile and 
    // turns it into ROOT
    for (int row = 0; row < MAX_ROW; row++) {
        for (int col = 0; col < MAX_COL; col++) {
            if (underground[row][col].type == DANGER) {
                underground[row][col].type = ROOT;
                // keep track of every 3rd root
                if (root_count % 3 == 0) {
                    underground[row][col].root_type = 1;
                }
            }
        }
    }
    // set player tile to DANGER if not already on ROOT tile
    if (underground[player_row][player_col].type != ROOT) {
        underground[player_row][player_col].type = DANGER;
    }
    // every 6th root, spawn root infront of player
    if (root_count == 5) {
        int target_row = player_row;
        int target_col = player_col;

        if (direction == 'w') {
            (target_row)--;
        } else if (direction == 'a') {
            (target_col)--;
        } else if (direction == 's') {
            (target_row)++;
        } else if (direction == 'd') {
            (target_col)++;
        } 
        if (underground[target_row][target_col].type != FLOWER) {
            underground[target_row][target_col].type = ROOT;
        }
        
    }
    return 1;
}

// remove roots except for every 3rd root (root_type == 1 roots)
void remove_roots(struct tile underground[MAX_ROW][MAX_COL]) {
    for (int row = 0; row < MAX_ROW; row++) {
        for (int col = 0; col < MAX_COL; col++) {
            if (underground[row][col].root_type == 0) {
                underground[row][col].type = EMPTY;
            }
        }
    }
}

// executes cut command, returns 1 if energetic flower is cut
int cut_command(struct tile map[MAX_ROW][MAX_COL], 
                int player_row, 
                int player_col, 
                char direction,
                int *energetic_row,
                int *energetic_col
) {
    int target_row = player_row;
    int target_col = player_col;

    if (direction == 'w') {
        (target_row)--;
    } else if (direction == 'a') {
        (target_col)--;
    } else if (direction == 's') {
        (target_row)++;
    } else if (direction == 'd') {
        (target_col)++;
    } 

    // check if target is in map
    if (check_boundary('a', target_row, target_col)) {
        // Only cut if target is BUSH or FLOWER or ROOT or SEEDLING
        if (map[target_row][target_col].type == FLOWER) {

            // alert flowers if flower cut is not energetic
            if (map[target_row][target_col].flower.state != ENERGETIC) {
                alert(map, target_row, target_col);
            }

            // removes pollen around anxious flower once its cut
            if (map[target_row][target_col].flower.state == ANXIOUS) {
                remove_pollen(map, target_row, target_col);
            }

            // if flower is cut, return 1 to initiate call of underground 
            // function in main
            if (map[target_row][target_col].flower.state == ENERGETIC) {
                *energetic_row = target_row;
                *energetic_col = target_col;
                map[target_row][target_col].type = EMPTY;
                map[target_row][target_col].flower.state = NONE;
                return 1;
            }
            // set tile to EMPTY once cut
            map[target_row][target_col].type = EMPTY;
            map[target_row][target_col].flower.state = NONE;
            
            // once flower is cut, set flowers to be set anxious if needed
            anxious_flower(map, player_row, player_col);
            return 0;  

        // handles cutting BUSH/ROOT/SEEDLING
        } else if (map[target_row][target_col].type == BUSH) {
            map[target_row][target_col].type = EMPTY;
            map[target_row][target_col].flower.state = NONE;
            // sets interacted so bush cant spawn on a tile right after it is cut
            map[target_row][target_col].interacted = 1;
        } else if (map[target_row][target_col].type == ROOT) {
            map[target_row][target_col].type = BRANCH;
        } else if (map[target_row][target_col].type == SEEDLING) {
            map[target_row][target_col].type = EMPTY;
            map[target_row][target_col].seedling_growth = 1;
        }
    }
    return 0;
}

// handles player movement, returns 1 if move is valid
int move_command(struct tile map[MAX_ROW][MAX_COL], 
                int row,
                int col, 
                char command,
                int *new_row,
                int *new_col
) {
    *new_row = row;
    *new_col = col;

    // check if desired movement is blocked or invalid
    if (command == 'i') {
        // idle has no effect for now
    } else if (command == 'w') {
        (*new_row)--;
    } else if (command == 'a') {
        (*new_col)--;
    } else if (command == 's') {
        (*new_row)++;
    } else if (command == 'd') {
        (*new_col)++;
    } 
    
    // if desired movement is outside of map do not move
    if (check_boundary('a', *new_row, *new_col)) {
        // checks if desired tile to move onto is valid
        if (map[*new_row][*new_col].type == FLOWER || 
            map[*new_row][*new_col].type == BUSH ||
            map[*new_row][*new_col].type == ROOT) {
            return 0;
        } else if (map[*new_row][*new_col].type == BRANCH) {
            // if moved onto branch, alert nearby flowers
            alert(map, *new_row, *new_col);
            map[*new_row][*new_col].type = EMPTY;
            return 1;
        } else if (map[*new_row][*new_col].type == EMPTY || 
                   map[*new_row][*new_col].type == POLLEN) {
    
            return 1;
        }
    }
    return 0;
}

//check if position is in or out of boundary, i for inner, o for outer
int check_boundary(char boundary_type, int row, int col) {
    if (boundary_type == 'i') {
        // checks if position is in inner boundary
        if (row >= 1 && row <= MAX_ROW - 2 && 
            col >= 1 && col <= MAX_COL - 2) {
            return 1; 
        }
    } else if (boundary_type == 'o') {
        // checks if position is in the outer boundary
        if (row == MAX_ROW - 1 || row == 0 || 
            col == 0 || col == MAX_COL - 1) {
            return 1; 
        }
    } else if (boundary_type == 'a') {
        // checks if position is inside of entire map
        if (row < MAX_ROW && row >= 0 && 
            col < MAX_COL && col >= 0) {
            return 1;
        } 
    }
    return 0;
}

// to add flowers onto map
void add_flowers(struct tile map[MAX_ROW][MAX_COL], 
                int start_row,
                int start_col, 
                int flower_count
) {
    int row, col;
    // handles condition where user wants 25 or more flowers
    if (flower_count >= 25) {
        for (row = 1; row < MAX_ROW; row += 2) {
            for (col = 1; col < MAX_COL; col += 2) {
                if (map[row][col].type == EMPTY) {
                    map[row][col].type = FLOWER;
                    map[row][col].flower.state = DORMANT;
                    map[row][col].flower.root_count = 0;
                }
            }
        }
    } else {
        // handles individual flower
        // check if user input for flower position is valid
        if (!check_boundary('i', start_row, start_col)) {
            printf("Invalid flower position!\n");
            return;
        }

        // check if position is odd and EMPTY
        if ((start_row % 2 == 1 && start_col % 2 == 1) && 
        map[start_row][start_col].type == EMPTY) {
            // if yes, change tile to flower
            map[start_row][start_col].type = FLOWER;
            map[start_row][start_col].flower.state = DORMANT;
            map[start_row][start_col].flower.root_count = 0;
        } else {
            printf("Invalid flower position!\n");
            return;
        }
    }
}

// to add foliage input by user on to map
int add_foliage(struct tile map[MAX_ROW][MAX_COL], 
                char foliage_type, 
                int start_row, 
                int start_col, 
                char orientation, 
                int length
) {

    // check if foliage placement is outside of inner boundary, 
    // if outside of range, return 0
    if (!check_boundary('i', start_row, start_col)) {
        printf("Invalid foliage position!\n");
        return 0;
    }
    
    if (foliage_type == 'b') { 
        // handles branches
        map[start_row][start_col].type = BRANCH;
        printf("Branch added!\n");   
        return 1;    
    
    } else if (foliage_type == 'u') { 
        // handles bushes
        // check if bushes will extend out of map
        if (orientation == 'h' && (start_col + length > MAX_COL || 
                                   length < 1)) {
            return 0; 
        } else if (orientation == 'v' && (start_row + length > MAX_ROW ||
                                          length < 1)) {
            return 0; 
        }
        
        // adds bushes to map based on orientation
        for (int i = 0; i < length; i++) {
            if (orientation == 'h') {
                map[start_row][start_col + i].type = BUSH;
            } else if (orientation == 'v') {
                map[start_row + i][start_col].type = BUSH;
            } 
        }

        printf("Bush added!\n");
        return 1;       
    } 
    printf("Invalid foliage position!\n");
    return 0;
}

//------------------------------------------------------------------------------
// Provided Functions
//------------------------------------------------------------------------------

/**
 * Initialises the game map with empty tiles, setting the type of each tile to 
 * `EMPTY` and initialising flower-related properties.
 *
 * @param map: The 2D array of `struct tile` representing the game map 
 *
 * @returns None
 *
**/
void initialise_map(struct tile map[MAX_ROW][MAX_COL]) {
    for (int row = 0; row < MAX_ROW; row++) {
        for (int col = 0; col < MAX_COL; col++) {
            map[row][col].type = EMPTY;
            map[row][col].flower.state = NONE;
            map[row][col].flower.cooldown = 0;
            map[row][col].flower.energy_count = 0;
            map[row][col].seedling_growth = 0;
            map[row][col].root_type = 0;
        }
    }
}

/**
 * Prints the game map 
 * 
 * @param map: The 2D array of `struct tile` representing the game map.
 * @param player_row: The row coordinate of the player.
 * @param player_col: The column coordinate of the player.
 *
 * @returns None
**/
void print_map(
    struct tile map[MAX_ROW][MAX_COL], 
    int player_row, 
    int player_col
) {
    for (int i = 0; i < MAX_ROW; i++) {
        for (int j = 0; j < MAX_COL; j++) {
            printf("+---");
        }
        printf("+\n");

        for (int j = 0; j < MAX_COL;j++) {
            printf("|");
            if (player_row != i || player_col != j) {
                print_tile(map[i][j]);
            } else if (map[i][j].type == EMPTY) {
                printf(" P ");
            } else if (map[i][j].type == DANGER) {
                printf("<P>");
            } else if (map[i][j].type == ROOT) {
                printf("<^>");
            }
        }

        printf("|\n");
    }

    for (int j = 0; j < MAX_COL; j++) {
        printf("+---");
    }
    printf("+\n");
}

/**
 * Helper function which prints the representation of a single tile 
 * based on its type. 
 *
 * @params tile: The `struct tile` to be printed.
 *
 * @returns None
**/
void print_tile(struct tile tile) {
    if (tile.type == EMPTY) {
        printf("   ");
    } else if (tile.type == BUSH) {
        printf("###");
    } else if (tile.type == BRANCH) {
        printf("_/-");
    } else if (tile.type == DANGER) {
        printf("< >");
    } else if (tile.type == ROOT) {
        printf("<^>");
    } else if (tile.type == POLLEN) {
        printf(":::");
    } else if (tile.type == FLOWER) {
        print_flower(tile.flower);
    } else if (tile.type == SEEDLING) {
        printf(".w.");
    }
}

/**
 * Helper function which prints a visual representation of the 
 * flower face based on its state.
 *
 * @params flower: The `struct flower` containing the state of the flower.
 *
 * @returns None
 *
**/
void print_flower(struct flower flower) {
    if (flower.state == DORMANT) {
        printf("uwu");
    } else if (flower.state == DISTURBED) {
        printf("uwo");
    } else if (flower.state == AWAKENED) {
        printf("owo");
    } else if (flower.state == ANXIOUS) {
        printf("o~o");
    } else if (flower.state == ENERGETIC) {
        printf("*w*");
    } 
}

