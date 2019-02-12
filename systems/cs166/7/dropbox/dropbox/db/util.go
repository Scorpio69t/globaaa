package db

import (
        "database/sql"
        _ "github.com/mattn/go-sqlite3"
)

/* This function initializes a new database connection
 *
 * Input:
 * - filepath string: the path to the database file
 *
 * Output:
 * - *sql.DB: the pointer to the connection
 *
 * Pre-conditions: this should only be called by the system when it starts
 * Post-conditions: database connection created
 */
func InitDB(filepath string) *sql.DB {
        db, err := sql.Open("sqlite3", filepath)
        if err != nil { panic(err) }
        if db == nil { panic("db nil") }
        return db
}
