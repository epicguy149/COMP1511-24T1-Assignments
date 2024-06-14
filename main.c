//
// DO NOT CHANGE THIS FILE
//
// You do not submit this file. This file is not marked.
// If you think you need to change this file you have
// misunderstood the assignment - ask in the course forum.
//
// Assignment 2 24T1 COMP1511: Crepe Stand
// main.c
//
// Version 1.0.0: Release
//
// This file allows you to interactively test the functions you
// implement in crepe_stand.c

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#include "crepe_stand.h"

#define MAX_LINE 2048

// Complete
#define HELP '?'
#define QUIT 'q'

// Stage 1
#define PRINT_CREPES 'p'
#define APPEND_CREPE 'a'
#define COUNT_CREPE 'c'

// Stage 2
#define INSERT_CREPE 'i'
#define PRINT_STATS 's'
#define CALCULATE_PRICE 'C'
#define TOTAL_INCOME 't'
#define CUSTOMER_BILL 'b'

// Stage 3
#define NEW_DAY 'n'
#define DISPLAY_DAYS 'd'
#define NEXT_DAY '>'
#define PREV_DAY '<'
#define REMOVE_CREPE 'r'
#define REMOVE_DAY 'R'

// Stage 4
#define COMPARE_WEEKDAYEND 'w'
#define MAX_PROFIT_PERIOD 'm'

// Command Returns
#define EXIT 0
#define READ_COMMAND 2


struct state {
    struct day *day_one;
    struct day *current_day;
};


// Helper Functions
struct date welcome_message(void);
struct date check_valid_date(char *date);
struct state *setup_crepe_stand(struct date date);
struct state *command_loop(struct state *state);
static void *not_null(void *ptr);
static int get_command(char *command, int max_command_length);
static int run_command(struct state *state, char *line);

// Do: Completed
static void do_print_help(void);

// Do: Stage 1
static void do_print_crepes(struct state *state, char *line);
static void do_append_crepe(struct state *state, char *line);
static void do_count_crepes(struct state *state, char *line);

// Do: Stage 2
static void do_insert_crepe(struct state *state, char *line);
static void do_get_stats(struct state *state, char *line);
static void do_calculate_price(struct state *state, char *line);
static void do_total_income(struct state *state, char *line);
static void do_customer_bill(struct state *state, char *line);

// Do: Stage 3
static void do_new_day(struct state *state, char *line);
static void do_display_days(struct state *state, char *line);
static void do_cycle_days(struct state *state, char *line);
static void do_remove_crepe(struct state *state, char *line);
static void do_remove_day(struct state *state, char *line);

// Do: Stage 4
static void do_compare_weekday_end(struct state *state, char *line);
static void do_max_profit_period(struct state *state, char *line);


int main(void) {
    struct date date = welcome_message();

    struct state *state = setup_crepe_stand(date);

    state = command_loop(state);

    printf("Thanks for visiting the 1511 Crepe Stand!\n");

    free_crepe_stand(state->day_one);
    free(state);

    return 0;
}

// Prints welcome message and scans the current date
//
// Parameters:
//      date        - where to store the scanned current date
//
// Returns:
//      nothing
//
struct date welcome_message(void) {
    char date[MAX_LINE];
    printf("Welcome to the 1511 Crepe Stand!\n");
    printf("Please enter today's date (YYYY-MM-DD): ");
    fgets(date, MAX_LINE, stdin);

    return check_valid_date(date);
}

// Ensures the provided date is in the correct format YYYY-MM-DD
//
// Parameters:
//      date        - scanned current date
//
// Returns:
//      nothing
//
struct date check_valid_date(char *date) {
    char yearStr[5] = { 0 };
    char monthStr[3] = { 0 };
    char dayStr[3] = { 0 };
    if (sscanf(date, "%4[0-9]-%2[0-9]-%2[0-9]",
               yearStr, monthStr, dayStr) != 3) {
        fprintf(stderr, "Date must be in the format YYYY-MM-DD.\n");
        exit(1);
    }

    // check if the date is in the correct format
    int year = atoi(yearStr);
    int month = atoi(monthStr);
    int day = atoi(dayStr);
    if (month < 1 || month > 12) {
        fprintf(stderr, "Month must be between 01 and 12 "
                "(got %s instead).\n", monthStr);
        exit(1);
    }

