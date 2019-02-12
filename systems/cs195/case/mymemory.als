sig Addr{}
sig Data{}
sig Memory {
	data: Addr -> Data
}

fact Canonicalize {
	no disj m, m' : Memory | m.data = m'.data
}

pred init (m : Memory) {
	no m.data
}

pred write(m, m' : Memory, a : Addr, d : Data) {
	m'.data = m.data ++ a -> d
}

fun read(m : Memory, a : Addr): Data {
	m.data[a]
}

assert WriteRead {
	all m, m' : Memory, a : Addr, d : Data |
		write[m, m', a, d] implies (read[m', a] = d)
}
check WriteRead
