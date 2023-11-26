#include <iostream>
#include "C:\Users\klauc\Documents\Projects\sqlite-amalgamation-3440000\sqlite3.h"
#include <string>

// builds a program to take in a new route name and save it to a database in sqllite
// return all the entries in the db at the end of the program

void displayAllEntries(sqlite3* db) {
    const char* query = "SELECT * FROM ROUTES;";
    sqlite3_stmt* stmt;
    int rc = sqlite3_prepare_v2(db, query, -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        std::cerr << "Error preparing the db query for return all: " << sqlite3_errmsg(db) << std::endl;
        return;
    }

    std::cout << "Existing climbing routes: \n";

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        const unsigned char* routeName = sqlite3_column_text(stmt, 1);
        const unsigned char* grade = sqlite3_column_text(stmt, 2);
        const unsigned char* comments = sqlite3_column_text(stmt, 3);

        std::cout << "Route Name: " << routeName << ", Grade: " << grade << ", Comments: " << comments << "\n";
    }

    sqlite3_finalize(stmt); // Finalize the prepared statement
}

int main() {
    sqlite3* db;
    int rc = sqlite3_open("climbing.db", &db); // Open or create the database file

    if (rc) {
        std::cerr << "Error opening SQLite database: " << sqlite3_errmsg(db) << std::endl;
        return 1;
    }

    // Ask user for new entry
    char response;

    std::cout << "Would you like to enter a new climb (Y/N)?";
    std::cin >> response;

    if (response == 'y' || response == 'Y') {

        std::string routeNameResponse;
        std::string routeGradeResponse; 
        std::string routeCommentResponse; 

        std::cout << "What is the route name? ";
        std::getline(std::cin >> std::ws, routeNameResponse);

        std::cout << "What was the grade of the route (i.e. 5.3)? ";
        std::getline(std::cin >> std::ws, routeGradeResponse);

        std::cout << "Do you have any comments on the climb?";
        std::getline(std::cin >> std::ws, routeCommentResponse);

        // SQL query to insert climbing route information
        const char* insertQuery = "INSERT INTO Routes (RouteName, Grade, Comments) VALUES (?, ?, ?)";

        sqlite3_stmt* stmt;
        rc = sqlite3_prepare_v2(db, insertQuery, -1, &stmt, nullptr);

        if (rc != SQLITE_OK) {
            std::cerr << "Error preparing SQL statement: " << sqlite3_errmsg(db) << std::endl;
            return 1;
        }

        // Bind parameters to the prepared statement
        sqlite3_bind_text(stmt, 1, routeNameResponse.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 2, routeGradeResponse.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 3, routeCommentResponse.c_str(), -1, SQLITE_STATIC);

        // Execute the SQL statement
        rc = sqlite3_step(stmt);
        if (rc != SQLITE_DONE) {
            std::cerr << "Error inserting data: " << sqlite3_errmsg(db) << std::endl;
            return 1;
        }

        sqlite3_finalize(stmt); // Finalize the prepared statement
        displayAllEntries(db);

        sqlite3_close(db); // Close the database connection
        std::cout << "Climbing route information inserted successfully!" << std::endl;
        return 0;
    }
}
