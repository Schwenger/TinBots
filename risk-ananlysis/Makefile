TEX     =pdflatex

all: tar

.PHONY: clean
clean:
	rm -f risk-document.log risk-document.mw risk-document.out risk-document.mw

.PHONY: clean-total
clean-total: clean
	rm -f *.eps


# ===== First step:  plant some trees =====

TREES_EPS := escortnoled.eps gowrong.eps ignorevictim.eps noescort.eps nopowerled.eps runintowall.eps seenoled.eps spuriousmovements.eps standingstill.eps systemfailure.eps uncooperative.eps victim404.eps victimlost.eps victimsilent.eps

${TREES_EPS}: fault_tree.py fault_tree_lib.py
# Thankfully, make is clever enough to call this only once.
	python3 fault_tree.py

.PHONY: compile-tree
compile-tree: ${TREES_EPS}

# Try evince, open, xdg-open, xpdf, or whatever your
# favourite PDF viewer is, to view the .eps file


# ===== Second step:  compile the document =====

risk-document.pdf: risk-document.tex
	$(TEX) -halt-on-error $<
# Second run because LastPage
	$(TEX) -halt-on-error -shell-escape $<

.PHONY: compile-document
compile-document: risk-document.pdf


# ===== Third step:  convert the trees into paper =====

TREES_PDF := $(addprefix tree_,${TREES_EPS:.eps=.pdf})

${TREES_PDF}: tree_%.pdf: %.eps
	epstopdf $< --outfile=$@

say:
	echo ${TREES_PDF}

# I was tempted to call this 'Into The Trees'
# https://www.amazon.com/Into-Trees-Zoe-Keating/dp/B0052YO4L0
.PHONY: pdf-trees
pdf-trees: ${TREES_PDF}

# ===== Final step:  Profit =====

../spec/document.pdf: ../spec/document.tex
	( cd ../spec/ && pdflatex -halt-on-error document.tex && \
	                 pdflatex -halt-on-error document.tex )

.PHONY: tar
tar: risk-document.pdf ${TREES_PDF} ../spec/document.pdf
	tar czf risk-document.tar.gz $^
