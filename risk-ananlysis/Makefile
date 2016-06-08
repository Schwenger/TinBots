all: compile

fault-tree.eps: fault-tree.dot
	dot -Tps -ofault-tree.eps fault-tree.dot

.PHONY: compile view

compile: fault-tree.eps

# Try evince, open, xdg-open, xpdf, or whatever your
# favourite PDF viewer is, to view the .eps file
