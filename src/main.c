#define _CRT_SECURE_NO_WARNINGS
#include "../include/interface.h"

void print_result_prompt(int result)
{
    switch (result)
    {
    case RESULT_SUCCESS:
        printf("Operation completed successfully\n");
        break;

    case RESULT_ERROR_UNKNOWN:
        printf("An unknown error occured...\n");
        break;

    default:
        break;
    }
}

int main()
{
    sqlite3* db;
    if (sqlite3_open("./db/nutrition.db", &db) != SQLITE_OK) {
        printf("Failed to open database: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return 1;
    }

    int usr_input;
    int result = RESULT_SUCCESS;

    do
    {
        printf("1 - Login\n"
            "2 - Register\n");
        printf("Enter the number of the option (enter a non-number to exit): ");

        if (scanf("%d", &usr_input) == 0)
        {
            result = RESULT_USER_EXIT;
            continue;
        }

        getchar();

        switch (usr_input)
        {
            int usr_id;

        case 1:
            result = authorization(db, &usr_id);

            if (result == RESULT_SUCCESS)
            {
                if (usr_id != 0)
                {
                    do
                    {
                        result = disp_client(db, usr_id);

                        if (result == RESULT_ERROR_UNKNOWN)
                        {
                            break;
                        }

                        printf("---------------------------------");
                        printf("1 - Order plan\n"
                            "2 - Update info...\n"
                            "3 - Delete account\n"
                            "4 - Log out\n");
                        printf("---------------------------------");
                        printf("Enter the number of the option: ");
                        usr_input = -1;
                        scanf("%d", &usr_input);

                        switch (usr_input)
                        {
                        case 1:
                            result = make_order(db, usr_id);
                            print_result_prompt(result);
                            result = RESULT_SUCCESS;
                            break;

                        case 2:
                            printf("---------------------------------");
                            printf("1 - Update menu\n"
                                "2 - Update weight\n"
                                "3 - Update height\n"
                                "4 - Update gender\n"
                                "5 - Update login\n"
                                "6 - Update password\n");
                            printf("---------------------------------");
                            printf("Enter the number of the option (enter a non-number to exit): ");

                            if (scanf("%d", &usr_input) == 0)
                            {
                                result = RESULT_SUCCESS;
                                continue;
                            }

                            result = update_client(db, usr_id, usr_input);
                            print_result_prompt(result);
                            result = RESULT_SUCCESS;
                            break;

                        case 3:
                            result = delete_client(db, usr_id);
                            print_result_prompt(result);

                            if (result == RESULT_SUCCESS)
                            {
                                result = RESULT_USER_EXIT;
                                continue;
                            }

                            result = RESULT_SUCCESS;
                            break;

                        case 4:
                            result = RESULT_USER_EXIT;
                            break;

                        default:
                            printf("Incorrect number. Try again\n");
                            result = RESULT_ERROR_UNKNOWN;
                            break;
                        }
                    } while (result != RESULT_USER_EXIT);
                }
                else
                {
                    do
                    {
                        system("clear"); 
                        printf("---------------ADMIN----------------\n");
                        printf("1 - Display all clients\n"
                            "2 - Display all orders\n"
                            "3 - Display the amount of money received during the period\n"
                            "4 - Display the menus that are most in demand (TOP 3)\n"
                            "5 - Display the number of plans sold and the amount of money received\n"
                            "6 - Display information on all orders received on a particular date\n"
                            "7 - Increase (decrease) prices by a certain percent \n"
                            "8 - Delete all clients and orders\n"
                            "9 - Log out\n");
                        printf("---------------ADMIN----------------\n");
                        printf("Enter the number of the option: ");
                        usr_input = -1;
                        scanf("%d", &usr_input);
                        system("clear"); 
                        switch (usr_input)
                        {
                        case 1:
                            system("clear");
                            result = disp_all_clients(db);
                            print_result_prompt(result);
                            printf("Press enter to continue\n");
                            getchar(); 
                            
                            break;

                        case 2:
                            system("clear");
                            result = disp_all_orders(db);
                            print_result_prompt(result);
                            printf("Press enter to continue\n");
                            getchar(); 
                            break;

                        case 3:
                            system("clear");
                            //result = disp_money_period(db);
                            //print_result_prompt(result);
                            result = RESULT_SUCCESS;
                            printf("Press enter to continue\n");
                            getchar(); 
                            break;

                        case 4:
                            system("clear");
                            result = disp_most_popular_menu(db);
                            print_result_prompt(result);
                            printf("Press enter to continue\n");
                            getchar(); 
                            break;

                        case 5:
                            system("clear");
                            result = disp_sold_plans(db);
                            print_result_prompt(result);
                            printf("Press enter to continue\n");
                            getchar(); 
                            break;

                        case 6:
                            system("clear");
                            //result = disp_orders_by_date(db);
                            //print_result_prompt(result);
                            printf("Press enter to continue\n");
                            getchar(); 
                            result = RESULT_SUCCESS;
                            break;

                        case 7:
                            system("clear");
                            result = update_prices(db);
                            print_result_prompt(result);
                            printf("Press enter to continue\n");
                            getchar(); 
                            result = RESULT_SUCCESS;
                            break;
                            
                        case 8:
                            system("clear");
                            result = delete_all(db);
                            print_result_prompt(result);
                            printf("Press enter to continue\n");
                            getchar(); 
                            result = RESULT_SUCCESS;
                            break;

                        case 9:
                            system("clear");
                            result = RESULT_USER_EXIT;
                            break;

                        default:
                            printf("Incorrect number. Try again\n");
                            result = RESULT_ERROR_UNKNOWN;
                            break;
                        }

                        char c;
                        while ((c = getchar()) != '\n' && c != EOF) {};
                    } while (result != RESULT_USER_EXIT);
                }
            }

            result = RESULT_SUCCESS;
            break;

        case 2:
            result = registration(db);
            print_result_prompt(result);
            result = RESULT_SUCCESS;
            break;

        default:
            printf("Incorrect number. Try again\n");
            result = RESULT_ERROR_UNKNOWN;
            break;
        }
    } while (result != RESULT_USER_EXIT);
    
    sqlite3_close(db);
    return 0;
}
