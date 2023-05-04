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
    char c;

    system("cls");
    printf("---------------------------------\n");
    printf("%*s\n", 20, "Welcome!");

    do
    {
        printf("---------------------------------\n");
        printf("1 - Login\n"
            "2 - Register\n");
        printf("---------------------------------\n");
        printf("Enter the number of the option (enter a non-number to exit): ");

        if (scanf("%d", &usr_input) == 0)
        {
            result = RESULT_USER_EXIT;
            continue;
        }

        system("cls");
        getchar();

        switch (usr_input)
        {
            int usr_id;

        case 1:
            result = authorization(db, &usr_id);
            system("cls");

            if (result == RESULT_SUCCESS)
            {
                if (usr_id != 0)
                {
                    do
                    {
                        printf("---------------------------------\n");
                        result = disp_client(db, usr_id);

                        if (result == RESULT_ERROR_UNKNOWN)
                        {
                            break;
                        }

                        printf("---------------------------------\n");
                        printf("1 - Display today's menu\n"
                            "2 - Update...\n"
                            "3 - Delete account\n"
                            "4 - Log out\n");
                        printf("---------------------------------\n");
                        printf("Enter the number of the option: ");
                        usr_input = -1;
                        scanf("%d", &usr_input);
                        system("cls");
                        while ((c = getchar()) != '\n' && c != EOF) {};

                        switch (usr_input)
                        {
                        case 1:
                            result = disp_client_menu(db, usr_id);
                            printf("Press enter to continue\n");
                            getchar();
                            system("cls");
                            print_result_prompt(result);
                            break;

                        case 2:
                            printf("---------------------------------\n");
                            printf("1 - Plan\n"
                                "2 - Menu\n"
                                "3 - Weight\n"
                                "4 - Height\n"
                                "5 - Gender\n"
                                "6 - Login\n"
                                "7 - Password\n");
                            printf("---------------------------------\n");
                            printf("Enter the number of the option (enter a non-number to exit): ");

                            if (scanf("%d", &usr_input) == 0)
                            {
                                system("cls");
                                while ((c = getchar()) != '\n' && c != EOF) {};
                                result = RESULT_SUCCESS;
                                continue;
                            }

                            system("cls");
                            while ((c = getchar()) != '\n' && c != EOF) {};
                            result = update_client(db, usr_id, usr_input);
                            system("cls");
                            print_result_prompt(result);
                            result = RESULT_SUCCESS;
                            break;

                        case 3:
                            result = delete_client(db, usr_id);
                            system("cls");

                            if (result == RESULT_SUCCESS)
                            {
                                result = RESULT_USER_EXIT;
                                continue;
                            }

                            print_result_prompt(result);
                            result = RESULT_SUCCESS;
                            break;

                        case 4:
                            result = RESULT_USER_EXIT;
                            system("cls");
                            break;

                        default:
                            system("cls");
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
                        printf("---------------ADMIN----------------\n");
                        printf("1 - Display all...\n"
                            "2 - Display the amount of money received during the period\n"
                            "3 - Display the menus that are most in demand (TOP 3)\n"
                            "4 - Display the number of plans sold and the amount of money received\n"
                            "5 - Display information on all orders received on a particular date\n"
                            "6 - Increase (decrease) prices by a certain percent \n"
                            "7 - Delete all clients and orders\n"
                            "8 - Log out\n");
                        printf("---------------ADMIN----------------\n");
                        printf("Enter the number of the option: ");
                        usr_input = -1;
                        scanf("%d", &usr_input);
                        system("cls");
                        while ((c = getchar()) != '\n' && c != EOF) {};

                        switch (usr_input)
                        {
                        case 1:
                            printf("---------------------------------\n");
                            printf("1 - Clients\n"
                                "2 - Orders\n"
                                "3 - Plans\n"
                                "4 - Menus\n");
                            printf("---------------------------------\n");
                            printf("Enter the number of the option (enter a non-number to exit): ");

                            if (scanf("%d", &usr_input) == 0)
                            {
                                system("cls");
                                while ((c = getchar()) != '\n' && c != EOF) {};
                                result = RESULT_SUCCESS;
                                continue;
                            }

                            system("cls");
                            while ((c = getchar()) != '\n' && c != EOF) {};
                            result = display_all(db, usr_input);
                            printf("Press enter to continue\n");
                            getchar();
                            system("cls");
                            print_result_prompt(result);
                            break;

                        case 2:
                            result = disp_money_period(db);
                            if (result != RESULT_USER_EXIT)
                            {
                                printf("Press enter to continue\n");
                                getchar();
                            }
                            system("cls");
                            print_result_prompt(result);
                            break;

                        case 3:
                            result = disp_most_popular_menu(db);
                            printf("Press enter to continue\n");
                            getchar();
                            system("cls");
                            print_result_prompt(result);
                            break;

                        case 4:
                            result = disp_sold_plans(db);
                            printf("Press enter to continue\n");
                            getchar(); 
                            system("cls");
                            print_result_prompt(result);
                            break;

                        case 5:
                            result = disp_orders_by_date(db);
                            if (result != RESULT_USER_EXIT)
                            {
                                printf("Press enter to continue\n");
                                getchar();
                            }
                            system("cls");
                            print_result_prompt(result);
                            result = RESULT_SUCCESS;
                            break;

                        case 6:
                            result = update_prices(db);
                            system("cls");
                            print_result_prompt(result);
                            result = RESULT_SUCCESS;
                            break;
                            
                        case 7:
                            result = delete_all(db);
                            system("cls");
                            print_result_prompt(result);
                            result = RESULT_SUCCESS;
                            break;

                        case 8:
                            result = RESULT_USER_EXIT;
                            system("cls");
                            break;

                        default:
                            system("cls");
                            printf("Incorrect number. Try again\n");
                            result = RESULT_ERROR_UNKNOWN;                       
                            break;
                        }

                        //while ((c = getchar()) != '\n' && c != EOF) {};
                    } while (result != RESULT_USER_EXIT);
                }
            }

            result = RESULT_SUCCESS;
            break;

        case 2:
            result = registration(db);
            system("cls");
            print_result_prompt(result);
            result = RESULT_SUCCESS;
            break;

        default:
            printf("Incorrect number. Try again\n");
            result = RESULT_ERROR_UNKNOWN;
            getchar(); 
            system("cls");
            break;
        }
    } while (result != RESULT_USER_EXIT);
    
    sqlite3_close(db);
    return 0;
}
