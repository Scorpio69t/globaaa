/*
 * code adapted from: https://siongui.github.io/2016/01/09/go-sqlite-example-basic-usage/
 */

package db

import (
	"../internal"
        "database/sql"
        _ "github.com/mattn/go-sqlite3"
	"os"
	"path"
	"regexp"
	"strings"
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
func CreateFileTable(db *sql.DB) {
	/* create table if it doesn't exist */
	sql_table := `
	CREATE TABLE IF NOT EXISTS fd(
		Checksum VARCHAR(32),
		Basename TEXT,
		Name TEXT,
		Owner TEXT,
		Directory BOOLEAN
	);
	`

	_, err := db.Exec(sql_table)
	if err != nil { panic(err) }
}

/* This function cleans the filepath supplied and queries the database to
 * list the files and directories in the pathname. We need to do this because
 * we don't have the real file tree system, but everthing is implemented in the
 * database.
 *
 * Input:
 * - *sql.DB: the connection to the databases
 * - pathname string: the pathname to be listed
 * - username string: the user who owns the directory
 *
 * Output:
 * - the list of objects
 *
 * Pre-conditions: this should only be called by the system when it already verified the user
 * Post-conditions: unchanged
 */
func List(fd *sql.DB, users *sql.DB, shares *sql.DB, pathname string, username string) internal.ListReturn {
	/* case where the user doesn't provide a path to 'ls' */
	if pathname == "." {
	  pwd_return := GetPwd(users, username)
	  if pwd_return.Err != "" { return internal.ListReturn { Err: pwd_return.Err } }	  
	  pathname = pwd_return.Path
	}

	/* find all files that are at the base specified by clean path */
	sql_file_list := `
	SELECT Name FROM fd WHERE Basename = ? AND Owner = ? AND Directory = ?
	`
	frows, ferr := fd.Query(sql_file_list, pathname, username, 0)
	if ferr != nil { return internal.ListReturn{ Err: ferr.Error() } }

	/* find all directories that are at the base specified by clean path */
	sql_dir_list := `
	SELECT Name FROM fd WHERE Basename = ? AND Owner = ? AND Directory = ?
	`
	drows, derr := fd.Query(sql_dir_list, pathname, username, 1)
	if derr != nil { return internal.ListReturn{ Err: derr.Error() } }

	var err error
	var files []string

	for frows.Next() {
	  var entry string
	  /* assuming the ';' here delimits unit statements like in Ocaml */
	  if err = frows.Scan(&entry); err != nil {
	    return internal.ListReturn{ Err: err.Error() }
	  }

	  files = append(files, entry)
	}

	if err != nil { return internal.ListReturn { Err: err.Error() } }

	var dirs []string
	for drows.Next() {
	  var entry string
	  /* assuming the ';' here delimits unit statements like in Ocaml */
	  if err = drows.Scan(&entry); err != nil {
	    return internal.ListReturn { Err: err.Error() }
	  }

	  dirs = append(dirs, entry)
	}

	if err != nil { return internal.ListReturn { Err: err.Error() } }

	/* let's now handle shared files -- let's only show them in a user's root directory */
	cwd := GetPwd(users, username)

	if cwd.Err != "" { return internal.ListReturn { Err: cwd.Err } }
	if cwd.Path != "/" { return internal.ListReturn { Files: files, Dirs: dirs } }

	/* let's query the shared table, and find all the files the user can access */
	sql_get_shared := `
	SELECT * FROM shares WHERE User = ?
	`

	srows, serr := shares.Query(sql_get_shared, username)
	if serr != nil { panic(serr) }
	defer srows.Close()


	for srows.Next() {
	  var (
	  	checksum string
	  	owner string
	  	base string
	  	name string
 		user string
		write bool
	  )
	  
	  if serr = srows.Scan(&checksum, &owner, &base, &name, &user, &write); serr != nil {
	    	return internal.ListReturn { Err: err.Error() }
	  }
	
	  /* quelch unused variable warnings */
	  _ = owner
	  _ = user
	  _ = write

	  /* format output for shared user */
	  if base == "/" {
	    files = append(files, (owner + ":" + base + name))
	   } else {
	    files = append(files, (owner + ":" + base + "/" + name))
	   }
	}

	return internal.ListReturn { Files: files, Dirs: dirs }
}

/* This function changes the pwd of the user who requests this. It also handles
 * special cases of cd such as . and ... It also checks if the requested 
 * path is valid (i.e. if the path is a directory) We need to do this because
 * we don't have the real file tree system, but everthing is implemented in the
 * database.
 *
 * Input:
 * - *sql.DB: the connection to the databases
 * - pathname string: the pathname to be cd-ed into
 * - username string: the user who owns the directory
 *
 * Output:
 * - the list of objects
 *
 * Pre-conditions: this should only be called by the system after its already verified the user
 * Post-conditions: pwd of the user changed in the database
 */
func Cd(users *sql.DB, fd *sql.DB, pathname string, username string) string {
	/* let's special case "." and ".." */
	if pathname == "." { return "" }

	if pathname == ".." {
	  /* let's get current pwd */
	  ret := GetPwd(users, username)
	  if ret.Err != "" { return ret.Err }

	  /* change into pwd's parent directory */
	  sql_change := `UPDATE users SET Pwd = ? WHERE Username = ?`
	  stmt, err := users.Query(sql_change, path.Dir(ret.Path), username)        
	  if err != nil { return err.Error() }

	  /* it sometimes seems I need these statements to get the SQL 
	   * statment to run */
	  stmt.Next()
          defer stmt.Close()

          return ""
	}

	/* let's simplify path s.t. it always resolves from home dir */
	new_path := ResolvePath(users, username, pathname)
	new_path = path.Clean(new_path)

	/* check to make sure we have access */
	perm := Access(fd, username, path.Dir(new_path), path.Base(new_path))
	if !perm { return "Invalid Path" }

	perm = IsDir(fd, username, path.Dir(new_path), path.Base(new_path))
	if !perm { return "Path not a directory" }

	/* if no error, user can change into directory */
	sql_change := `UPDATE users SET Pwd = ? WHERE Username = ?`
	
	stmt, err := users.Query(sql_change, new_path, username)
	if err != nil { panic(err) }

	stmt.Next()
	defer stmt.Close()

	return ""
}

/* This function verifies if the user have the permission to download that file
 * or not. If so, it returns the checksum for the info supplied.
 *
 * Input:
 * - *sql.DB: the connection to the databases
 * - pathname string: the pathname to be downloaed
 * - username string: the user who owns the file
 *
 * Output:
 * - the struct containing the checksum of the file requested
 *
 * Pre-conditions: this should only be called by the system when it already verified the user
 * Post-conditions: unchanged
 */
func Download(fd *sql.DB, users *sql.DB, pathname string, username string) internal.DownloadReturn {
	new_path := ResolvePath(users, username, pathname)

	sql_perm := `
	SELECT Checksum FROM fd WHERE Owner = ? AND Name = ? AND Basename = ?
	`
	base := path.Dir(new_path)
	name := path.Base(new_path)

	rows, err := fd.Query(sql_perm, username, name, base)
	if err != nil { panic(err) }

	var checksum string
	result := rows.Next()

	if result == false { return internal.DownloadReturn{ Err: "Invalid path"} }

	rows.Scan(&checksum)
	rows.Close()

	return internal.DownloadReturn{ Checksum: checksum }
}

/* This function updates the database for the uploaded file.
 *
 * Input:
 * - *sql.DB: the connection to the databases
 * - pathname string: the pathname to be uploaded
 * - check string: checksum of the uploaed file
 * - username string: the user who owns the file
 *
 * Output:
 * - the status
 *
 * Pre-conditions: this should only be called by the system when it already verified the user
 * Post-conditions: the database of the filesystem updated
 */
func Upload(fd *sql.DB, users *sql.DB, ncount *sql.DB, pathname string, check string, username string, flag bool) string {
	new_path := ResolvePath(users, username, pathname)
	name := path.Base(new_path)
	base_name := path.Base(path.Dir(new_path))
	base_dir := path.Dir(path.Dir(new_path))

	if !IsDir(fd, username, base_dir, base_name) { return "Invalid Path" }

	/* flag, I believe, is true if we're uploading to a shared file */
	if !flag {
	  /* let's make sure the file doesn't already exist */
	  if name == "." || name == ".." { return "Invalid path" }

	  sql_check := `
	  SELECT * FROM fd WHERE Basename = ? AND Name = ? AND Owner = ?
	  `

	  rows, err := fd.Query(sql_check, path.Dir(new_path), name, username)
          if err != nil { panic(err) }

          result := rows.Next()
          if result { return "Invalid path" }
          rows.Close()

	  /* user has permission to access directory */
	  sql_add_file := `
	  INSERT OR REPLACE INTO fd(
                  Checksum, 
                  Basename,
                  Name,
                  Owner,
                  Directory
  	  ) values (?, ?, ?, ?, ?)
	  `

	  stmt2, err3 := fd.Prepare(sql_add_file)
	  if err3 != nil { panic(err3) }
	  defer stmt2.Close()

	  _, err4 := stmt2.Exec(check, path.Dir(new_path), name, username, 0)
	  if err4 != nil { panic(err4) }

	  /* increase the number of references to the file */
	  IncreaseNcount(ncount, check)

	  return ""
	} else {

	  sql_update_file := `
	  UPDATE fd SET Checksum = ? WHERE
	  Basename = ?
	  AND Name = ?
	  AND Owner = ?
	  AND Directory = ?
	  `

	  stmt2, err3 := fd.Prepare(sql_update_file)
	  if err3 != nil { panic(err3) }
	  defer stmt2.Close()

	  _, err4 := stmt2.Exec(check, path.Dir(new_path), name, username, 0)
	  if err4 != nil { panic(err4) }

	  return ""

	}
}

/* This function checks if the user input the correct dir name to be created or
 * not  and then updates the database to have the dir created.
 *
 * Input:
 * - *sql.DB: the connection to the databases
 * - pathname string: the pathname to be created
 * - username string: the user who owns the dir
 *
 * Output:
 * - the status
 *
 * Pre-conditions: this should only be called by the system when it already verified the user
 * Post-conditions: the database of the filesystem updated
 */
func Mkdir(fd *sql.DB, users *sql.DB, pathname string, username string) string {
	/*regardless of pathname provided, let's resolve the path to one
	 * from the home directory in order to simplify things */
	pwd := ResolvePath(users, username, pathname)

	/* handle multiple ".." at beginning of path that wouldn't
	 * be caught by Clean() */
	pwd = Consume(pwd)

	/* make sure name of directory is valid */
	name := path.Base(pwd)
	if !isAlphanumeric(name) { return "Invalid Name" }
	if strings.EqualFold(name, "shared") { return "Protected name" }

	pwd_name := path.Base(path.Dir(pwd))
	pwd_base := path.Dir(path.Dir(pwd))

	/* check user has access to directory in which they're creating dir */
	perm := Access(fd, username, pwd_base, pwd_name)
	if !perm { return "Invalid Path" }

	perm = IsDir(fd, username, pwd_base, pwd_name)
	if !perm { return "Path not a directory" }

	/* check if file already exists */
	perm = Access(fd, username, path.Dir(pwd), name)
	if perm { return "File already exists" }

	/* insert directory into table */
	sql_mkdir := `
	INSERT INTO fd(
		Checksum,
		Basename,
		Name,
		Owner,
		Directory
	) values (?, ?, ?, ?, ?)
	`

	stmt, qerr := fd.Prepare(sql_mkdir)
	if qerr != nil { panic(qerr) }
	defer stmt.Close()

	_, eerr := stmt.Exec(nil, path.Dir(pwd), name, username, true)
	if eerr != nil { panic(eerr) }
		
	return ""
}

/* This function checks if the user input the correct dir name to be removed or
 * not (as well as the permissions) and then updates the database to have the dir removed.
 *
 * Input:
 * - *sql.DB: the connection to the databases
 * - pathname string: the pathname to be removed
 * - username string: the user who owns the dir
 *
 * Output:
 * - the status
 *
 * Pre-conditions: this should only be called by the system when it already verified the user
 * Post-conditions: the database of the filesystem updated
 */
func Rm(fd *sql.DB, users *sql.DB, shares *sql.DB, ncount *sql.DB, pathname string, username string, flag bool) string {
	pwd := ResolvePath(users, username, pathname)

	/* handle multiple ".." at beginning of path that wouldn't
	 * be caught by Clean() */
	pwd = Consume(pwd)
	name := path.Base(pwd)

	/* if the user is trying to delete a shared file; shared files
	 * can't be directories */
	if flag {
	  sql_rm := `
	  DELETE FROM shares WHERE Base = ? AND Name = ? AND User = ?
	  `

	  stmt, err := shares.Prepare(sql_rm)
	  if err != nil { panic(err) }
	  defer stmt.Close()

	  _, err = stmt.Exec(path.Dir(pwd), name, username)
	  if err != nil { panic(err) }
	} else {
	  /* check if file already exists */
	  perm := Access(fd, username, path.Dir(pwd), name)
	  if !perm { return "Invalid Path" }

	  /* if it's a directory, check that it's empty */
	  perm = IsDir(fd, username, path.Dir(pwd), name)
	  if perm { if IsEmpty(fd, pwd, username) { return "Directory not empty" } }


	  /* let's get the checksum so we can remove it from /uploads */
          sql_check := `
          SELECT Checksum FROM fd WHERE Basename = ? AND Name = ? and Owner = ?
          `
	  rows, err := fd.Query(sql_check, path.Dir(pwd), name, username)
	  if err != nil { panic(err) }

	  var check string
	  if rows.Next() { rows.Scan(&check) }
	  rows.Close()

	  /* let's see if the file we're deleting has only a single reference;
	   * we can do this, since if we're here we're certain the user trying to
	   * remove the file is an owner */
	  count := GetNcount(ncount, check)

	  if count == 1 { 
	    DecreaseNcount(ncount, check)
	    os.Remove("../bin/uploads/" + check) 
	  } else {
	    DecreaseNcount(ncount, check)
	  }

	  /* if the owner is deleting the file, first remove it from the file db,
	   * then remove all shares */
	  sql_rm := `
	  DELETE FROM fd WHERE Basename = ? AND Name = ? AND Owner = ?
	  `

	  stmt, serr := fd.Prepare(sql_rm)
	  if serr != nil { panic(serr) }

	  _, err = stmt.Exec(path.Dir(pwd), name, username)
	  if err != nil { panic(err) }
	  stmt.Close()

	  sql_rm_shares := `
	  DELETE FROM shares WHERE Base = ? AND Name = ? AND Owner = ?
	  `

	  stmt, serr = shares.Prepare(sql_rm_shares)
	  if serr != nil { panic(serr) }
	  defer stmt.Close()

	  _, err = stmt.Exec(path.Dir(pwd), name, username)
	  if err != nil { panic(err) }
	}
	
	return ""
}

/* check a user's access to a certain file */
func Access(fd *sql.DB, username string, base string, name string) bool {
	sql_check := `
	SELECT Name FROM fd WHERE Owner = ? AND Basename = ? AND Name = ? 
	`

	rows, err := fd.Query(sql_check, username, base, name)
	if err != nil { panic(err) }
	defer rows.Close()

	result := rows.Next()
	rows.Close()

	return result
}

/* clean path further, eliminating any ".." */
func Consume(pathname string) string {
	pwd := path.Clean(pathname)
	count := strings.Count(pwd, "..")

	for i := 0; i < count; i++ {
	  pwd = path.Dir(pwd)
	}

	return pwd
}

/* we want to unify the path to make functions like mkdir, cd, and rmdir easier */
func ResolvePath(users *sql.DB, username string, pathname string) string {
	var pwd string
	if string(pathname[0]) != "/" {
	  ret := GetPwd(users, username)
	  if ret.Err != "" {
	    return ret.Err
	  } else {
	    if ret.Path != "/" {
	      pwd = ret.Path + "/" + pathname
	    } else {
	      pwd = ret.Path + pathname
	    }
	  }
	} else {
	  pwd = pathname
	}

	return pwd
}

/* return whether the path is a directory or not */
func IsDir(fd *sql.DB, username string, base string, name string) bool {
	sql_check := `
	SELECT Directory FROM fd WHERE Owner = ? AND Basename = ? AND Name = ? 
	`

	rows, err := fd.Query(sql_check, username, base, name)
	if err != nil { panic(err) }
	defer rows.Close()

	dir_bool := false
	if rows.Next() { rows.Scan(&dir_bool) }

	return dir_bool
}

/* check if a directory is empty */
func IsEmpty(fd *sql.DB, base string, username string) bool {
        sql_empty := `
        SELECT Name FROM fd WHERE Basename = ? AND Owner = ?
        `

        rows, err := fd.Query(sql_empty, base, username)
        if err != nil { panic(err) }
        defer rows.Close()

        result := rows.Next()
	rows.Close()

	return result
}

var isAlphanumeric = regexp.MustCompile(`^[a-zA-Z0-9]+$`).MatchString
