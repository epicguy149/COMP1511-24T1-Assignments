// Assignment 2 24T1 COMP1511: Crepe Stand
// crepe_stand.c
//
// This program was written by Chai Jengyi (z5524645)
// on 2024-04-08
//
// Version 1.0.0: Assignment released.
//
// BUM

#include "crepe_stand.h"

#include <ctype.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SIG_STRAWBERRY 's'
#define SIG_MATCHA 'm'
#define SIG_CHOCOLATE 'c'
#define NO_SIG_SOLD 'n'
#define NO_SIG_MOST_POP 'N'

// All types of signature crepes possible
enum crepe_signature_type {
    NOT_SIGNATURE,
    STRAWBERRY_SPECIAL,
    MATCHA_MADNESS,
    CHOCOLATE_CONNOISSEUR
};

// Signature crepes orders within a day
struct signature {
    int strawberries;
    int matchas;
    int chocolates;
};

////////////////////////////////////////////////////////////////////////////////

// A single crepe
//      Stores all the information about a single crepe.
//      Forms a linked list of crepes by pointing to the next one (or NULL).
struct crepe {
    // Name of the customer who ordered the crepe
    char customer_name[MAX_STR_LEN];

    // The batter the crepe will be made out of
    // ORIGINAL, CHOCOLATE or MATCHA
    enum batter_type batter;

    // There can be no topping or one topping
    // NO_TOPPING, BANANA, STRAWBERRY, NUTELLA
    enum topping_type topping;

    // Whether or not the crepe is gluten free;
    // Can be 1 or 0
    int is_gluten_free;

    // Size of the crepe in centimeters
    int diameter_cm;

    // A pointer to the next crepe in the orders list
    // Is NULL if this crepe is the tail of the list.
    struct crepe *next;

    // The signature type of the crepe
    // NOT_SIGNATURE, STRAWBERRY_SPECIAL, MATCHA_MADNESS, CHOCOLATE_CONNOISSEUR
    enum crepe_signature_type signature_type;
};

// A particular day’s list of orders
struct day {
    // Date of the day as a struct day, with fields year, month, day
    struct date date;

    // A pointer to the start of the list of this day's orders
    struct crepe *orders;

    // The total income for the day
    // i.e. the total summed price of all crepes
    double income;

    // A pointer to the next day's orders
    // Is NULL if this day is the tail of the list.
    struct day *next;

    // Signature orders made within a day,
    // Can be strawberry, matcha or chocolate
    struct signature signature_orders;
};


////////////////////////////////////////////////////////////////////////////////

// Add prototypes for any extra functions you create here.

// Stage 1

// Creates a new custom crepe.
//
// Parameters:
//      customer_name - name of customer ordering
//      batter - batter base of crepe
//      topping - topping for the crepe
//      is_gluten_free - 1 if it is GF, 0 if not
//      diameter_cm - how large crepe is in cm
// Returns:
//      pointer to newly created struct crepe.
//
struct crepe *create_crepe(char *customer_name,
                           enum batter_type batter,
                           enum topping_type topping,
                           int is_gluten_free,
                           int diameter_cm);

int validate_crepe(enum batter_type batter,
                   enum topping_type topping,
                   int is_gluten_free,
                   int diameter_cm);

void append_onto_orders(struct crepe **orders,
                        struct crepe *new_crepe);

// Stage 2

void insert_into_orders(struct crepe **orders,
                        struct crepe *new_crepe,
                        int position);

// Stage 3

int compare_dates(struct date date_one,
                  struct date date_two);

char calculate_popular_sig(struct signature signature_orders);

void free_crepes(struct day *day);

void decrement_signature_order(struct day *current_day,
                               enum crepe_signature_type signature_type);

// Stage 4
int check_day(struct day *day);

double calculate_daily_profit(struct day *current_day);

int sale_in_year(struct day *first_day, int year);

//////////////////////////////////////////////////////////////////////
//                        Stage 1 Functions                         //
//////////////////////////////////////////////////////////////////////

// Provided Function stubs:

// Creates a new day by allocating memory for it and initalizes it with a given
// date, sets all the rest of the values of the day to starter/NULL values.
//
struct day *create_day(struct date new_date) {
    struct day *new_day = malloc(sizeof(struct day));

    // initialize values
    new_day->date.year = new_date.year;
    new_day->date.month = new_date.month;
    new_day->date.day = new_date.day;

    new_day->signature_orders.strawberries = 0;
    new_day->signature_orders.matchas = 0;
    new_day->signature_orders.chocolates = 0;

