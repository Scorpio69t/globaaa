package main

import (
	"bufio"
	"encoding/binary"
	"encoding/hex"
	"fmt"
	"os"
	"strings"
)

type pair struct {
	plaintext  [8]byte
	ciphertext [8]byte
}

func main() {
	pairs := parsePairs()
	if len(pairs) == 0 {
		fmt.Fprintf(os.Stderr, "need at least one plaintext/ciphertext pair\n")
		os.Exit(1)
	}

	// declare max value
	max := uint32(0x1000000)

	// variables to hold keys
	k1 := uint32(0)
	k2 := uint32(0)
	
	// create a map [8]byte -> uint32
	m := make(map[[8]byte]uint32)

	fmt.Fprintf(os.Stderr, "Starting...\n")

	// given a message m, store k1(m) -> k in map
 	for i := uint32(0); i < max; i++ {
	  m[Encrypt(i, pairs[0].plaintext)] = i
	}

	/* see if there's any overlap in the intermediate cipher;
  	 * whether k2^{-1}(c) == k1(m) */
 	for i := uint32(0); i < max; i++ {
	  v := Decrypt(i, pairs[0].ciphertext)
	  _, prs := m[v]

	  if prs {
	    k1 = m[v]
	    k2 = i
	    break
	  }
	}

	printKeyPair(k1, k2)
}

func printKeyPair(k1, k2 uint32) {
	var k1bytes [4]byte
	var k2bytes [4]byte
	binary.BigEndian.PutUint32(k1bytes[:], k1)
	binary.BigEndian.PutUint32(k2bytes[:], k2)
	fmt.Printf("Recovered key pair: (%v, %v)\n", hex.EncodeToString(k1bytes[1:]), hex.EncodeToString(k2bytes[1:]))
}

// parsePairs reads lines of the form
// <plaintext> <ciphertext>
// from stdin. If it encounters an error,
// it prints the error and exits.
func parsePairs() []pair {
	var pairs []pair
	s := bufio.NewScanner(os.Stdin)
	for s.Scan() {
		parts := strings.Fields(s.Text())
		if len(parts) == 0 {
			// skip empty lines
			continue
		}
		if len(parts) != 2 {
			fmt.Println(os.Stderr, "invalid syntax: expected lines of the form <plaintext> <ciphertext>\n")
			os.Exit(1)
		}
		plainBytes, err := hex.DecodeString(parts[0])
		if err != nil {
			fmt.Fprintf(os.Stderr, "could not parse plaintext as hex: %v\n", err)
			os.Exit(1)
		}
		cipherBytes, err := hex.DecodeString(parts[1])
		if err != nil {
			fmt.Fprintf(os.Stderr, "could not parse plaintext as hex: %v\n", err)
			os.Exit(1)
		}

		if len(plainBytes) != 8 {
			fmt.Fprintf(os.Stderr, "plaintext must be 8 bytes\n", err)
			os.Exit(1)
		}
		if len(cipherBytes) != 8 {
			fmt.Fprintf(os.Stderr, "ciphertext must be 8 bytes\n", err)
		}

		var newPair pair
		copy(newPair.plaintext[:], plainBytes)
		copy(newPair.ciphertext[:], cipherBytes)
		pairs = append(pairs, newPair)
	}
	if err := s.Err(); err != nil {
		fmt.Fprintf(os.Stderr, "could not read stdin: %v\n", err)
		os.Exit(1)
	}
	return pairs
}
