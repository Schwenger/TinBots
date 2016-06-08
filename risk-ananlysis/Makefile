all: compile view

fault-tree.eps: fault-tree.dot
	dot -Tps -ofault-tree.eps fault-tree.dot

.PHONY: compile view

compile: fault-tree.eps

view: compile
	evince fault-tree.eps &