    new_day->orders = NULL;
    new_day->income = 0.0;
    new_day->next = NULL;

    return new_day;
}

// Creates a new crepe by allocating memory for it and initializes it with
// the provided values.
//
struct crepe *create_crepe(char *customer_name,
                           enum batter_type batter,
                           enum topping_type topping,
                           int is_gluten_free,
                           int diameter_cm
) {
    struct crepe *new_crepe = malloc(sizeof(struct crepe));

    // copy values
    strncpy(new_crepe->customer_name, customer_name, MAX_STR_LEN);
    new_crepe->batter = batter;
    new_crepe->topping = topping;
    new_crepe->is_gluten_free = is_gluten_free;
    new_crepe->diameter_cm = diameter_cm;
    new_crepe->signature_type = NOT_SIGNATURE;

    // point to net node
    new_crepe->next = NULL;

    return new_crepe;
}

// Appends a custom crepe order to the current day's list of orders.
// Validates the provided values then creates and appends a crepe to the end
// of the list of orders.
//
int append_crepe_custom(struct day *current_day,
                        char *customer_name,
                        enum batter_type batter,
                        enum topping_type topping,
                        int is_gluten_free,
                        int diameter_cm
) {
    int validation_result = validate_crepe(batter, topping, is_gluten_free,
                                           diameter_cm);

    if (validation_result != VALID_CREPE) {
        return validation_result;
    }

    struct crepe *new_crepe = create_crepe(customer_name, batter, topping,
                                           is_gluten_free, diameter_cm);

    append_onto_orders(&(current_day->orders), new_crepe);

    return VALID_CREPE;
}

// Appends a signature crepe order to the current day's list of orders.
// Initializes the values of the crepe based on the crepe_type, it then creates
// and appends the crepe at the end of the list of orders.
//
int append_crepe_signature(struct day *current_day,
                           char *customer_name,
                           char *crepe_type
) {
    // initialize all the values of the crepe
    enum batter_type batter;
    enum crepe_signature_type signature_type;
    enum topping_type topping;
    int is_gluten_free;
    int diameter_cm = 25;

    // initialize values based on crepe_type
    if (strcmp(crepe_type, "matcha") == 0) {
        batter = MATCHA;
        topping = STRAWBERRY;
        is_gluten_free = 0;
        signature_type = MATCHA_MADNESS;
        current_day->signature_orders.matchas++;
    } else if (strcmp(crepe_type, "strawberry") == 0) {
        batter = ORIGINAL;
        topping = STRAWBERRY;
        is_gluten_free = 1;
        signature_type = STRAWBERRY_SPECIAL;
        current_day->signature_orders.strawberries++;
    } else if (strcmp(crepe_type, "chocolate") == 0) {
        batter = CHOCOLATE;
        topping = NUTELLA;
        is_gluten_free = 0;
        signature_type = CHOCOLATE_CONNOISSEUR;
        current_day->signature_orders.chocolates++;
    } else {
        // invalid signature crepe type
        return 1;
    }

    int validation_result = validate_crepe(batter, topping, is_gluten_free,
                                           diameter_cm);

    if (validation_result != VALID_CREPE) {
        return validation_result;
    }

    struct crepe *new_crepe = create_crepe(customer_name, batter, topping,
                                           is_gluten_free, diameter_cm);

    new_crepe->signature_type = signature_type;

    append_onto_orders(&(current_day->orders), new_crepe);

    return VALID_CREPE;
}

// Prints the list of crepe orders in a given day. Iterates through
// each crepe in the list and prints the details of each crepe.
//
void print_crepes(struct day *current_day) {
    if (current_day->orders == NULL) {
        print_no_crepes();
        return;
    }

    struct crepe *current_crepe = current_day->orders;
    int position = 1;

    while (current_crepe != NULL) {
        print_single_crepe(current_crepe, position);

        if (current_crepe->next != NULL) {
            print_arrow();
        }
        position++;
        current_crepe = current_crepe->next;
    }
}

// Counts the number of crepes ordered in a given day. Iterates through
// the list of crepes, incrementing a counter after each crepe.
//
int count_crepes(struct day *current_day) {
    if (current_day->orders == NULL) {
        return 0;
    }

    int crepe_count = 0;

    struct crepe *current_crepe = current_day->orders;

    while (current_crepe != NULL) {
        crepe_count++;
        current_crepe = current_crepe->next;
    }

    return crepe_count;
}