    int is_leap_year = year % 4 != 0 ? 0 : year % 100 != 0 ? 1 :
                       year % 400 == 0;
    int monthMaxDays[] = { 31, 28 + is_leap_year, 31, 30, 31,
                           30, 31, 31, 30, 31, 30, 31 };
    int maxDays = monthMaxDays[month - 1];

    char *monthNames[] = {
        "January",
        "February",
        "March",
        "April",
        "May",
        "June",
        "July",
        "August",
        "September",
        "October",
        "November",
        "December",
    };

    if (day < 1 || day > maxDays) {
        fprintf(stderr, "In %s, day must be between 01 and %d "
                "(got %s instead).\n", monthNames[month - 1], maxDays, dayStr);
        exit(1);
    }

    // remove '\n' if present
    date[strcspn(date, "\n")] = '\0';

    // check the numbers for the date are the correct length
    if (strlen(date) != 10) {
        fprintf(stderr, "Date must be in the format YYYY-MM-DD.\n");
        exit(1);
    }

    struct date date_info;
    date_info.year = year;
    date_info.month = month;
    date_info.day = day;
    
    return date_info;
}

// Calls create_day to create day_one of the list of days
// Creates the struct state which will store day_one and current_day
//
// Parameters:
//      date        - scanned current date
//
// Returns:
//      a pointer to the created struct state
//
struct state *setup_crepe_stand(struct date date) {
    struct day *day_one = create_day(date);
    struct state *state = malloc(sizeof(struct state));
    state->day_one = day_one;
    state->current_day = day_one;
    return state;
}

// Loops until CTRL + D, scanning in commands from the user
//
// Parameters:
//      state        - pointer to the struct state containing day_one and
//                     current_day
//
// Returns:
//      a pointer to the struct state
//
struct state *command_loop(struct state *state) {
    int read_command = READ_COMMAND;
    char line[MAX_LINE];
    while (read_command) {
        printf("Enter Command: ");
        read_command = get_command(line, MAX_LINE);
        if (read_command) {
            read_command = run_command(state, line);
        }
    }
    return state;
}

// Scans in a command line from stdin, removes the newline and any whitespace
//
// Parameters:
//      command             - where to store the scanned line
//      max_command_length  - maximum length of command line
//
// Returns:
//      int indicating if fgets was successful or not
//
static int get_command(char *command, int max_command_length) {
    if (fgets(command, max_command_length, stdin) == NULL) {
        return 0;
    }

    // remove '\n' if present
    command[strcspn(command, "\n")] = '\0';

    int leading_whitespace = 0;
    while (isspace(command[leading_whitespace])) {
        leading_whitespace++;
    }
    memmove(command,
            command + leading_whitespace,
            MAX_LINE - leading_whitespace);

    return 1;
}

// Ensures a given pointer is not null.
//
// Parameters:
//      ptr         - pointer to check
//
// Returns:
//     the pointer if not null
//
static void *not_null(void *ptr) {
    if (ptr != NULL) {
        return ptr;
    }
    fprintf(stderr, "This solution was going to pass a NULL pointer.\n");
    fprintf(stderr, "You have probably not implemented a required "
            "function.\n");
    fprintf(stderr, "If you believe this message is in error, post on the "
            "course forum.\n");
    exit(1);
}

