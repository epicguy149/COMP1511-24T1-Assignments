#ifndef CREPE_STAND_H
#define CREPE_STAND_H

//
// DO NOT CHANGE THIS FILE
//
// You do not submit this file. This file is not marked.
// If you think you need to change this file you have
// misunderstood the assignment - ask in the course forum.
//
// Assignment 2 24T1 COMP1511: Crepe_Stand
// crepe_stand.h
//
// You must not change this file.
//
// Version 1.0.0: Release

#define DEFAULT -1

#define VALID_CREPE 0
#define INVALID_BATTER -2
#define INVALID_TOPPING -3
#define INVALID_GFO -4
#define INVALID_SIZE -5
#define INVALID_POSITION -6

#define VALID_DAY 0
#define INVALID_DAY -7

#define NO_CREPES 1
#define SUCCESS 0

#define MAX_STR_LEN 32

// Provided enums:

// Represents the different types of batter a crepe can have.
enum batter_type {
    ORIGINAL,
    CHOCOLATE,
    MATCHA,
};

// Represents the different toppings a crepe can have.
enum topping_type {
    NO_TOPPING,
    BANANA,
    STRAWBERRY,
    NUTELLA,
};

// Date info - year, month, day
struct date {
    int year;
    int month;
    int day;
};

// Stores information about a particular day's orders
// counts:
//      - total crepes
//      - original batter crepes
//      - chocolate batter crepes
//      - matcha batter crepes
//      - gluten-free crepes
//      - small crepes
//      - medium crepes
//      - large crepes
// finds the most popular topping
struct stats {
    int total_crepes;
    int original;
    int chocolate;
    int matcha;
    int gluten_free;
    enum topping_type popular_topping;
    int small;
    int medium;
    int large;
};

// Stores information about a particular customer's bill
//      - total price for the customer to pay
//      - how many crepes the customer ordered
struct bill {
    double total_price;
    int crepe_count;
};

// Stores the amount of profits made across all weekdays and all weekends
struct profits {
    double weekday;
    double weekend;
};

// Stores information about the max profit period
//      - start date of the max profit period
//      - end date of the max profit period
//      - profit made during this period
struct max_profit {
    struct date start_date;
    struct date end_date;
    double profit;
};

// Forward declarations for opaque structs
struct crepe;
struct day;

////////////////////////////////////////////////////////////////////////
//                         Stage 1 Functions                          //
////////////////////////////////////////////////////////////////////////

// Creates a new day of crepe orders
// 
// Parameters:
//      new_date - date of the new day
// Returns:
//      pointer to newly created struct day
//
struct day *create_day(struct date new_date);

// Appends a newly ordered custom crepe to the day's orders
// 
// Parameters:
//      current_day     - current day being used in the list of days
//      customer_name   - name of customer ordering
//      batter          - batter base of crepe
//      topping         - topping for the crepe
//      is_gluten_free  - 1 if it is GF, 0 if not
//      diameter_cm     - how large crepe is in cm
// Returns:
//      a constant indicating success of appending to print the
//      appropriate message in main.c
//
int append_crepe_custom(struct day *current_day, 
                        char *customer_name, 
                        enum batter_type batter, 
                        enum topping_type topping, 
                        int is_gluten_free, 
                        int diameter_cm);

// Appends a newly ordered signature crepe to the day's orders
// 
// Parameters:
//      current_day     - current day being used in the list of days
//      customer_name   - name of customer ordering
//      crepe_type      - the type of signature crepe being ordered. 
//                        strawberry, matcha or chocolate.
// Returns:
//      a constant indicating success of appending to print the
//      appropriate message in main.c
//
int append_crepe_signature(struct day *current_day, 
                           char *customer_name, 
                           char *crepe_type);

// Prints the day's current orders.
// 
// Parameters:
//      current_day - current day being used in the list of days
// Returns:
//      nothing
//
void print_crepes(struct day *current_day);

// Counts the day's total count of crepes.
// 
// Parameters:
//      current_day - current day being used in the list of days
// Returns:
//      the number of crepes ordered that day
//
int count_crepes(struct day *current_day);

////////////////////////////////////////////////////////////////////////
//                         Stage 2 Functions                          //
////////////////////////////////////////////////////////////////////////

// Inserts a new custom crepe into the day's orders of crepes
// 
// Parameters:
//      current_day     - current day being used in the list of days
//      position        - position to insert the crepe at. e.g. 0 - first,
//                        1 - second
//      customer_name   - name of customer ordering
//      batter          - batter base of crepe
//      topping         - topping for the crepe
//      is_gluten_free  - 1 if it is GF, 0 if not
//      diameter_cm     - how large crepe is in cm
// Returns:
//      a constant indicating success of appending to print the
//      appropriate message in main.c
//
int insert_crepe_custom(struct day *current_day, 
                        int position, 
                        char *customer_name, 
                        enum batter_type batter, 
                        enum topping_type topping, 
                        int is_gluten_free, 
                        int diameter_cm);

