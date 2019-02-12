package main

import (
	"bytes"
	"fmt"
	"os/exec"
)

/* modified example from: https://golang.org/pkg/os/exec/ */
func main() {
	
	/* run a program you've previously compiled */
	cmd := exec.Command("/home/dfarnha1/course/cs166/6/go/bin/hello")
	var out bytes.Buffer
	cmd.Stdout = &out
	cmd.Run()
	fmt.Printf("%s", out.String())
}