// Your functions go here (include function comments):

// Does the actual appending of the crepe onto the list of orders, goes to 
// end of the list of orders, then appends new crepe onto the end of the list
//
// Parameters:
//      orders - points to the pointer to the current day's orders
//      new_crepe - the new crepe created
//
void append_onto_orders(struct crepe **orders, struct crepe *new_crepe) {
    if (*orders == NULL) {
        *orders = new_crepe;
    } else {
        struct crepe *current_crepe = *orders;

        while (current_crepe->next != NULL) {
            current_crepe = current_crepe->next;
        }

        current_crepe->next = new_crepe;
    }
}

// Checks if the values input by user are valid by comparing it to the
// constraints each corresponding field has.
//
// Parameters:
//      batter - batter base of crepe
//      topping - topping for the crepe
//      is_gluten_free - 1 if it is GF, 0 if not
//      diameter_cm - how large crepe is in cm
// Returns:
//      1 if crepe is valid, corresponding error if invalid.
//
int validate_crepe(enum batter_type batter,
                   enum topping_type topping,
                   int is_gluten_free,
                   int diameter_cm
) {
    if (batter < ORIGINAL || batter > MATCHA) {
        return INVALID_BATTER;
    }

    if (topping < NO_TOPPING || topping > NUTELLA) {
        return INVALID_TOPPING;
    }

    if (is_gluten_free != 0 && is_gluten_free != 1) {
        return INVALID_GFO;
    }

    if (diameter_cm < 10 || diameter_cm >= 40) {
        return INVALID_SIZE;
    }

    return VALID_CREPE;
}

//////////////////////////////////////////////////////////////////////
//                        Stage 2 Functions                         //
//////////////////////////////////////////////////////////////////////

// Provided function stubs:

// Inserts a custom crepe to a specified position in the list of the current
// day's orders.
// Iterates through the list of orders until the position is found, then
// inserts the crepe at that position.
// If the position provided is not in the list, appends at the end of the list.
//
int insert_crepe_custom(struct day *current_day,
                        int position,
                        char *customer_name,
                        enum batter_type batter,
                        enum topping_type topping,
                        int is_gluten_free,
                        int diameter_cm
) {
    if (position < 1) {
        return INVALID_POSITION;
    }

    int validation_result = validate_crepe(batter, topping, is_gluten_free,
                                           diameter_cm);

    if (validation_result != VALID_CREPE) {
        return validation_result;
    }

    struct crepe *new_crepe = create_crepe(customer_name, batter, topping,
                                           is_gluten_free, diameter_cm);

    insert_into_orders(&(current_day->orders), new_crepe, position);

    return VALID_CREPE;
}

// Inserts a signature crepe to a specified position in the list of the current
// day's orders.
// Initializes the values of the crepe based on the crepe_type.
// Iterates through the list of orders until the position is found, then
// inserts the crepe at that position.
// If the position provided is not in the list, appends at the end of the list.
// If position provided is invalid return INVALID_POSITION
//
int insert_crepe_signature(struct day *current_day,
                           int position,
                           char *customer_name,
                           char *crepe_type
) {
    enum batter_type batter;
    enum crepe_signature_type signature_type;
    enum topping_type topping;
    int is_gluten_free;
    int diameter_cm = 25;

    if (position < 1) {
        return INVALID_POSITION;
    }

    if (strcmp(crepe_type, "matcha") == 0) {
        batter = MATCHA;
        topping = STRAWBERRY;
        is_gluten_free = 0;
        signature_type = MATCHA_MADNESS;
        current_day->signature_orders.matchas++;
    } else if (strcmp(crepe_type, "strawberry") == 0) {
        batter = ORIGINAL;
        topping = STRAWBERRY;
        is_gluten_free = 1;
        signature_type = STRAWBERRY_SPECIAL;
        current_day->signature_orders.strawberries++;
    } else if (strcmp(crepe_type, "chocolate") == 0) {
        batter = CHOCOLATE;
        topping = NUTELLA;
        is_gluten_free = 0;
        signature_type = CHOCOLATE_CONNOISSEUR;
        current_day->signature_orders.chocolates++;
    } else {
        return 1;
    }

    int validation_result = validate_crepe(batter, topping, is_gluten_free,
                                           diameter_cm);

    if (validation_result != VALID_CREPE) {
        return validation_result;
    }

    struct crepe *new_crepe = create_crepe(customer_name, batter, topping,
                                           is_gluten_free, diameter_cm);

    new_crepe->signature_type = signature_type;

    insert_into_orders(&(current_day->orders), new_crepe, position);

    return VALID_CREPE;
}

