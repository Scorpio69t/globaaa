package db

import (
        "database/sql"
        _ "github.com/mattn/go-sqlite3"
)

/* This function creates a new database
 *
 * Input:
 * - db *sql.DB: the connection to the database
 *
 * Output:
 * nothing
 *
 * Pre-conditions: this should only be called by the system when it starts
 * Post-conditions: database created
 */
func CreateNcountTable(db *sql.DB) {

	/* feel free to change any of these fields; made this quickly to test some file table
	 * functionality */
	sql_table := `
	CREATE TABLE IF NOT EXISTS ncount(
		Hash VARCHAR(32),
		N INTEGER
	);
	`
	_, err := db.Exec(sql_table)
	if err != nil { panic(err) }
}

/* This function returns the number active copies of the supplied checksum
 *
 * Input:
 * - *sql.DB: the connection to the databases
 * - hash string: a hash of a file
 *
 * Output:
 * - the number of user of that hash
 *
 * Pre-conditions: this should only be called by the system for the deduplication
 * Post-conditions: unchanged
 */
func GetNcount(db *sql.DB, hash string) int {
	sql_get_n_count := `
	SELECT N FROM ncount WHERE Hash = ?
	`
	rows, err := db.Query(sql_get_n_count, hash)
	if err != nil {
		panic(err)
	}
	defer rows.Close()

	var ncount int
	result := rows.Next()

	if result == false {
		return 0
	}

	err = rows.Scan(&ncount)
	rows.Close()

	return ncount
}

/* This function increases the number active coppies of the supplied checksum by 1
 *
 * Input:
 * - *sql.DB: the connection to the databases
 * - hash string: a hash of a file
 *
 * Output:
 * - the number of user of that hash
 *
 * Pre-conditions: this should only be called by the system for the deduplication
 * Post-conditions: unchanged
 */
func IncreaseNcount(db *sql.DB, hash string) int {

	sql_get_n_count := `
	SELECT N FROM ncount WHERE Hash = ?
	`
	rows, err := db.Query(sql_get_n_count, hash)
	if err != nil {
		panic(err)
	}
	var ncount int
	result := rows.Next()
	err = rows.Scan(&ncount)

	rows.Close()

	if result == false {
		/* No hash in the table, INSERT one */
		sql_additem := `
		INSERT OR REPLACE INTO ncount(
			Hash,
			N
		) values(?, 1)
		`

		stmt, err := db.Prepare(sql_additem)
		if err != nil {
			panic(err)
		}

		_, err2 := stmt.Exec(hash)
		if err2 != nil {
			panic(err2)
		}
		stmt.Close()
	} else {
		sql_updateitem := `
		UPDATE ncount
		SET N = ?
		WHERE Hash = ?
		`

		stmt, err := db.Prepare(sql_updateitem)
		if err != nil {
			panic(err)
		}

		_, err2 := stmt.Exec(ncount + 1, hash)
		if err2 != nil {
			panic(err2)
		}
		stmt.Close()
	}
	return ncount + 1
}

/* This function reduces the number active coppies of the supplied checksum by 1
 *
 * Input:
 * - *sql.DB: the connection to the databases
 * - hash string: a hash of a file
 *
 * Output:
 * - the number of user of that hash
 *
 * Pre-conditions: this should only be called by the system for the deduplication
 * Post-conditions: unchanged
 */
func DecreaseNcount(db *sql.DB, hash string) int {

	sql_get_n_count := `
	SELECT N FROM ncount WHERE Hash = ?
	`
	rows, err := db.Query(sql_get_n_count, hash)
	if err != nil {
		panic(err)
	}
	var ncount int
	result := rows.Next()
	err = rows.Scan(&ncount)
	rows.Close()
	if result == false {
		/* do nothing */
	} else {
		sql_updateitem := `
		UPDATE ncount
		SET N = ?
		WHERE Hash = ?
		`

		stmt, err := db.Prepare(sql_updateitem)
		if err != nil {
			panic(err)
		}

		if ncount - 1 < 0 {
			ncount = 1
		}
		_, err2 := stmt.Exec(ncount - 1, hash)
		if err2 != nil {
			panic(err2)
		}
		stmt.Close()
	}
	return ncount - 1
}
