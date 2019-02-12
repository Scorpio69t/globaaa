traverse: ast.ml dfs.ml bfs.ml
	ocamlc -c ast.ml
	ocamlc -o dfs ast.cmo dfs.ml
	ocamlc -o bfs ast.cmo bfs.ml

clean:
	rm -rf dfs bfs *.cmo *.cmi _build
