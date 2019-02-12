// Author: jliebowf
// Date: Spring 2015

package main

import (
	"../internal"
	"../lib/support/rpc"
	"../db"
	"crypto/sha256"
	"fmt"
	"io/ioutil"
	"os"
	"path"
	"regexp"
	"strings"
	"encoding/base64"
	"unicode/utf8"
)

func main() {
	/* PROVIDED CODE */
	var reset bool
	var baseDir, listenAddr string

	/* handle cmd-line arguments */
	switch {
	case len(os.Args) == 2 && os.Args[1] == "--reset":
		reset = true
        	reset_err := os.RemoveAll("uploads")
		if reset_err != nil { 
		  println("problem cleaning up uploads folder") 
		  os.Exit(1)
		}
		reset_err = os.Remove("../db/user.db")
		if reset_err != nil { 
		  println("problem cleaning up user db") 
		  os.Exit(1)
		}
		reset_err = os.Remove("../db/fd.db")
		if reset_err != nil { 
		  println("problem cleaning up file db")
		  os.Exit(1)
		}
		reset_err = os.Remove("../db/shares.db")
		if reset_err != nil { 
		  println("problem cleaning up sharing info") 
		  os.Exit(1)
		}
		println("successfully cleaned up") 
		os.Exit(1)
	case len(os.Args) == 3 && (len(os.Args[1]) == 0 || os.Args[1][0] != '-'):
		baseDir = os.Args[1]
		listenAddr = os.Args[2]
	default:
		fmt.Fprintf(os.Stderr, "Usage: %v [--reset | <base-dir> <listen-address>]\n", os.Args[0])
		os.Exit(1)
	}

	/* These are here to suppress "variable not used" errors.
 	 * In your implementation, you must actually use them! */
	_ = reset
	_ = baseDir

	/* we're going to need a file table, so let's create it */
	fd := db.InitDB("../db/fd.db")
	defer fd.Close()
        db.CreateFileTable(fd)

	/* let's also create the user table */
	users := db.InitDB("../db/user.db")
	defer users.Close()
	db.CreateUserTable(users)

	/* let's keep track of references to files alah file systems */
	ncount := db.InitDB("../db/ncount.db")
	defer ncount.Close()
	db.CreateNcountTable(ncount)

	/* and finally, for good measure, the share table */
	shares := db.InitDB("../db/shares.db")
	defer shares.Close()
	db.CreateSharesTable(shares)
   
	/* lastly make our '/uploads' directory; it's unclear right now if the 
	 * currently granted permissions effectively block other users from 
	 * executing into the directory / reading files from the directory. 
	 * 'MkdirAll() should only create the directory if it doesn't already exist. */
        err := os.MkdirAll("uploads", 0700)
        if err != nil {
                fmt.Fprintf(os.Stderr, "problem setting up server storage: %v\n", err)
		os.Exit(1)
        }

	/* register rpc handlers that'll handle cmds to server */
	rpc.RegisterHandler("upload", uploadHandler)
	rpc.RegisterHandler("download", downloadHandler)
	rpc.RegisterHandler("list", listHandler)
	rpc.RegisterHandler("mkdir", mkdirHandler)
	rpc.RegisterHandler("remove", removeHandler)
	rpc.RegisterHandler("pwd", pwdHandler)
	rpc.RegisterHandler("cd", cdHandler)
	rpc.RegisterHandler("share", shareHandler)
	rpc.RegisterHandler("rm_share", rmshareHandler)
	rpc.RegisterHandler("showShares", showSharesHandler)

	rpc.RegisterHandler("login", login)
	rpc.RegisterHandler("createAccount", createAccount)
	rpc.RegisterHandler("deleteAccount", deleteAccount)

	/* run the server */
	rpc.RegisterFinalizer(finalizer)
	err = rpc.RunServer(listenAddr)
	if err != nil {
		fmt.Fprintf(os.Stderr, "could not run server: %v\n", err)
		os.Exit(1)
	}
}

/* This should simply query the user's database and return the PWD
 * field within that database. This assumes that an authentication token, 
 * i.e. session ID, is passed from the client. All SQL commands are handled
 * through prepared statments */
