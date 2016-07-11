TEX     =pdflatex
VIEWER  =evince

SOURCE  =verification.tex
OBJECT  =$(patsubst %.tex,%.pdf,$(SOURCE))
LOGFILE =$(patsubst %.tex,%.log,$(SOURCE))
AUXFILE =$(patsubst %.tex,%.aux,$(SOURCE))


all: view

$(OBJECT): %.pdf: %.tex $(SOURCE)
	$(TEX) -halt-on-error $<
# Second run because LastPage
	$(TEX) -halt-on-error -shell-escape $<

.PHONY: clean clean-total compile old say spellcheck-de spellcheck-en touch view zip

clean:
	rm -f $(LOGFILE) $(AUXFILE) document.out document.mw

clean-total: clean
	rm -f $(OBJECT)

compile: ${OBJECT}

# For debugging the Makefile
say: super
	@echo "Source:\t$(SOURCE)"
	@echo ".log file:\t$(LOGFILE)"
	@echo ".aux file:\t$(AUXFILE)"
	@echo "Object:\t$(OBJECT)"

spellcheck-de:
	for i in $(SOURCE); do aspell --lang=de check $$i; done

spellcheck-en:
	for i in $(SOURCE); do aspell --lang=en_US check $$i; done

view: ${OBJECT}
	@command -v ${VIEWER} > /dev/null || { \
		echo "Bad $$VIEWER: ${VIEWER}" ; \
		echo "If you change this, PLEASE DON'T COMMIT SUCH A CHANGE!" ; \
		exit 1 ; \
	}
	$(VIEWER) $(OBJECT) &
