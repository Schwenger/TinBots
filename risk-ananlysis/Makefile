TEX     =pdflatex
VIEWER  =evince

all: compile-tree compile-document

fault-tree.eps: fault_tree.py fault_tree_lib.py
	python3 fault_tree.py

.PHONY: compile-tree
compile-tree: fault-tree.eps

# Try evince, open, xdg-open, xpdf, or whatever your
# favourite PDF viewer is, to view the .eps file

risk-document.pdf: risk-document.tex
	$(TEX) -halt-on-error $<
# Second run because LastPage
	$(TEX) -halt-on-error -shell-escape $<

.PHONY: compile-document
compile-document: risk-document.pdf

.PHONY: clean
clean:
	rm -f risk-document.log risk-document.mw risk-document.out risk-document.mw

.PHONY: clean-total
clean-total: clean
	rm -f *.eps
