all:
	pdflatex felgr_david.tex
	pdflatex felgr_david.tex

start: all
	evince felgr_david.pdf

purge:
	-rm -f *.log
	-rm -f *.aux
	-rm -f *.toc
	-rm -f *.bbl
	-rm -f *.blg
	-rm -f *.bcf
	-rm -f *.idx
	-rm -f *.ilg
	-rm -f *.ind
	-rm -f *.out
	-rm -f *.run.xml

clean: purge
	-rm -f felgr_david.pdf