// Calculates the statistics for a day's crepe orders.
// Iterates through the list of orders and tallies counts for each property,
// also determines the most popular topping in a day based on the counts.
//
struct stats get_stats(struct day *current_day) {
    // create new struct and set all fields to 0
    struct stats day_stats = {0};

    struct crepe *current_crepe = current_day->orders;

    int topping_counts[] = {0, 0, 0, 0};

    while (current_crepe != NULL) {
        day_stats.total_crepes++;

        // check batter count
        if (current_crepe->batter == ORIGINAL) {
            day_stats.original++;
        } else if (current_crepe->batter == CHOCOLATE) {
            day_stats.chocolate++;
        } else if (current_crepe->batter == MATCHA) {
            day_stats.matcha++;
        }

        // check gluten-free count
        if (current_crepe->is_gluten_free) {
            day_stats.gluten_free++;
        }

        // check topping counters
        topping_counts[current_crepe->topping]++;

        // categorizes size of crepe based on its diameter
        if (current_crepe->diameter_cm < 20) {
            day_stats.small++;
        } else if (current_crepe->diameter_cm < 30) {
            day_stats.medium++;
        } else if (current_crepe->diameter_cm < 40) {
            day_stats.large++;
        }

        current_crepe = current_crepe->next;
    }
    int highest_count = 0;
    int same_count = 0;

    // determines the most popular topping by comparing their counts
    for (int i = 0; i < 4; i++) {
        if (topping_counts[i] > highest_count) {
            highest_count = topping_counts[i];
            day_stats.popular_topping = i;
            same_count = 0;
        } else if (topping_counts[i] == highest_count && highest_count != 0) {
            same_count = 1;
        }
    }

    // if there are equally popular toppings, then NO_TOPPING is most popular
    if (same_count == 1 || highest_count == topping_counts[0]) {
        day_stats.popular_topping = NO_TOPPING;
    }

    return day_stats;
}

// Calculates price of a specific crepe in a specific position based on its
// properties.
//
double calculate_price(struct day *current_day, int position) {
    struct crepe *current_crepe = current_day->orders;
    double total_price = 0.0;
    int current_position = 1;

    while (current_position < position && current_crepe != NULL) {
        current_position++;
        current_crepe = current_crepe->next;
    }

    if (current_crepe == NULL || position < 1) {
        return INVALID_POSITION;
    }

    // calculates the price of crepe based on its properties
    if (current_crepe->batter == ORIGINAL) {
        total_price += 8.00;
    } else if (current_crepe->batter == CHOCOLATE) {
        total_price += 8.00;
    } else if (current_crepe->batter == MATCHA) {
        total_price += 9.50;
    }

    if (current_crepe->topping == BANANA ||
        current_crepe->topping == STRAWBERRY) {
        total_price += 2.00;
    } else if (current_crepe->topping == NUTELLA) {
        total_price += 3.50;
    }

    if (current_crepe->is_gluten_free) {
        total_price += 0.60;
    }

    if (current_crepe->diameter_cm >= 30) {
        total_price += 3.50;
    } else if (current_crepe->diameter_cm >= 20) {
        total_price += 2.50;
    }

    return total_price;
}

// Calculates the total income generated in a given day.
// Iterates theough the list of orders in the day, summing the price of each
// crepe.
//
double total_income(struct day *current_day) {
    struct crepe *current_crepe = current_day->orders;

    if (current_crepe == NULL) {
        return 0;
    }

    int position = 1;
    double income = 0.0;

    while (current_crepe != NULL) {
        income += calculate_price(current_day, position);
        position++;
        current_crepe = current_crepe->next;
    }

    return income;
}

// Calculates the bill and count of crepes for a specific customer based
// on the name given.
// Iterates through the list of orders, and looks for orders made by a specific
// customer, then sums the total price of orders made by customer.
//
struct bill customer_bill(struct day *current_day, char *customer_name) {
    struct crepe *current_crepe = current_day->orders;
    struct bill current_customer = {0.0, 0};
    int position = 1;

    while (current_crepe != NULL) {
        // check if order is made by the specific customer
        if (strcmp(current_crepe->customer_name, customer_name) == 0) {
            
            current_customer.total_price +=
                calculate_price(current_day, position);
                
            current_customer.crepe_count++;
        }
        position++;
        current_crepe = current_crepe->next;
    }
    return current_customer;
}

