all: compile

fault-tree.eps: fault_tree.py
	python3 fault_tree.py

.PHONY: compile view

compile: fault-tree.eps

# Try evince, open, xdg-open, xpdf, or whatever your
# favourite PDF viewer is, to view the .eps file