// Checks what the entered command was, and calls the corresponding do function
//
// Parameters:
//      state       - pointer to the struct state containing day_one and
//                    current_day
//      line        - the scanned in line from stdin containing the commands
//
// Returns:
//      an int indicating whether or not to keep reading commands
//
static int run_command(struct state *state, char *line) {
    char command = line[0];
    if (command == HELP) {
        do_print_help();
    } else if (command == QUIT) {
        return EXIT;
    } else if (command == PRINT_CREPES) {
        do_print_crepes(state, line);
    } else if (command == APPEND_CREPE) {
        do_append_crepe(state, line);
    } else if (command == COUNT_CREPE) {
        do_count_crepes(state, line);
    } else if (command == INSERT_CREPE) {
        do_insert_crepe(state, line);
    } else if (command == PRINT_STATS) {
        do_get_stats(state, line);
    } else if (command == CALCULATE_PRICE) {
        do_calculate_price(state, line);
    } else if (command == TOTAL_INCOME) {
        do_total_income(state, line);
    } else if (command == CUSTOMER_BILL) {
        do_customer_bill(state, line);
    } else if (command == NEW_DAY) {
        do_new_day(state, line);
    } else if (command == DISPLAY_DAYS) {
        do_display_days(state, line);
    } else if (command == NEXT_DAY) {
        do_cycle_days(state, line);
    } else if (command == PREV_DAY) {
        do_cycle_days(state, line);
    } else if (command == REMOVE_CREPE) {
        do_remove_crepe(state, line);
    } else if (command == REMOVE_DAY) {
        do_remove_day(state, line);
    } else if (command == COMPARE_WEEKDAYEND) {
        do_compare_weekday_end(state, line);
    } else if (command == MAX_PROFIT_PERIOD) {
        do_max_profit_period(state, line);
    } else {
        printf("Invalid command %c, use '?' to get help!\n", line[0]);
    }
    return READ_COMMAND;
}

// Prints the help table, explaining the different commands available
//
// Parameters:
//      nothing
//
// Returns:
//      nothing
//
static void do_print_help(void) {
    printf(
        "=====================[  1511 Crepe Stand  ]=====================\n"
        "      ===============[     Usage Info     ]===============      \n"
        "  ?                                                             \n"
        "    Show help                                                   \n"
        "  a [crepe type] [customer name] {OPTIONAL: [batter] [topping]  \n"
        "    [gluten free] [size]}                                       \n"
        "    Append a crepe to the end of the orders list                \n"
        "  p                                                             \n"
        "    Print out all the crepes in the order list                  \n"
        "  c                                                             \n"
        "    Print out the amount crepes in the order list; total,       \n"
        "    chocolate and matcha counts.                                \n"
        "  i [position] [crepe type] [customer name] {OPTIONAL: [batter] \n"
        "    [topping] [gluten free] [size]}                             \n"
        "    Insert a crepe to the [position] position in the list       \n"
        "  s                                                             \n"
        "    Prints out the statistics of the day's orders               \n"
        "  C [position]                                                  \n"
        "    Calculates the cost of the crepe in the given position      \n"
        "  t                                                             \n"
        "    Prints out the current total income for the day             \n"
        "  b [customer name]                                             \n"
        "    Prints out the given customer's bill for the day            \n"
        "  n [date in YYYY-MM-DD format]                                 \n"
        "    Inserts a new day to the list of days                       \n"
        "  d                                                             \n"
        "    Prints out all days and associated information              \n"
        "  >                                                             \n"
        "    Cycles to the next day in the list of days                  \n"
        "  <                                                             \n"
        "    Cycles to the previous day in the list of days              \n"
        "  r [position]                                                  \n"
        "    Removes a crepe from the current day in the specified       \n"
        "    position                                                    \n"
        "  R [date in YYYY-MM-DD format]                                 \n"
        "    Removes specified day in the list of days and all its       \n"
        "    ordered crepes.                                             \n"
        "  w                                                             \n"
        "    Prints the total profit over all weekdays and all weekends  \n"
        "  m [year to check]                                             \n"
        "    Prints the maximum profit period in the given year          \n"
        "================================================================\n"
    );
}

////////////////////////////////////////////////////////////////////////////////
//                             Stage 1 Functions                              //
////////////////////////////////////////////////////////////////////////////////

// Handles calling and printing for print_crepes in crepe_stand.c
//
// Parameters:
//      state       - pointer to the struct state containing day_one and
//                    current_day
//      line        - the scanned in line from stdin containing the commands
//
// Returns:
//      nothing
//
static void do_print_crepes(struct state *state, char *line) {
    // this line is passed for consistency, but is not needed
    (void) line;

    print_crepes(not_null(state->current_day));
}

