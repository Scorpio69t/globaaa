package db

import (
        "database/sql"
        _ "github.com/mattn/go-sqlite3"
	"path"
	"../internal"	
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
func CreateSharesTable(db *sql.DB) {
	sql_table := `
	CREATE TABLE IF NOT EXISTS shares(
		Checksum VARCHAR(32),
		Owner TEXT,
		Base TEXT,
		Name TEXT,
		User TEXT,
		Write BOOLEAN
	);
	`

	_, err := db.Exec(sql_table)
	if err != nil { panic(err) }
}

/* This function shares the file requested by the user and checks if the request is
 * valid or not
 *
 * Input:
 * - *sql.DB: the connection to the databases
 * - other: the parameters
 *
 * Output:
 * - the error
 *
 * Pre-conditions: this should only be called by the system for the sharing
 * Post-conditions: file shared (database updated)
 */
func Share(shares* sql.DB, fd *sql.DB, users *sql.DB, owner string, pathname string, sharee string, perm bool) string {
	pathname = ResolvePath(users, owner, pathname)
	if IsDir(fd, owner, path.Dir(pathname), path.Base(pathname)) { return "Can't share/remove directories" }

	/* let's get the file at this specific path with this owner: this should be unique
	 * in our table */
	sql_assert := `
	SELECT checksum FROM fd WHERE owner = ? AND Basename = ? AND Name = ?
	`
	
	rows, err := fd.Query(sql_assert, owner, path.Dir(pathname), path.Base(pathname))
	if err != nil { panic(err) }
	defer rows.Close()

	result := rows.Next()
	var checksum string
	if !result { 
	  return "You don't have permissions to share this file" 
	} else {
	  rerr := rows.Scan(&checksum)
	  if rerr != nil { panic(rerr) }
	}

	/* let's make sure the user exists before we share with him */
	sql_user := `
	SELECT * FROM users WHERE Username = ?
	`
	urows, uerr := users.Query(sql_user, sharee)
	if uerr != nil { panic(uerr) }
	defer urows.Close()

	uresult := urows.Next()
	if !uresult { return "That user doesn't exist" }

	/* if everything passed, let's share with the sharee */
	sql_cleanup := `
	DELETE FROM shares WHERE 
	Checksum = ?
	AND Owner = ?
	AND Base = ?
	AND Name = ?
	AND User = ?
	`

        dstmt, derr := shares.Prepare(sql_cleanup)
	if derr != nil { panic(derr) }
	defer dstmt.Close()

	_, derr1 := dstmt.Exec(checksum, owner, path.Dir(pathname), path.Base(pathname), sharee)
        if derr1 != nil { panic(derr1) }

	sql_share := `
	INSERT OR REPLACE INTO shares(
		Checksum,
		Owner,
		Base,
		Name, 
		User,
		Write
	) values(?, ?, ?, ?, ?, ?)
	`

	stmt, err1 := shares.Prepare(sql_share)
	if err1 != nil { panic(err1) }
	defer stmt.Close()

	_, err2 := stmt.Exec(checksum, owner, path.Dir(pathname), path.Base(pathname), sharee, perm)
	if err2 != nil { panic(err2) }

	return ""	
}

/* This function removes shares of the file requested by the user and checks if the request is
 * valid or not -- only owners of files can share and remove shares
 *
 * Input:
 * - *sql.DB: the connection to the databases
 * - other: the parameters
 *
 * Output:
 * - the error
 *
 * Pre-conditions: this should only be called by the system for sharing
 * Post-conditions: file share removed (database updated)
 */
func RmShare(shares *sql.DB, fd *sql.DB, users *sql.DB, owner string, pathname string, username string) string {
	pathname = ResolvePath(users, owner, pathname)
	if IsDir(fd, owner, path.Dir(pathname), path.Base(pathname)) { return "Can't share/remove directories" }

	/* let's make sure the caller is the owner of the file */
	sql_assert := `
	SELECT checksum FROM fd WHERE owner = ? AND Basename = ? AND Name = ?
	`
	
	rows, err := fd.Query(sql_assert, owner, path.Dir(pathname), path.Base(pathname))
	if err != nil { panic(err) }
	defer rows.Close()

	result := rows.Next()
	var checksum string
	if !result { 
	  return "Unable to remove share" 
	} else {
	  rerr := rows.Scan(&checksum)
	  if rerr != nil { panic(rerr) }
	}

	sql_remove := `
	DELETE FROM shares WHERE 
	Checksum = ?
	AND Owner = ?
	AND Base = ?
	AND Name = ?
	AND User = ?
	`

	stmt, err1 := shares.Prepare(sql_remove)
	if err1 != nil { panic(err1) }
	defer stmt.Close()

	_, err2 := stmt.Exec(checksum, owner, path.Dir(pathname), path.Base(pathname), username)
	if err2 != nil { panic(err2) }

	return ""	
}

/* This function returns the users whom this file is shared with
 *
 * Input:
 * - *sql.DB: the connection to the databases
 * - other: the parameters
 *
 * Output:
 * - the error
 *
 * Pre-conditions: this should only be called by the system for the sharing
 * Post-conditions: unchanged
 */
func ShowShares(shares* sql.DB, fd *sql.DB, users *sql.DB, pathname string, username string) []internal.Share {
	new_path := ResolvePath(users, username, pathname)

	sql_perm := `
	SELECT User, Write FROM shares WHERE Owner = ? AND Name = ? AND Base = ?
	`
	base := path.Dir(new_path)
	name := path.Base(new_path)

	println("[SHOW_SHARES-]", username, name, base)
	rows, err := shares.Query(sql_perm, username, name, base)
	if err != nil { panic(err) }

	var output []internal.Share
	var u, w string

	for rows.Next() {
		err2 := rows.Scan(&u, &w)
		if err2 != nil { panic(err2) }
		if (w == "false") {
			output = append(output, internal.Share{Sharee_: u, WritePerm_: false})
		} else {
			output = append(output, internal.Share{Sharee_: u, WritePerm_: true})
		}
	}

	rows.Close()

	return output
}
