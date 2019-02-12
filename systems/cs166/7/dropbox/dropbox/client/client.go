package main

import (
	"fmt"
	"os"
	"bufio"
	"../internal"
	"../lib/support/client"
	"../lib/support/rpc"
	"errors"
)

/* 
 * No expected input/output
 * Run with clean state
 * Function only runs the client with the supplied CLI interface
 */
func main() {

	/* check cmd-line args */
	if len(os.Args) != 2 {
		fmt.Fprintf(os.Stderr, "Usage: %v <server>\n", os.Args[0])
		os.Exit(1)
	}

	/* connect to the server */
	server := rpc.NewServerRemote(os.Args[1])

	reader := bufio.NewReader(os.Stdin)
	fmt.Println("Please enter 1 to login, 2 to sign up, or 3 to delete an account.")
	fmt.Print("Input: ")
	choice, _ := reader.ReadString('\n')

	var ret internal.LoginReturn

	/*
	The CLI for the login/sign up/delete account. All verfications/most computatitons are 
	done server-side in order to protect the server from bad inputs. 
	*/

	if (choice == "1\n") {
		/*
		For logging in 
		*/
		fmt.Print("Username: ")
		username, _ := reader.ReadString('\n')
		username = username[:len(username)-1]

		fmt.Print("Password: ")
		password, _ := reader.ReadString('\n')
		password = password[:len(password)-1]

		err := server.Call("login", &ret, username, password)
		if err != nil {
			fmt.Fprintf(os.Stderr, "%v\n", err)
			os.Exit(1)
		} else {
			if (ret.Err != "") {
				fmt.Fprintf(os.Stderr, "%s\n", ret.Err)
				os.Exit(1)
			} else {
				fmt.Println("Login successful")
			}
		}
	} else if (choice == "2\n") {
		/*
		For signing up 
		*/
		fmt.Print("Username: ")
		username, _ := reader.ReadString('\n')
		username = username[:len(username)-1]

		fmt.Print("Password: ")
		password, _ := reader.ReadString('\n')
		password = password[:len(password)-1]

		var ret string
		err := server.Call("createAccount", &ret, username, password)
		if err != nil {
			fmt.Fprintf(os.Stderr, "%v\n", err)
			os.Exit(1)
		} else {
			if (ret != "") {
				fmt.Fprintf(os.Stderr, "%s\n", ret)
				os.Exit(1)
			} else {
				fmt.Println("Account created")
			}
		}
		os.Exit(0)
	} else if (choice == "3\n") {
		/*
		For deleting an account 
		*/
		fmt.Print("Username: ")
		username, _ := reader.ReadString('\n')
		username = username[:len(username)-1]

		fmt.Print("Password: ")
		password, _ := reader.ReadString('\n')
		password = password[:len(password)-1]

		var ret string
		err := server.Call("deleteAccount", &ret, username, password)
		if err != nil {
			fmt.Fprintf(os.Stderr, "%v\n", err)
			os.Exit(1)
		} else {
			if (ret != "") {
				fmt.Fprintf(os.Stderr, "%s\n", ret)
				os.Exit(1)
			} else {
				fmt.Println("Account deleted")
			}
		}
		os.Exit(0)
	} else {
		os.Exit(0)
	}

	/* 
	 * We use the provided RunCLI for the client. 
	 */

	c := Client{server, ret.Session}
	err := client.RunCLI(&c)
	if err != nil {
		fmt.Printf(":) fatal error: %v\n", err)
		os.Exit(1)
	}
}

/* 
 * The only change here is that we have a session string with the client object.
 */
type Client struct {
	server *rpc.ServerRemote
	session string // supplied by the server
}

/* This function returns the pwd of the user.
 *
 * Input:
 * nothing
 *
 * Output:
 * - path string: the pwd of the requested user (identified by the session from 
 *                the Client object)
 * - err error: error if occured (when making remote function call)
 *
 * Pre-conditions: the user is authenticated (have session in the client object)
 * Post-conditions: unchanged
 */
func (c *Client) PWD() (path string, err error) {
	var ret internal.PWDReturn
	err = c.server.Call("pwd", &ret, c.session)
	if err != nil {
		return "", client.MakeFatalError(err)
	}
	if ret.Err != "" {
		return "", fmt.Errorf(ret.Err)
	}
	return ret.Path, nil
}

/* This function returns the DirEnt of all objects in the inputted path
 *
 * Input:
 * - path: the path to be listed
 *
 * Output:
 * - entries []client.DirEnt: an array of the directory entries of the inputted path
 * - err error: error if occured (when making remote function call)
 *
 * Pre-conditions: the user is authenticated (have session in the client object)
 * Post-conditions: unchanged
 */
func (c *Client) List(path string) (entries []client.DirEnt, err error) {
	var ret internal.ListReturn
	err = c.server.Call("list", &ret, path, c.session)
	if err != nil {
		return nil, client.MakeFatalError(err)
	}
	if ret.Err != "" {
		return nil, fmt.Errorf(ret.Err)
	}
	var ents []client.DirEnt
	for _, e := range ret.Entries {
		ents = append(ents, e)
	}
	return ents, nil
}