// Handles calling and printing for append_crepe_custom and
// append_crepe_signature
// in crepe_stand.c
//
// Parameters:
//      state       - pointer to the struct state containing day_one and
//                    current_day
//      line        - the scanned in line from stdin containing the commands
//
// Returns:
//      nothing
//
static void do_append_crepe(struct state *state, char *line) {
    char command;
    char crepe_type[MAX_STR_LEN];
    char customer_name[MAX_STR_LEN];
    int batter;
    int topping;
    int gluten_free;
    int size;
    int scanf_return = sscanf(line, "%c %31s %31s %d %d %d %d",
                              &command,
                              crepe_type,
                              customer_name,
                              &batter,
                              &topping,
                              &gluten_free,
                              &size);

    if (scanf_return != 7 && scanf_return != 3) {
        printf("ERROR: Invalid command. The correct format is %c [crepe type] "
               "[customer name] {OPTIONAL: [batter] [toppping] [gluten-free] "
               "[size]\n", command);
        return;
    }

    int append_crepe_return;
    if (scanf_return == 7) {
        append_crepe_return =
        append_crepe_custom(not_null(state->current_day),
                                                  customer_name,
                                                  batter,
                                                  topping,
                                                  gluten_free,
                                                  size);
    } else {
        append_crepe_return =
        append_crepe_signature(not_null(state->current_day),
                               customer_name,
                               crepe_type);
    }

    if (append_crepe_return == INVALID_BATTER) {
        printf("ERROR: Invalid batter. Batter should be custom, original, "
               "matcha or chocolate.\n");
    } else if (append_crepe_return == INVALID_TOPPING) {
        printf("ERROR: Invalid topping. Topping should be none, banana, "
               "strawberry or nutella.\n");
    } else if (append_crepe_return == INVALID_GFO) {
        printf("ERROR: Invalid GFO input. Should be 1 for gluten-free "
               "or 0 for not.\n");
    } else if (append_crepe_return == INVALID_SIZE) {
        printf("ERROR: Invalid size. Diameter must be between 10cm and "
               "40cm.\n");
    } else if (append_crepe_return == INVALID_DAY) {
        printf("ERROR: No days entered. Please enter a day first.\n");
    } else if (append_crepe_return == VALID_CREPE) {
        printf("%s's crepe added to orders!\n", customer_name);
    } else {
        printf("ERROR: Unknown return value.\n");
    }
}

// Handles calling and printing for count_crepes in crepe_stand.c
//
// Parameters:
//      state       - pointer to the struct state containing day_one and
//                    current_day
//      line        - the scanned in line from stdin containing the commands
//
// Returns:
//      nothing
//
static void do_count_crepes(struct state *state, char *line) {
    // this line is passed for consistency, but is not needed
    (void) line;

    int count_crepes_return = count_crepes(not_null(state->current_day));

    if (count_crepes_return == 1) {
        printf("There has been 1 crepe ordered so far today!\n");
    } else if (count_crepes_return > 1) {
        printf("There have been %d crepes ordered so far today!\n",
                count_crepes_return);
    } else if (count_crepes_return == 0) {
        printf("No crepes have been ordered yet!\n");
    } else {
        printf("ERROR: Invalid amount of crepes (%d) ordered.\n",
                count_crepes_return);
    }
}

////////////////////////////////////////////////////////////////////////////////
//                             Stage 2 Functions                              //
////////////////////////////////////////////////////////////////////////////////