// Your functions go here (include function comments):

// Does the insertion of a new crepe into a list of orders at a specified
// position. If the position provided is outside of the list, append the crepe
// to the end of the list.
//
// Parameters:
//      orders: points to the pointer to the current day's orders
//      new_crepe: the new crepe created
//      position: the position in the list that the crepe should take
//
void insert_into_orders(struct crepe **orders,
                        struct crepe *new_crepe,
                        int position
) {
    struct crepe *current_crepe = NULL;

    // check if new_crepe is the first order of the day
    if (*orders == NULL || position == 1) {
        new_crepe->next = *orders;
        *orders = new_crepe;
    } else {
        // if not, iterate through list until the we reach the end
        current_crepe = *orders;

        int current_position = 1;

        while (current_crepe->next != NULL && current_position < position - 1) {
            current_crepe = current_crepe->next;
            current_position++;
        }
        new_crepe->next = current_crepe->next;
        current_crepe->next = new_crepe;
    }

    current_crepe = new_crepe->next;
    while (current_crepe != NULL) {
        current_crepe = current_crepe->next;
    }
}

//////////////////////////////////////////////////////////////////////
//                        Stage 3 Functions                         //
//////////////////////////////////////////////////////////////////////

// Provided function stubs:

// Creates a new day and inserts the new day into the list of days in its
// appropriate position, which is based on its date.
// The days in the list are sorted ascendingly based on their dates.
//
struct day *new_day(struct day *day_one, struct date date) {
    struct day *new_day = create_day(date);
    struct day *current_day = day_one;

    // check if new date is earlier than head
    if (compare_dates(day_one->date, date) == -1) {
        new_day->next = day_one;
        return new_day;
    }

    // checks if the new day has the same date as the first day in the list
    if (compare_dates(current_day->date, date) == 0) {
        return NULL;
    }


    // iterate through the list as long as new_day's date is earlier than the
    // next date
    while (current_day->next != NULL &&
           compare_dates(current_day->next->date, date) == 1) {
        
        // check if new date already in list
        if (compare_dates(current_day->next->date, date) == 0) {
            return NULL;
        }

        current_day = current_day->next;
    }

    // inserts the new day right before the next day which has a later date
    new_day->next = current_day->next;
    current_day->next = new_day;

    return day_one;
}

// Displays all the days in the list of days by iterating through the list
// and printing its details.
//
void display_days(struct day *day_one, struct day *current_day) {

    struct day *this_day = day_one;
    int is_current_day = 0;
    double income = 0.0;
    char most_pop;

    while (this_day != NULL) {
        // checks if this_day is the current day
        if (compare_dates(this_day->date, current_day->date) == 0) {
            is_current_day = 1;
        }

        most_pop = calculate_popular_sig(this_day->signature_orders);
        income = total_income(this_day);

        print_single_day(is_current_day, this_day->date, most_pop, income);

        if (this_day->next != NULL) {
            print_arrow();
        }

        is_current_day = 0;
        this_day = this_day->next;
    }
}

// Changes the current day by cycling through the list of days forwards or
// backwards based on the user input.
// Iterates through the list of days until the current day is found, then
// changes the current day to the next or previous day based on command.
//
struct day *cycle_days(char command,
                       struct day *day_one,
                       struct day *current_day
) {
    struct day *this_day = day_one;

    if (command == '>') {
        // if there is no next day, cycle to day_one
        if (current_day->next == NULL) {
            return day_one;
        } else {
            return (current_day->next);
        }

    } else if (command == '<') {
        // if current_day is the first day, jump to the end of the list
        if (current_day == day_one) {
            while (this_day->next != NULL) {
                this_day = this_day->next;
            }
            return this_day;
        }

        // look for day where next is current_day, changes current_day to the
        // day before the current day
        while (this_day != NULL) {
            if (this_day->next == current_day) {
                return this_day;
            }
            this_day = this_day->next;
        }
    }

    return NULL;
}