func pwdHandler(session string) internal.PWDReturn {
	/* connect to database created by server */
	users := db.InitDB("../db/user.db")
	defer users.Close()

	/* authenticate user via their session ID */
	username := db.GetUsername(users, session) 	
	if username == "" {
	  return internal.PWDReturn{ Err: "Problem Authenticating" }
	}

	/* this handles querying the database using a prepared statement */
	return db.GetPwd(users, username)
}

/* All files are held in the fd table. Query the table for all files and directories
 * whose basename is equal to the path provided, making sure to handle the case where
 * path is "" */
func listHandler(pathname string, session string) internal.ListReturn {
	/* connect to user database */
	users := db.InitDB("../db/user.db")
	defer users.Close()

	username := db.GetUsername(users, session) 	
	if username == "" {
	  return internal.ListReturn{ Err: "Problem Authenticating" }
	}

	/* connect to file database */
	fd := db.InitDB("../db/fd.db")
	defer fd.Close()

	/* connect to shared file database */
	shares := db.InitDB("../db/shares.db")
	defer shares.Close()

	/* let's clean the path to get its simplest version */
	clean_path := path.Clean(pathname)

	list_return := db.List(fd, users, shares, clean_path, username)
	if list_return.Err != "" {
	  return list_return
	}

	var entries []internal.DirEnt

	for _, fi := range list_return.Files {
	  entries = append(entries, internal.DirEnt{
                        IsDir_: false,
                        Name_:  fi,
                })
        }

	for _, di := range list_return.Dirs {
	  entries = append(entries, internal.DirEnt{
                        IsDir_: true,
                        Name_:  di,
                })
        }

	/* let's manually add '.' and '..' */
	entries = append(entries, internal.DirEnt{
			IsDir_: true,
			Name_: ".",
		})

	entries = append(entries, internal.DirEnt{
			IsDir_: true,
			Name_: "..",
		})

	return internal.ListReturn{Entries: entries}
}

 /* This checks if the user is the owner of the file in the SQL query -
  * as well as if the pathname is a directory, and then updates the entries
  * in the user's database to reflect a change in the abstract file tree */
func cdHandler(pathname string, session string) string {
	users := db.InitDB("../db/user.db")
	defer users.Close()

	username := db.GetUsername(users, session) 
	if username == "" {
	  return "Problem Authenticating"
	}

	fd := db.InitDB("../db/fd.db")
	defer fd.Close()
	
	clean_path := path.Clean(pathname)
	err := db.Cd(users, fd, clean_path, username)
	return err
}

/* You want to download the file from path. In order to do this: check to 
 * see if the user is the owner of the file, find the file in the /uploads
 * folder, then download the file to the user's client. Sharing is handled 
 * below */
func downloadHandler(pathname string, session string) internal.DownloadReturn {
	users := db.InitDB("../db/user.db")
	defer users.Close()

	username := db.GetUsername(users, session) 
	if username == "" {
	  return internal.DownloadReturn{ Err: "Problem Authenticating" }
	}

	fd := db.InitDB("../db/fd.db")
	defer fd.Close()

	clean_path := path.Clean(pathname)
	shared := isShared(clean_path)

	/* this means we've found the correct appended name for a shared file */
	if len(shared) > 1  { 
	  clean_path = shared[1]

	  shares := db.InitDB("../db/shares.db")
	  defer shares.Close()

	  /* let's make sure the file is still shared with the user requesting it */
	  sql_check := `
	  SELECT * FROM shares WHERE owner = ? AND base = ? AND name = ? AND user = ?
	  `

	  rows, err := shares.Query(sql_check, shared[0], path.Dir(clean_path), path.Base(clean_path), username)
          if err != nil { panic(err) }
          defer rows.Close()

          result := rows.Next()
	  if !result { return internal.DownloadReturn{ Err: "Invalid path" } } 

	  /* if the file is shared with the user, change username to that of the
	   * owner of the file -- this'll allow us to use the same database call */
	  username = shared[0]
	}

	/* let's try to download the file */
	download_return := db.Download(fd, users, clean_path, username)

	if download_return.Err != "" { return download_return }

	/* all files should be stored in the uploads directory via their checksums */
	upload_path := "uploads/" + download_return.Checksum
	body, rerr := ioutil.ReadFile(upload_path)

	if rerr != nil {
		return internal.DownloadReturn{Err: rerr.Error() }
	}

	return internal.DownloadReturn{Body: body, Err: ""}
}

