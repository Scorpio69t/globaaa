package db

import (
        "database/sql"
        _ "github.com/mattn/go-sqlite3"
	"golang.org/x/crypto/scrypt"
	"encoding/base64"
	"math/rand"
	"time"
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
func CreateUserTable(db *sql.DB) {

	/* feel free to change any of these fields; made this quickly to test some file table
	 * functionality */
	sql_table := `
	CREATE TABLE IF NOT EXISTS users(
		Username TEXT NOT NULL PRIMARY KEY,
		Hash VARCHAR(32),
		Salt VARCHAR(8),
		Sid VARCHAR(32),
		Timeout TEXT,
		Pwd TEXT
	);
	`

	_, err := db.Exec(sql_table)
	if err != nil { panic(err) }
}

/* ADD FUTURE user.db FUNCTIONS HERE; USE PREPARED STATEMENTS */

func GetPwd(db *sql.DB, username string) internal.PWDReturn {
	sql_getpwd := `SELECT pwd FROM users WHERE username = ?`
	rows, err := db.Query(sql_getpwd, username)	
	defer rows.Close()

	if err != nil { return internal.PWDReturn{ Err: err.Error() } }

	var pwd string

	/* next() prepares row for scan() */
	for rows.Next() {
	  err = rows.Scan(&pwd)
	  if err != nil { return internal.PWDReturn{ Err: err.Error() } }	  
	}

	return internal.PWDReturn{ Path: pwd }
}

/* Generates random string for session
 *
 * Input:
 * - length
 *
 * Output:
 * - the string 
 *
 * Pre-conditions: this should only be called by the system for the account creation no authentication required
 * Post-conditions: unchanged
 */
func GetRandomString(length int) string {
	rand.Seed(time.Now().UnixNano())
	const chars = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789"
	b := make([]byte, length)
    for i := range b {
        b[i] = chars[rand.Intn(len(chars))]
    }
    return string(b)
}

/* hashes password
 *
 * Input:
 * - password and salt
 *
 * Output:
 * - the string 
 *
 * Pre-conditions: this should only be called by the system for the account verificaiton no authentication required
 * Post-conditions: unchanged
 */
func HashPassword(password, salt string) string {
	dk, _ := scrypt.Key([]byte(password), []byte(salt), 16384, 8, 1, 32)
	str := base64.StdEncoding.EncodeToString(dk)
	return str
}

/* Updates the database
 *
 * Pre-conditions: this should only be called by the system for the account creation no authentication required
 * Post-conditions: unchanged
 */
func InsertNewUser(users *sql.DB, fd *sql.DB, username, password string) {
	salt := GetRandomString(8)
	hashedPassword := HashPassword(password, salt)

	/* place user into 'users' table */
	sql_additem := `
	INSERT OR REPLACE INTO users(
		Username,
		Hash,
		Salt,
		Sid,
		Timeout,
		Pwd
	) values(?, ?, ?, ?, CURRENT_TIMESTAMP, ?)
	`

	stmt, err := users.Prepare(sql_additem)
	if err != nil {
		panic(err)
	}
	defer stmt.Close()

	_, err2 := stmt.Exec(username, hashedPassword, salt, "", "/")
	if err2 != nil {
		panic(err2)
	}

	/* add user's home directory in fd table */
	sql_addhome := `
	INSERT INTO fd(
		Checksum,
                Basename,
                Name,
                Owner,
                Directory
	) values(?, ?, ?, ?, ?)
	`

	stmt2, err3 := fd.Prepare(sql_addhome)
	if err3 != nil { panic(err3) }
	defer stmt2.Close()

	_, err4 := stmt2.Exec(nil, "/", "/", username, 1)
	if err4 != nil { panic(err4) }
}

/* Checks if the username is already exist or not
 *
 * Pre-conditions: this should only be called by the system for the account creation no authentication required
 * Post-conditions: unchanged
 */
func HasUsername(db *sql.DB, username string) bool {
	sql_read_username := `
	SELECT Username FROM users WHERE Username = ?
	`
	rows, err := db.Query(sql_read_username, username)
	if err != nil {
		panic(err)
	}
	defer rows.Close()

	result := rows.Next()
	rows.Close()

	return result 
}

/* Get the salt of the user 
 *
 * Pre-conditions: this should only be called by the system for the account verification no authentication required
 * Post-conditions: unchanged
 */
func GetSalt(db *sql.DB, username string) string {
	sql_read_salt := `
	SELECT Salt FROM users WHERE Username = ?
	`
	rows, err := db.Query(sql_read_salt, username)
	if err != nil {
		panic(err)
	}
	defer rows.Close()

	var salt string
	result := rows.Next()

	if result == false {
		return ""
	}

	err = rows.Scan(&salt)
	rows.Close()

	return salt 
}

/* Returns true/false if the username and passwrod is correct or  not 
 *
 * Pre-conditions: this should only be called by the system for the account verification no authentication required
 * Post-conditions: unchanged
 */
func Authenticate(db *sql.DB, username, password string) bool {
	sql_read_username := `
	SELECT Username FROM users WHERE Username = ? AND Hash = ?
	`
	hash := HashPassword(password, GetSalt(db, username))
	rows, err := db.Query(sql_read_username, username, hash)
	if err != nil {
		panic(err)
	}
	defer rows.Close()

	result := rows.Next()
	rows.Close()

	return result 
}

/* Deletes the user by username 
 *
 * Pre-conditions: user must be authenticated 
 * Post-conditions: deletes the user
 */
func DeleteUser(db *sql.DB, fd *sql.DB, shares *sql.DB, ncount *sql.DB, username string) {
	sql_read_username := `
	DELETE FROM users WHERE Username = ?
	`
	stmt, err := db.Prepare(sql_read_username)
	if err != nil {
		panic(err)
	}

	_, err2 := stmt.Exec(username)
	if err2 != nil {
		panic(err2)
	}
	stmt.Close()

	sql_delete := `
	SELECT * FROM fd WHERE owner = ?
	`

	/* let's find all files owned by user */
	rows, rerr := fd.Query(sql_delete, username)
	if rerr != nil { panic(rerr) }

	var patharray []string
	for rows.Next() {
	  var (
	        checksum string
                owner string
                basename string
                name string
                directory bool
	  )

	  rows.Scan(&checksum, &basename, &name, &owner, &directory)
	  _ = checksum
	  _ = owner
	  _ = directory	

	   var updated_path string
	   /* remove all files */
	   if basename == "/" {
	     updated_path = basename + name
	   } else {
	     updated_path = basename + "/" + name
	   }

	   patharray = append(patharray, updated_path)
	}

	rows.Close()

	arrlen := len(patharray)
	for i:= 0; i < arrlen; i++ {
	  Rm(fd, db, shares, ncount, patharray[i], username, false)
	}
}


/* Get the username of the user by the session 
 *
 * Pre-conditions: user must be authenticated 
 * Post-conditions: unchanged
 */
func GetUsername(db *sql.DB, session string) string {
	sql_get_username := `
	SELECT Username FROM users WHERE Sid = ? AND Timeout > datetime('now')
	`
	rows, err := db.Query(sql_get_username, session)
	if err != nil {
		panic(err)
	}
	defer rows.Close()

	var ret string
	result := rows.Next()

	if result == false {
		return ""
	}

	err = rows.Scan(&ret)
	rows.Close()

	return ret 
}

 /*
 * Pre-conditions: user must be authenticated 
 * Post-conditions: sessions updated
 */

func UpdateSession(db *sql.DB, username, session string) {
	sql_additem := `
	UPDATE users
	SET Sid = ?, Timeout = datetime('now','+1 hour')
	WHERE Username = ?;
	`

	stmt, err := db.Prepare(sql_additem)
	if err != nil {
		panic(err)
	}
	defer stmt.Close()

	_, err2 := stmt.Exec(session, username)
	if err2 != nil {
		panic(err2)
	}

	sql_reset_home := `
	UPDATE users
	SET Pwd = "/"
	WHERE Username = ?
	`

	stmt2, err3 := db.Prepare(sql_reset_home)
	if err3 != nil { panic(err3) }

	_, err4 := stmt2.Exec(username)
	if err4 != nil { panic(err4) }
}