// Removes a crepe from a day's list of orders based on the position input
// by user.
// Iterates through the list of orders until crepe with the corresponding
// position is found, then removes the crepe from the list of days by
// disconnecting it from the list.
// Also frees the memory allocated to the removed crepe.
//
int remove_crepe(struct day *current_day, int position) {
    if (position < 1) {
        return INVALID_POSITION;
    }

    struct crepe *current_crepe = current_day->orders;
    struct crepe *previous_crepe = NULL;
    int current_position = 1;

    if (current_crepe == NULL) {
        return INVALID_POSITION;
    }

    if (position == 1) {
        if (current_crepe->next == NULL) {
            current_day->orders = NULL;
        } else {
            current_day->orders = current_crepe->next;
        }

        decrement_signature_order(current_day, current_crepe->signature_type);
        free(current_crepe);
        return VALID_CREPE;
    }

    // iterates through the list of orders until the position is found
    while (current_crepe != NULL && current_position < position) {
        previous_crepe = current_crepe;
        current_position++;
        current_crepe = current_crepe->next;
    }

    // check if the position provided is outside the list of orders,
    if (current_crepe == NULL) {
        return INVALID_POSITION;
    }

    decrement_signature_order(current_day, current_crepe->signature_type);

    previous_crepe->next = current_crepe->next;
    free(current_crepe);
    return VALID_CREPE;
}

// Removes a day from the list of days based on the date provided.
// If the day being removed is the first day, check if its the only day in list
// If it is the only day in the list, create a new day with a default date
// and make the new day the head of the list of days.
// If there are other days, make the next day the new head of the list.
//
struct day *remove_day(struct day *day_one,
                       struct day **current_day,
                       struct date date
) {
    struct date replacement_date = {2024, 1, 1};
    struct day *this_day = day_one;
    struct day *next_day = this_day->next;

    if (compare_dates(day_one->date, date) == 0) {
        free_crepes(day_one);
        free(day_one);

        if (next_day == NULL) {
            struct day *new_day = create_day(replacement_date);
            *current_day = new_day;
            return new_day;

        } else {
            if (this_day == *current_day) {
                *current_day = next_day;
            }
            return next_day;
        }
    }

    struct day *previous_day = NULL;

    // iterate through the list of days until a day with matching date is found
    while (this_day != NULL && (compare_dates(this_day->date, date) != 0)) {
        previous_day = this_day;
        this_day = this_day->next;
    }

    if (this_day == NULL) {
        return NULL;
    }

    previous_day->next = this_day->next;

    if (this_day == *current_day) {
        if (this_day->next != NULL) {
            *current_day = this_day->next;
        } else {
            *current_day = day_one;
        }
    }

    free_crepes(this_day);
    free(this_day);

    return day_one;
}

// Frees all the memory allocated to the crepe stand, which is all the days and
// their orders.
//
void free_crepe_stand(struct day *day_one) {
    struct day *current_day = day_one;
    struct day *next_day = NULL;

    while (current_day != NULL) {
        next_day = current_day->next;
        free_crepes(current_day);
        free(current_day);
        current_day = next_day;
    }
}

// Your functions here (include function comments):

// Frees all the memory allocated to the crepes in a day's list of orders.
// Iterates through the list of crepes, then freeing the memory allocated to
// them.
//
// Parameters:
//      day: pointer to the day we want to remove
//
void free_crepes(struct day *day) {
    struct crepe *current_crepe = day->orders;
    struct crepe *next_crepe = NULL;

    while (current_crepe != NULL) {
        next_crepe = current_crepe->next;
        free(current_crepe);
        current_crepe = next_crepe;
    }
}

// Decrements the count of a signature crepe type in a day.
// Function is called when a crepe is removed to ensure that the counts are
// accurate.
//
// Parameters:
//      current_day - the current day, from which we are removing a crepe
//      signature_type - the type of signature crepe being removed
//
void decrement_signature_order(struct day *current_day,
                               enum crepe_signature_type signature_type) {
    if (signature_type == STRAWBERRY_SPECIAL) {
        current_day->signature_orders.strawberries--;
    } else if (signature_type == MATCHA_MADNESS) {
        current_day->signature_orders.matchas--;
    } else if (signature_type == CHOCOLATE_CONNOISSEUR) {
        current_day->signature_orders.chocolates--;
    }
}

// Calculates the most signature crepe type in a day based on the counts.
// Also checks for ties, where are there are multiple crepe types that are
// equally the most popular.
//
// Parameters:
//      signature_orders - a struct containing the values of signature
//                         orders made in a day
// Returns:
//      SIG_STRAWBERRY, SIG_MATCHA, SIG_CHOCOLATE
//
char calculate_popular_sig(struct signature signature_orders) {
    int highest_count = signature_orders.strawberries;
    char most_pop = SIG_STRAWBERRY;
    int tie = 0;

    if (signature_orders.matchas > highest_count) {
        highest_count = signature_orders.matchas;
        most_pop = SIG_MATCHA;
    } else if (signature_orders.matchas == highest_count) {
        tie = 1;
    }

    if (signature_orders.chocolates > highest_count) {
        highest_count = signature_orders.chocolates;
        most_pop = SIG_CHOCOLATE;
        tie = 0;
    } else if (signature_orders.chocolates == highest_count) {
        tie = 1;
    }

    if (highest_count == 0) {
        return NO_SIG_SOLD;
    } else if (tie == 1) {
        return NO_SIG_MOST_POP;
    }

    return most_pop;
}