/* I'm trying to write out body of file to path. In order to do this, I
 * need to get a checksum for the file I'm uploading so I can place it in
 * the fd table. A big concern here, too, is that I need to check the path --
 * I shouldn't let a user upload a file to a directory that doesn't exist. Therefore,
 * find the basename of the path, make sure that it's a directory and the user
 * has access, then upload the file. */
func uploadHandler(pathname string, body []byte, session string) string {
	users := db.InitDB("../db/user.db")
	defer users.Close()

	username := db.GetUsername(users, session) 
	if username == "" {
	  return "Problem Authenticating"
	}

	fd := db.InitDB("../db/fd.db")
	defer fd.Close()

	ncount := db.InitDB("../db/ncount.db")
	defer ncount.Close()
	
	flag := false
	clean_path := path.Clean(pathname)
	shared := isShared(clean_path)

	/* this means we've found the correct appended name for a shared file */
	if len(shared) > 1  { 
	  clean_path = shared[1]

	  shares := db.InitDB("../db/shares.db")
	  defer shares.Close()

	  /* let's see if we have write privs */
	  sql_privs := `
	  SELECT write FROM shares WHERE owner = ? AND base = ? AND name = ? AND user = ?
	  `

	  rows, err := shares.Query(sql_privs, shared[0], path.Dir(clean_path), path.Base(clean_path), username)
          if err != nil { panic(err) }
          defer rows.Close()

          rows.Next()
	  var bool_result bool
	  rows.Scan(&bool_result)

	  if !bool_result { return "Insufficient permissions" } 

	  /* if the file we're uploading is in the shared directory, update owner in fd and
	   * all shares */
          sql_shares := `
          SELECT checksum FROM shares WHERE owner = ? AND base = ? AND name = ? AND user = ?
          `
	  
	  rows, err = shares.Query(sql_shares, shared[0], path.Dir(clean_path), path.Base(clean_path), username)
          if err != nil { panic(err) }
          defer rows.Close()

	  if rows.Next() {
	    var check string 
	    err = rows.Scan(&check)
	    if err != nil { panic(err) }

	    /* let's find the checksum of the file we're trying to upload */
	    sum := sha256.Sum256(body)
	    checksum := base64.StdEncoding.EncodeToString(sum[:len(sum)])

	    /* if we're here, it means we found a shared file that we're trying to change; 
	     * update file in the shared db */
	    sql_shares_update := `
	    UPDATE shares SET checksum = ? WHERE checksum = ? AND owner = ?
	    `
	    rows, err = shares.Query(sql_shares_update, checksum, check, shared[0])
            if err != nil { panic(err) }
            defer rows.Close()

	    /* the call the db.Upload will handle the fd */
	    username = shared[0]

	    flag = true
	  }
	}

	/* let's find the checksum of the file we're trying to upload */
	sum := sha256.Sum256(body)
	checksum := base64.StdEncoding.EncodeToString(sum[:len(sum)])


	err := db.Upload(fd, users, ncount, clean_path, checksum, username, flag)	
	if err != "" { return err }

	/* hide file from other users */
	upload_path := "uploads/" + checksum
	werr := ioutil.WriteFile(upload_path, body, 0700)
	if werr != nil { return werr.Error() }
	return ""
}

/* If the user provides a path, create the directory along the path
 * if it doesn't exist. If the user only provides the name of a directory
 * create the directory in the user's pwd. If the directory currently exists,
 * return an error. */
func mkdirHandler(pathname string, session string) string {
	users := db.InitDB("../db/user.db")
	defer users.Close()

	username := db.GetUsername(users, session) 
	if username == "" {
	  return "Problem Authenticating"
	}

	fd := db.InitDB("../db/fd.db")
	defer fd.Close()

	clean_path := path.Clean(pathname)

	err := db.Mkdir(fd, users, clean_path, username) 
	return err
}

/* This will remove both files and directories. If you're removing
 * a directory, this will first check to make sure the directory is
 * empty */
