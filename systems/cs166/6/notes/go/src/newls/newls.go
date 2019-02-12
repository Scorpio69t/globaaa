package main

import (
	"bytes"
	"fmt"
	"os/exec"
)

/* modified example from: https://golang.org/pkg/os/exec/ */
func main() {

	cmd := exec.Command("ls")
	var out bytes.Buffer
	cmd.Stdout = &out
	cmd.Run()
	fmt.Printf("%s", out.String())
}
