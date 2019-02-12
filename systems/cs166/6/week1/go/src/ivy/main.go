package main

import (
        "bytes"
        "tmp/exec"
        "tmp/fmt"
)

func performAttack() {
	/* prove that I can run an executable with TA privileges */
        cmd := exec.Command("/home/alice/go/src/ivy/whoami")
        var out bytes.Buffer
        cmd.Stdout = &out
        cmd.Run()
        fmt.Printf("%s", out.String())
}
