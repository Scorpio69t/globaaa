package main

import (
	"encoding/binary"
	"math"
)

const rounds = 64

func Encrypt(k uint32, m [8]byte) (c [8]byte) {
	// Only keep the least significant 24 bits
	k &= (1 << 24) - 1

	v0, v1 := binary.BigEndian.Uint32(m[:]), binary.BigEndian.Uint32(m[4:])
	const delta = 0x9E3779B9
	sum := uint32(0)
	key := [4]uint32{k, k, k, k}
	for i := 0; i < rounds; i++ {
		v0 += (((v1 << 4) ^ (v1 >> 5)) + v1) ^ (sum + key[sum&3])
		sum += delta
		v1 += (((v0 << 4) ^ (v0 >> 5)) + v0) ^ (sum + key[(sum>>11)&3])
	}
	binary.BigEndian.PutUint32(c[:], v0)
	binary.BigEndian.PutUint32(c[4:], v1)
	return
}

func Decrypt(k uint32, c [8]byte) (m [8]byte) {
	// Only keep the least significant 24 bits
	k &= (1 << 24) - 1

	v0, v1 := binary.BigEndian.Uint32(c[:]), binary.BigEndian.Uint32(c[4:])
	const delta = 0x9E3779B9
	sum := uint32((delta * rounds) & math.MaxUint32)
	key := [4]uint32{k, k, k, k}
	for i := 0; i < rounds; i++ {
		v1 -= (((v0 << 4) ^ (v0 >> 5)) + v0) ^ (sum + key[(sum>>11)&3])
		sum -= delta
		v0 -= (((v1 << 4) ^ (v1 >> 5)) + v1) ^ (sum + key[sum&3])
	}
	binary.BigEndian.PutUint32(m[:], v0)
	binary.BigEndian.PutUint32(m[4:], v1)
	return
}

func DoubleEncrypt(k1, k2 uint32, m [8]byte) (c [8]byte) {
	return Encrypt(k2, Encrypt(k1, m))
}

func DoubleDecrypt(k1, k2 uint32, c [8]byte) (m [8]byte) {
	return Decrypt(k1, Decrypt(k2, c))
}