// Inserts a new signature crepe into the day's orders of crepes
// 
// Parameters:
//      current_day     - current day being used in the list of days
//      position        - position to insert the crepe at. e.g. 0 - first,
//                        1 - second
//      customer_name   - name of customer ordering
//      batter          - batter base of crepe
//      topping         - topping for the crepe
//      is_gluten_free  - 1 if it is GF, 0 if not
//      diameter_cm     - how large crepe is in cm
// Returns:
//      a constant indicating success of appending to print the
//      appropriate message in main.c
//
int insert_crepe_signature(struct day *current_day, 
                           int position, 
                           char *customer_name, 
                           char *crepe_type);

// Calculates stats about what has been ordered so far on the current day
//      - number of total crepes, chocolate and matcha batters
//      - number of gluten-free crepes
//      - most popular topping
// 
// Parameters:
//      current_day - current day being used in the list of days
// Returns:
//      struct stat containing all the calculated statistics
//
struct stats get_stats(struct day *current_day);

// Calculates the price of the crepe in the given position
// 
// Parameters:
//      current_day - current day being used in the list of days
//      position    - position of crepe to find the price of
// Returns:
//      price of specified crepe
//
double calculate_price(struct day *current_day, int position);

// Calculates the total income for the given day
// 
// Parameters:
//      current_day - current day being used in the list of days
// Returns:
//      the total income for the given day
//
double total_income(struct day *current_day);

// Calculates the given customer's bill for the day and the amount of crepes
// ordered.
// 
// Parameters:
//      current_day     - current day being used in the list of days
//      customer_name   - string containing the name of the customer to
//                        calculate the bill for
// Returns:
//      a struct bill containing the total bill and the amount of crepes ordered
//
struct bill customer_bill(struct day *current_day, char *customer_name);

////////////////////////////////////////////////////////////////////////
//                         Stage 3 Functions                          //
////////////////////////////////////////////////////////////////////////

// Inserts a new day in chronological position
// 
// Parameters:
//      day_one - first day in the list of days
//      date - date of the new day to insert
// Returns:
//      the first day of the list of days chronologically
//
struct day *new_day(struct day *day_one, struct date date);

// Prints out all the days and their associated info; 
//      - whether it is the current day
//      - the date
//      - the most popular signature crepe
//      - total income for that day
// 
// Parameters:
//      day_one - first day in the list of days
//      current_day - the current day being viewed
// Returns:
//      nothing
//
void display_days(struct day *day_one, struct day *current_day);

// Cycles backward and forwards through the days
// 
// Parameters:
//      day_one - first day in the list of days
//      current_day - the current day
// Returns:
//      the new current day
//
struct day *cycle_days(char command, 
                       struct day *day_one, 
                       struct day *current_day);

// Removes the crepe in the specified position from the current day's 
// orders.
// 
// Parameters:
//      current_day - the current day
//      position    - position of the crepe to remove, indexed from 1
//                    e.g. 1: first crepe, 2: second crepe
// Returns:
//      a constant indicating if the crepe was removed successfully, or if
//      the given position was invalid.
//
int remove_crepe(struct day *current_day, int position);

// Removes the specified day from the list of days
// 
// Parameters:
//      day_one     - pointer to the first day in the list of days 
//                    chronologically
//      current_day - pointer to a pointer to the current day
//      remove_date - date to remove
// Returns:
//      a pointer to the first day in the list of days chronologically
//
struct day *remove_day(struct day *day_one, 
                       struct day **current_day, 
                       struct date date);

// Frees all days and their orders in the given list
// 
// Parameters:
//      day_one - first day in the list
// Returns:
//      nothing
// 
void free_crepe_stand(struct day *day_one);

////////////////////////////////////////////////////////////////////////
//                         Stage 4 Functions                          //
////////////////////////////////////////////////////////////////////////

// Calculates the total income for all weekdays and all weekends
// 
// Parameters:
//      day_one - first day in the list
// Returns:
//      a struct profits containing the profit made on the weekdays and
//      the weekends
//
struct profits compare_weekday_end(struct day *day_one);

// Calculates the maximum profit period for a given year.
// Finds the start date and end date for the period.
//
// Parameters:
//      day_one - pointer to the first day in the list
//      year - year to find the max profit period of
// Returns:
//      a struct max_profit containing:
//          - the date of the start of the profit period
//          - the date of the end of the profit period
//          - the profit made during that period
//
struct max_profit max_profit_period(struct day *day_one, int year);

////////////////////////////////////////////////////////////////////////
//                          Helper Functions                          //
////////////////////////////////////////////////////////////////////////

// Prints out the details of a single crepe
//
// Parameters:
//      crepe       - the crepe to be printed
//      position    - position of the crepe in the orders list, 
//                    indexed from 1
// Returns:
//      nothing
//
void print_single_crepe(struct crepe *crepe, int position);

// Prints an arrow to use when printing crepes and days
// 
// Parameters:
//      nothing
// Returns:
//      nothing
//
void print_arrow(void);

void print_no_crepes(void);

// Prints out the details of a single day
// 
// Parameters:
//      is_current_day - whether or not the day being printed is the current one
//      date           - the date string of the day
//      most_pop       - char indicating the most popular signature crepe:
//                          - 's': strawberry special
//                          - 'm': matcha madness
//                          - 'c': chocolate connoisseur
//      income         - income made on the day
//
void print_single_day(int is_current_day, 
                      struct date date, 
                      char most_pop, 
                      double income);

#endif