func removeHandler(pathname string, session string) string {
	users := db.InitDB("../db/user.db")
	defer users.Close()

	username := db.GetUsername(users, session) 
	if username == "" {
	  return "Problem Authenticating"
	}

	fd := db.InitDB("../db/fd.db")
	defer fd.Close()

	clean_path := path.Clean(pathname)

	shares := db.InitDB("../db/shares.db")
	defer shares.Close()

	ncount := db.InitDB("../db/ncount.db")
	defer ncount.Close()

	shared := isShared(clean_path)
	if len(shared) > 1  { 
	  clean_path = shared[1]

	  err := db.Rm(fd, users, shares, ncount, clean_path, username, true)
	  return err
	}

	err := db.Rm(fd, users, shares, ncount, clean_path, username, false)
	return err
}

/* Share the file specified by pathname, whose owner you get through authentication,
 * with username */
func shareHandler(pathname string, username string, write bool, session string) string {
	users := db.InitDB("../db/user.db")
	defer users.Close()

	owner := db.GetUsername(users, session) 
	if owner == "" {
	  return "Problem Authenticating"
	}

	fd := db.InitDB("../db/fd.db")
	defer fd.Close()

	shares := db.InitDB("../db/shares.db")
	defer shares.Close()

	clean_path := path.Clean(pathname)

	err := db.Share(shares, fd, users, owner, clean_path, username, write)
	return err
}

/* Share the file specified by pathname, whose owner you get through authentication,
 * with username */
func rmshareHandler(pathname string, username string, session string) string {
	users := db.InitDB("../db/user.db")
	defer users.Close()

	owner := db.GetUsername(users, session) 
	if owner == "" {
	  return "Problem Authenticating"
	}

	fd := db.InitDB("../db/fd.db")
	defer fd.Close()

	shares := db.InitDB("../db/shares.db")
	defer shares.Close()

	clean_path := path.Clean(pathname)

	err := db.RmShare(shares, fd, users, owner, clean_path, username)
	return err
}

/* PROVIDED CODE: TODO */
func finalizer() {
	fmt.Println("Shutting down...")
}

var isAlphanumeric = regexp.MustCompile(`^[a-zA-Z0-9]+$`).MatchString

func createAccount(username, password string) string {
	users := db.InitDB("../db/user.db")
	defer users.Close()
	fd := db.InitDB("../db/fd.db")
	defer fd.Close()

	if len(username) > 16 {
		return "Cannot create account: username cannot exceed 16 characters"
	}
	if len(username) < 4 {
		return "Cannot create account: username must have more than 4 characters"
	}
	if !isAlphanumeric(username) {
		return "Cannot create account: username must be alphanumeric (a-z, A-Z, 0-9)"
	}

	if db.HasUsername(users, username) {
		return "Cannot create account: username already exist"
	}
	db.InsertNewUser(users, fd, username, password)
	return ""
}

func login(username, password string) internal.LoginReturn {
	users := db.InitDB("../db/user.db")
	defer users.Close()

	if db.Authenticate(users, username, password) {
		sid := db.GetRandomString(32)
		db.UpdateSession(users, username, sid)
		fmt.Print("Username (from sid): ")
		fmt.Println(db.GetUsername(users, sid))
		return internal.LoginReturn{Err: "", Session: sid}
	}
	return internal.LoginReturn{Err: "Cannot login: username and password combination is not valid"}
}

func deleteAccount(username, password string) string {
	users := db.InitDB("../db/user.db")
	defer users.Close()

	fd := db.InitDB("../db/fd.db")
	defer fd.Close()

	shares := db.InitDB("../db/shares.db")
	defer shares.Close()

	ncount := db.InitDB("../db/ncount.db")
	defer ncount.Close()

	if db.Authenticate(users, username, password) {
		db.DeleteUser(users, fd, shares, ncount, username)
		return ""
	}
	return "Cannot delete account: username and password combination is not valid"
}


func showSharesHandler(pathname, session string) []internal.Share {
	users := db.InitDB("../db/user.db")
	defer users.Close()

	username := db.GetUsername(users, session) 
	if username == "" {
	  return nil // problem authenticating
	}

	fd := db.InitDB("../db/fd.db")
	defer fd.Close()

	shares := db.InitDB("../db/shares.db")
	defer shares.Close()

	return db.ShowShares(shares, fd, users, pathname, username)
}

/* let's see if we're trying to download a shared file by matching on ':' */
func isShared(path string) []string {
        b := []byte(":")
        f := func(c rune) bool {
          r, _ := utf8.DecodeLastRune(b)
          return (c == r)
        }
        return strings.FieldsFunc(path, f)
}