// Handles calling and printing for insert_crepe_custom and
// insert_crepe_signature
// in crepe_stand.c
//
// Parameters:
//      state       - pointer to the struct state containing day_one and
//                    current_day
//      line        - the scanned in line from stdin containing the commands
//
// Returns:
//      nothing
//
static void do_insert_crepe(struct state *state, char *line) {
    char command;
    int position;
    char crepe_type[MAX_STR_LEN];
    char customer_name[MAX_STR_LEN];
    int batter;
    int topping;
    int gluten_free;
    int size;
    int scanf_return = sscanf(line, "%c %d %31s %31s %d %d %d %d",
                              &command,
                              &position,
                              crepe_type,
                              customer_name,
                              &batter,
                              &topping,
                              &gluten_free,
                              &size);

    if (scanf_return != 8 && scanf_return != 4) {
        printf("ERROR: Invalid command. The correct format is %c [position] "
               "[crepe type] [customer name] {OPTIONAL: [batter] [toppping] "
               "[gluten-free] [size]\n", command);
        return;
    }

    int insert_crepe_return;
    if (scanf_return == 8) {
        insert_crepe_return =
        insert_crepe_custom(not_null(state->current_day),
                            position,
                            customer_name,
                            batter,
                            topping,
                            gluten_free,
                            size);
    } else {
        insert_crepe_return =
        insert_crepe_signature(not_null(state->current_day),
                               position,
                               customer_name,
                               crepe_type);
    }

    if (insert_crepe_return == INVALID_BATTER) {
        printf("ERROR: Invalid batter. Batter should be custom, original, "
               "matcha or chocolate.\n");
    } else if (insert_crepe_return == INVALID_TOPPING) {
        printf("ERROR: Invalid topping. Topping should be none, banana, "
               "strawberry or nutella.\n");
    } else if (insert_crepe_return == INVALID_GFO) {
        printf("ERROR: Invalid GFO input. Should be 1 for gluten-free or 0 "
               "for not.\n");
    } else if (insert_crepe_return == INVALID_SIZE) {
        printf("ERROR: Invalid size. Diameter must be "
               "between 10cm and 40cm.\n");
    }  else if (insert_crepe_return == INVALID_DAY) {
        printf("ERROR: No days entered. Please enter a day first.\n");
    } else if (insert_crepe_return == INVALID_POSITION) {
        printf("ERROR: Invalid position. Position must be 1 or greater.\n");
    } else if (insert_crepe_return == VALID_CREPE) {
        printf("%s's crepe inserted to orders!\n", customer_name);
    } else {
        printf("ERROR: Unknown return value.\n");
    }
}

// Handles calling and printing for get_stats in crepe_stand.c
//
// Parameters:
//      state       - pointer to the struct state containing day_one and
//                    current_day
//      line        - the scanned in line from stdin containing the commands
//
// Returns:
//      nothing
//
static void do_get_stats(struct state *state, char *line) {
    // this line is passed for consistency, but is not needed
    (void) line;

    struct stats stats = get_stats(state->current_day);

    if (stats.total_crepes == 0) {
        printf("No orders have been placed yet!\n");
        return;
    } else if (stats.total_crepes < 0) {
        printf("ERROR: Total crepes should not be less than 0.\n");
        return;
    }

    printf("==================[   Crepe Stand Stats   ]==================\n");
    printf("Of the %d crepes ordered today, there was %d original, "
           "%d chocolate and %d matcha.\n",
           stats.total_crepes,
           stats.original,
           stats.chocolate,
           stats.matcha);

    printf("There were a total of %d gluten-free crepes.\n", stats.gluten_free);

    if (stats.popular_topping == BANANA) {
        printf("The most popular topping was banana.\n");
    } else if (stats.popular_topping == STRAWBERRY) {
        printf("The most popular topping was strawberry.\n");
    } else if (stats.popular_topping == NUTELLA) {
        printf("The most popular topping was nutella.\n");
    } else if (stats.popular_topping == NO_TOPPING) {
        printf("No topping was more popular than the others.\n");
    }

    printf("Of the crepes ordered today, there was %d small, %d medium and "
           "%d large.\n", stats.small, stats.medium, stats.large);
    printf("=============================================================\n");
}

// Handles calling and printing for calculate_price in crepe_stand.c
//
// Parameters:
//      state       - pointer to the struct state containing day_one and
//                    current_day
//      line        - the scanned in line from stdin containing the commands
//
// Returns:
//      nothing
//
static void do_calculate_price(struct state *state, char *line) {
    char command;
    int position;
    int scanf_return = sscanf(line, "%c %d", &command, &position);

    if (scanf_return != 2) {
        printf("ERROR: Invalid command. The correct format is %c [position]\n",
                command);
        return;
    }

    double calculate_price_return =
    calculate_price(state->current_day, position);

    if (calculate_price_return == INVALID_POSITION) {
        printf("ERROR: Invalid position. Position must correspond to a crepe in"
               " the list.\n");
    } else {
        printf("This crepe costs %.2lf!\n", calculate_price_return);
    }
}

// Handles calling and printing for total_income in crepe_stand.c
//
// Parameters:
//      state       - pointer to the struct state containing day_one and
//                    current_day
//      line        - the scanned in line from stdin containing the commands
//
// Returns:
//      nothing
//
static void do_total_income(struct state *state, char *line) {
    // this line is passed for consistency, but is not needed
    (void) line;

    double total_income_return = total_income(state->current_day);

    if (total_income_return == 0) {
        printf("No orders have been placed yet!\n");
    } else {
        printf("Today's current income is %.2lf!\n", total_income_return);
    }
}