/* This function updates the pwd accoring to the path string supplied
 *
 * Input:
 * - path: the path to be cd-ed into
 *
 * Output:
 * - err error: error if occured (when making remote function call)
 *
 * Pre-conditions: the user is authenticated (have session in the client object)
 * Post-conditions: the pwd of the current user is changed accordingly
 */
func (c *Client) CD(path string) (err error) {
	var ret string
	err = c.server.Call("cd", &ret, path, c.session)
	if err != nil {
		return client.MakeFatalError(err)
	}
	if ret != "" {
		return fmt.Errorf(ret)
	}
	return nil
}

/* This function returns the file requested by the user (or reports an error)
 *
 * Input:
 * - path: the path to file to be downloaded
 *
 * Output:
 * - body []byte: the body of the file requested
 * - err error: error if occured (when making remote function call)
 *
 * Pre-conditions: the user is authenticated (have session in the client object)
 * Post-conditions: unchanged
 */
func (c *Client) Download(path string) (body []byte, err error) {
	var ret internal.DownloadReturn
	err = c.server.Call("download", &ret, path, c.session)
	if err != nil {
		return nil, client.MakeFatalError(err)
	}
	if ret.Err != "" {
		return nil, fmt.Errorf(ret.Err)
	}
	return ret.Body, nil
}

/* This function uploads the file if the user have permission to do so
 *
 * Input:
 * - path: the local path to file to be uploaded
 * - body []byte: the body of the file to be uploaded
 *
 * Output:
 * - err error: error if occured (when making remote function call)
 *
 * Pre-conditions: the user is authenticated (have session in the client object)
 * Post-conditions: the file is uploaded if there are no errors
 */
func (c *Client) Upload(path string, body []byte) (err error) {
	var ret string
	err = c.server.Call("upload", &ret, path, body, c.session)
	if err != nil {
		return client.MakeFatalError(err)
	}
	if ret != "" {
		return fmt.Errorf(ret)
	}
	return nil
}

/* This function makes a new directory
 *
 * Input:
 * - path: the name of the new dir to be created
 *
 * Output:
 * - err error: error if occured (when making remote function call)
 *
 * Pre-conditions: the user is authenticated (have session in the client object)
 * Post-conditions: the directory  is created
 */
func (c *Client) Mkdir(path string) (err error) {
	var ret string
	err = c.server.Call("mkdir", &ret, path, c.session)
	if err != nil {
		return client.MakeFatalError(err)
	}
	if ret != "" {
		return fmt.Errorf(ret)
	}
	return nil
}

/* This function removes a directory
 *
 * Input:
 * - path: the name of the new dir to be removed
 *
 * Output:
 * - err error: error if occured (when making remote function call)
 *
 * Pre-conditions: the user is authenticated (have session in the client object)
 * Post-conditions: the directory is removed
 */
func (c *Client) Remove(path string) (err error) {
	var ret string
	err = c.server.Call("remove", &ret, path, c.session)
	if err != nil {
		return client.MakeFatalError(err)
	}
	if ret != "" {
		return fmt.Errorf(ret)
	}
	return nil
}

/* This function shares a file requested by the user
 *
 * Input:
 * - path: the name of the file
 * - username: the user to share this file with
 * - write: should that user have  write permission or not
 *
 * Output:
 * - err error: error if occured (when making remote function call)
 *
 * Pre-conditions: the user is authenticated (have session in the client object)
 * Post-conditions: the file is shared and the database is updated accordingly
 */
func (c *Client) Share(path string, username string, write bool) (err error) {
	var ret string
	err = c.server.Call("share", &ret, path, username, write, c.session)
	if err != nil {
		return client.MakeFatalError(err)
	}
	if ret != "" {
		return fmt.Errorf(ret)
	}
	return nil
}

/* This function removes a share as requested by the user
 *
 * Input:
 * - path: the name of the file
 * - username: the user to share this file with
 *
 * Output:
 * - err error: error if occured (when making remote function call)
 *
 * Pre-conditions: the user is authenticated (have session in the client object)
 * Post-conditions: the file is not shared anymore and the database is updated accordingly
 */
func (c *Client) RemoveShare(path, username string) (err error) {
	var ret string
	err = c.server.Call("rm_share", &ret, path, username, c.session)
	if err != nil {
		return client.MakeFatalError(err)
	}
	if ret != "" {
		return fmt.Errorf(ret)
	}
	return nil
}

/* This function returns an array of the users this file is shared with
 *
 * Input:
 * - path: the name of the file
 *
 * Output:
 * - shares []client.Share: the results
 * - err error: error if occured (when making remote function call)
 *
 * Pre-conditions: the user is authenticated (have session in the client object)
 * Post-conditions: unchanged
 */
func (c *Client) GetShares(path string) (shares []client.Share, err error) {
	var ret []internal.Share
	err = c.server.Call("showShares", &ret, path, c.session)
	if err != nil {
		return nil, client.MakeFatalError(err)
	}
	if ret == nil {
		return nil, errors.New("No sharees")
	}
	var ret2 []client.Share
	for _,element := range ret {
		ret2 = append(ret2, element)
	}
	return ret2, nil
}