// Compares two dates and determines their relationship.
// Comparison is made based on date_two, so if date_two is earlier than
// date_one, return -1, else if date_two is the same as date_one return 0.
// If date_two is later then date_one return 1.
//
// Parameters:
//      date_one - the first date used for comparison
//      date_two - the second date used for comparison
// Returns:
//      1 if date_two is later than date_1, 0 if the same, -1 if earlier
//
int compare_dates(struct date date_one, struct date date_two) {

    if (date_two.year < date_one.year) {
        return -1;

    } else if (date_two.year == date_one.year) {
        // if year is the same, check month
        if (date_two.month < date_one.month) {
            return -1;

        } else if (date_two.month == date_one.month) {
            // if month is the same, check day
            if (date_two.day < date_one.day) {
                return -1;

            } else if (date_two.day == date_one.day) {
                return 0;
            }
        }
    }

    return 1;
}

//////////////////////////////////////////////////////////////////////
//                        Stage 4 Functions                         //
//////////////////////////////////////////////////////////////////////

// Provided function stubs:

// Calculates the amount of profit generated on weekends and weekdays across
// all days in the list.
// Then the profits are seperated and calculated based on whether they are 
// weekdays or weekends.
//
struct profits compare_weekday_end(struct day *day_one) {
    struct profits profits = {0, 0};
    struct day *current_day = day_one;
    double income = 0;

    while (current_day != NULL) {
        if (check_day(current_day)) {
            // weekday
            income = total_income(current_day) - 35;
            profits.weekday += income;
        } else {
            // weekend
            income = total_income(current_day) - 50;
            profits.weekend += income;
        }
        current_day = current_day->next;
    }
    return profits;
}

// Calculates the period with the maximum profit within a specific year.
// Finds the span of days within the year that holds the most profit.
// Iterates through all possible span of days, from 1 to the total number 
// of days in the list.
// Then with each possible span of days, iterates through the list of days,
// then records the period with the highest profit.
//
struct max_profit max_profit_period(struct day *day_one, int year) {
    struct day *current_day = day_one;
    struct day *first_day = NULL;
    struct max_profit max_period = {{0, 0, 0}, {0, 0, 0}, -99999};
    int days_in_year = 0;

    // look for first day in the year and counts the number of days in the year
    while (current_day != NULL && current_day->date.year == year) {
        if (first_day == NULL) {
            first_day = current_day;
        }
        days_in_year++;
        current_day = current_day->next;
    }

    if (day_one->next == NULL) {
        max_period.start_date = day_one->date;
        max_period.end_date = day_one->date;

        max_period.end_date.year = 0;

        // check if there have been any crepes sold in the year
        if (sale_in_year(day_one, year) && day_one->date.year == year) {
            max_period.profit = calculate_daily_profit(day_one);

        } else {
            max_period.start_date.year = 0;
        }

        return max_period;
    }

    struct day *span_start = first_day;
    struct day *previous_day = NULL;

    double current_profit = 0;
    int max_profit_span = 0;

    // increase span_size to cover more days until span_size equals number of
    // days in year
    for (int span_size = 1; span_size <= days_in_year; span_size++) {
        // resets start of each span to start from the first day in the year
        span_start = first_day;

        // move span across days until all days are covered
        for (int i = 0; i <= days_in_year - span_size; i++) {
            current_profit = 0;
            current_day = span_start;

            // calculate total profit in span
            for (int j = 0; j < span_size; j++) {
                current_profit += calculate_daily_profit(current_day);
                previous_day = current_day;
                current_day = current_day->next;
            }

            // check if profit in span is largest
            if (current_profit > max_period.profit) {
                max_period.profit = current_profit;
                max_period.start_date = span_start->date;
                max_period.end_date = previous_day->date;
                max_profit_span = span_size;
            }

            // move span to start from next day in the list
            span_start = span_start->next;
        }
    }

    if (max_profit_span == 1) {
        max_period.end_date.year = 0;
    }

    return max_period;
}