// Handles calling and printing for customer_bill in crepe_stand.c
//
// Parameters:
//      state       - pointer to the struct state containing day_one and
//                    current_day
//      line        - the scanned in line from stdin containing the commands
//
// Returns:
//      nothing
//
static void do_customer_bill(struct state *state, char *line) {
    char command;
    char customer_name[MAX_STR_LEN];
    int scanf_return = sscanf(line, "%c %31s", &command, customer_name);

    if (scanf_return != 2) {
        printf("ERROR: Invalid command. The correct format is %c "
               "[customer name]\n", command);
        return;
    }

    struct bill customer_bill_return = customer_bill(state->current_day,
                                                     customer_name);

    if (customer_bill_return.crepe_count == 0) {
        printf("No orders have been placed by %s yet today.\n", customer_name);
    } else if (customer_bill_return.crepe_count == 1) {
    printf("%s has ordered 1 crepe, and their bill for today is %.2lf.\n",
               customer_name,
               customer_bill_return.total_price);
    } else {
        printf("%s has ordered %d crepes, and their bill for today is %.2lf.\n",
               customer_name,
               customer_bill_return.crepe_count,
               customer_bill_return.total_price);
    }
}

////////////////////////////////////////////////////////////////////////////////
//                             Stage 3 Functions                              //
////////////////////////////////////////////////////////////////////////////////

// Handles calling and printing for new_day in crepe_stand.c
//
// Parameters:
//      state       - pointer to the struct state containing day_one and
//                    current_day
//      line        - the scanned in line from stdin containing the commands
//
// Returns:
//      nothing
//
static void do_new_day(struct state *state, char *line) {
    char command;
    char date_str[MAX_STR_LEN];
    int scanf_return = sscanf(line, "%c %31s", &command, date_str);

    if (scanf_return != 2) {
        printf("ERROR: Invalid command. The correct format is %c "
               "[YYYY-MM-DD]\n", command);
        return;
    }

    struct date date = check_valid_date(date_str);

    struct day *return_day = new_day(state->day_one, date);

    if (return_day == NULL) {
        printf("This day has already been inserted!\n");
    } else {
        state->day_one = return_day;
        printf("New day (%s) inserted.\n", date_str);
    }
}

// Handles calling and printing for display_days in crepe_stand.c
//
// Parameters:
//      state       - pointer to the struct state containing day_one and
//                    current_day
//      line        - the scanned in line from stdin containing the commands
//
// Returns:
//      nothing
//
static void do_display_days(struct state *state, char *line) {
    // this line is passed for consistency, but is not needed
    (void) line;

    display_days(state->day_one, state->current_day);
}

// Handles calling and printing for cycle_days in crepe_stand.c
//
// Parameters:
//      state       - pointer to the struct state containing day_one and
//                    current_day
//      line        - the scanned in line from stdin containing the commands
//
// Returns:
//      nothing
//
static void do_cycle_days(struct state *state, char *line) {
    char command;
    sscanf(line, "%c", &command);

    state->current_day =
    cycle_days(command, state->day_one, state->current_day);

    if (command == NEXT_DAY) {
        printf("Cycled forward.\n");
    } else {
        printf("Cycled backward.\n");
    }
}

// Handles calling and printing for remove_crepe in crepe_stand.c
//
// Parameters:
//      state       - pointer to the struct state containing day_one and
//                    current_day
//      line        - the scanned in line from stdin containing the commands
//
// Returns:
//      nothing
//
static void do_remove_crepe(struct state *state, char *line) {
    char command;
    int position;
    int scanf_return = sscanf(line, "%c %d", &command, &position);

    if (scanf_return != 2) {
        printf("ERROR: Invalid command. The correct format is %c "
               "[position]\n", command);
        return;
    }

    int remove_return = remove_crepe(state->current_day, position);

    if (remove_return == INVALID_POSITION) {
        printf("ERROR: Invalid position. Position must refer to a crepe in the "
               "current day.\n");
    } else if (remove_return == VALID_CREPE) {
        printf("Removing crepe no. %d\n", position);
    } else {
        printf("ERROR: Unknown return value.\n");
    }
}

