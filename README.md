#

- - -

# labrabota5

### Team Name: Dream Team

### Project name: Healthy Nutrition

- - -

### Description:

A food delivery company is implementing an app that will offer a daily meal plan based on
body mass index.The plan is selected for 1/3/6 months (more is cheaper).
Each plan has several menu options (\~3).
Each menu option contains information about calories, protein/fat/carbohydrate content.
<br>
##### The server part has the information:

1. about the plans: (example) low-calorie, moderate, high-calorie
2. about the contents of the menu: the list of dishes, calories, etc.
3. about the clints (further in the tables)
4. about the execution of orders

##### The client has the information:

1. personal data
2. data on your order

##### It is necessary to perform:

1. create tables:

> * meal plan: id \| type \| period \| price \| menu number
> * menu: id \| breakfast\| lunch\| dinner
> * menu and meal plan are linked by id
> * customers: login \| password \| weight\|height\| gender \| meal plan id
> * orders: customer id \| login \| date \| plan id \| period \| price
> * we pull up the period and price from the plan by the plan id

2. the function of client login by login and password (you can also register, if you are not lazy)
3. Using select to issue the following information:

> * for all orders: the amount of money received for the specified period
> according to the menu that is in the greatest demand
> 
> * for all orders: information about the number of plans sold and the amount of money received (grouping by plans)

4. Provide data updates using insert, delete, update
5. Create a function to increase (decrease) the price for a certain %
6. Create a function that displays information about all accepted orders on a certain date
7. Create a function that allows you to change the meal plan and then recalculate the cost

<br>

***

### Installation:
Execute Commands:
```
git clone [git@github.com](mailto:git@github.com):fpmi-tp2023/labrabota5pr1-dreamteam.git
git pull
```
Or download ZIP file 

### Usage: 
Execute Commands:
```
make
bin/program
```

### Contributing:
The authors of the project:
* Maxim Emelyanovich - coder 
* Danila Tsatsuk - content maker
* Yanchuk Bronislav - team leader / tester
* Viktor Drobyshevsky - SQL Manager