// Your functions here (include function comments):

// Checks if there have been any crepes sold within a year.
// Iterates through list of days and checks if there have been any orders made.
//
// Parameters:
//      first_day - the first day in the year
//      year - the year to check
//
// Returns:
//      1 if there has been at least 1 sale, 0 if no sales
//
int sale_in_year(struct day *first_day, int year) {
    struct day *current_day = first_day;

    while (current_day != NULL && current_day->date.year == year) {

        if (current_day->orders != NULL) {
            return 1;
        }
        current_day = current_day->next;
    }

    return 0;
}

// Calculates profit on a certain day based on whether it is a weekday or
// weekend.
//
// Parameters:
//      current_day - pointer to the day we want to find the profit for
// Returns:
//      the amount of profit generated in the day
//
double calculate_daily_profit(struct day *current_day) {
    double profit = 0;

    // check if weekday or weekend
    if (check_day(current_day)) {
        // calculates profit based on weekday or weekend
        profit += total_income(current_day) - 35;
    } else {
        profit += total_income(current_day) - 50;
    }

    return profit;
}

// Checks if certain date is a weekend or weekday using Zeller's congruence.
// If day_of_week is 0 or 1, it is a weekend, return 0.
// If day_of_week is 2,3,4,5,6, it is a weekday, return 1.
//
// Parameters:
//      day - pointer to the day we want to check
// Returns:
//      0 or 1 if weekend, numbers 2-6 is weekday
//
int check_day(struct day *current_day) {
    int year = current_day->date.year;
    int month = current_day->date.month;
    int day_of_month = current_day->date.day;

    // zellers congruence
    if (month < 3) {
        month += 12;
        year -= 1;
    }

    int year_of_century = year % 100;
    int century = year / 100;
    int day_of_week = (day_of_month + (13 * (month + 1) / 5) +
                       year_of_century + (year_of_century / 4) +
                       (century / 4) + (5 * century)) %
                      7;

    // if day_of_week is 1 or 0 it is a weekend
    if (day_of_week == 1 || day_of_week == 0) {
        // weekend
        return 0;
    } else {
        // weekday
        return 1;
    }
}

//////////////////////////////////////////////////////////////////////
//                         Helper Functions                         //
//////////////////////////////////////////////////////////////////////

void print_single_crepe(struct crepe *crepe, int position) {
    printf("---------------------\n");
    printf("%d. %s's crepe\n", position, crepe->customer_name);

    if (crepe->batter == ORIGINAL) {
        printf("Original flavour\n");
    } else if (crepe->batter == MATCHA) {
        printf("Matcha flavour\n");
    } else {
        printf("Chocolate flavour\n");
    }

    printf("Toppings: ");
    if (crepe->topping == NO_TOPPING) {
        printf("none\n");
    } else if (crepe->topping == BANANA) {
        printf("bananas\n");
    } else if (crepe->topping == STRAWBERRY) {
        printf("strawberries\n");
    } else if (crepe->topping == NUTELLA) {
        printf("nutella\n");
    }

    printf("gluten free? ");
    if (crepe->is_gluten_free) {
        printf("yes\n");
    } else {
        printf("no\n");
    }

    printf("Size: %dcm\n", crepe->diameter_cm);
    printf("---------------------\n");
}

void print_arrow(void) {
    printf("\t|\n\tv\n");
}

void print_no_crepes(void) {
    printf("No crepes have been ordered yet!\n");
}

void print_single_day(int is_current_day,
                      struct date date,
                      char most_pop,
                      double income) {
    printf("---------------------\n");
    if (is_current_day) {
        printf("!CURRENT DAY!\n");
    }
    printf("Date: %04d-%02d-%02d\n", date.year, date.month, date.day);
    if (most_pop == SIG_STRAWBERRY) {
        printf("Most popular signature crepe: Strawberry Special\n");
    } else if (most_pop == SIG_MATCHA) {
        printf("Most popular signature crepe: Matcha Madness\n");
    } else if (most_pop == SIG_CHOCOLATE) {
        printf("Most popular signature crepe: Chocolate Connoisseur\n");
    } else if (most_pop == NO_SIG_SOLD) {
        printf("There were no signature crepes sold!\n");
    } else if (most_pop == NO_SIG_MOST_POP) {
        printf("No signature crepe was more popular than another!\n");
    } else {
        printf("ERROR: %c, unknown most popular character\n", most_pop);
    }
    printf("Total income: $%.2lf\n", income);
    printf("---------------------\n");
}