// Handles calling and printing for remove_day in crepe_stand.c
//
// Parameters:
//      state       - pointer to the struct state containing day_one and
//                    current_day
//      line        - the scanned in line from stdin containing the commands
//
// Returns:
//      nothing
//
static void do_remove_day(struct state *state, char *line) {
    char command;
    char date_str[MAX_STR_LEN];
    int scanf_return = sscanf(line, "%c %31s", &command, date_str);

    if (scanf_return != 2) {
        printf("ERROR: Invalid command. The correct format is %c "
               "[YYYY-MM-DD]\n", command);
        return;
    }

    struct date date = check_valid_date(date_str);

    struct day *remove_day_return = remove_day(state->day_one,
                                               &state->current_day,
                                               date);
    if (remove_day_return == NULL) {
        printf("ERROR: Invalid date.\n");
    } else {
        printf("Successfully removed %s.\n", date_str);
        state->day_one = remove_day_return;
    }
}

////////////////////////////////////////////////////////////////////////////////
//                             Stage 4 Functions                              //
////////////////////////////////////////////////////////////////////////////////

// Handles calling and printing for compare_weekday_end in crepe_stand.c
//
// Parameters:
//      state       - pointer to the struct state containing day_one and
//                    current_day
//      line        - the scanned in line from stdin containing the commands
//
// Returns:
//      nothing
//
static void do_compare_weekday_end(struct state *state, char *line) {
    // this line is passed for consistency, but is not needed
    (void) line;

    struct profits profit = compare_weekday_end(state->day_one);

    if (profit.weekday < 0) {
        printf("The weekdays made -$%.2lf\n", profit.weekday * -1);
    } else {
        printf("The weekdays made $%.2lf\n", profit.weekday);
    }
    if (profit.weekend < 0) {
        printf("The weekends made -$%.2lf\n", profit.weekend * -1);
    } else {
        printf("The weekends made $%.2lf\n", profit.weekend);
    }

}

// Handles calling and printing for max_profit_period in crepe_stand.c
//
// Parameters:
//      state       - pointer to the struct state containing day_one and
//                    current_day
//      line        - the scanned in line from stdin containing the commands
//
// Returns:
//      nothing
//
static void do_max_profit_period(struct state *state, char *line) {
    char command;
    int year;
    int scanf_return = sscanf(line, "%c %d", &command, &year);

    if (scanf_return != 2) {
        printf("ERROR: Invalid command. The correct format is %c [YYYY]\n",
               command);
        return;
    }

    struct max_profit max = max_profit_period(state->day_one, year);

    if (max.start_date.year == 0 && max.end_date.year == 0) {
        printf("ERROR: The year %d has not been entered. Try again with a "
               "valid year.\n", year);
    } else if (max.end_date.year == 0) {
        if (max.profit < 0) {
            printf("No profit was made during %d. ", year);
            printf("The least loss was on %04d-%02d-%02d.\n",
                   max.start_date.year,
                   max.start_date.month,
                   max.start_date.day);
            printf("This day made -$%.2lf.\n", max.profit * -1);
        } else {
            printf("The max profit period for %d ", year);
            printf("was on %04d-%02d-%02d.\n", 
                   max.start_date.year,
                   max.start_date.month,
                   max.start_date.day);
            printf("The profit during this period was $%.2lf.\n", max.profit);
        }
    } else {
        if (max.profit < 0) {
            printf("No profit was made during %d. ", year);
            printf("The least loss was on %04d-%02d-%02d.\n",
                   max.start_date.year,
                   max.start_date.month,
                   max.start_date.day);
            printf("This day made -$%.2lf.\n", max.profit * -1);
        } else {
            printf("The max profit period for %d was was between ", year);
            printf("%04d-%02d-%02d and %04d-%02d-%02d.\n",
                   max.start_date.year,
                   max.start_date.month,
                   max.start_date.day,
                   max.end_date.year,
                   max.end_date.month,
                   max.end_date.day);
            printf("The profit during this period was $%.2lf.\n", max.profit);
        }
    }
}
