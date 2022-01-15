#------------ Defintion of Executables ------------------
LATEX	= pdflatex -halt-on-error -file-line-error 
BIBTEX	= bibtex
VIEWPDF = evince
SPELL  = ispell --lang=en_GB
PYTHON = /usr/bin/python
FIG2DEV = fig2dev
SVG2TEX = $(PYTHON) bin/svg2tex
INKSCAPE = inkscape
BIBTEXDOWNLOAD = bin/edabibtex

#------------ Defintion of Variables ------------------
PRINTER = ljlog

TARGET=$(basename $(shell egrep -l 'documentclass' *.tex) )
HEFT_SRC = $(wildcard *.tex) 
FIG_SRC = $(shell find . -name *.fig)
FIG_PDF = $(FIG_SRC:.fig=.fig.pdf)
FIG_TEX = $(FIG_SRC:.fig=.fig.tex)
SVG_SRC = $(shell find . -name *.svg)
SVG_PDF = $(SVG_SRC:.svg=.svg.pdf)
SVG_TEX = $(SVG_SRC:.svg=.svg.tex)
RERUN = "(There were undefined references|Rerun to get (cross-references|the bars) right)"
RERUNBIB = "Citation.*undefined"
export TEXINPUTS=:./texmf/
export BSTINPUTS=:./texmf/
#------------ Creation of Targets --------------------

ifneq ($(words $(TARGET)), 1)
  $(error No or more than one file includes 'documentclass')
endif

$(TARGET).pdf: $(HEFT_SRC) $(FIG_PDF) $(FIG_TEX) $(SVG_PDF) $(SVG_TEX)
	$(LATEX) $(TARGET).tex;
	egrep -c $(RERUNBIB) $(TARGET).log && (make bib;$(LATEX) $(TARGET).tex); true
	egrep $(RERUN) $(TARGET).log && ($(LATEX) $(TARGET).tex); true
	egrep $(RERUN) $(TARGET).log && ($(LATEX) $(TARGET).tex); true
	egrep -i "(Reference|Citation).*undefined" $(TARGET).log; true

%.fig.pdf: %.fig
	$(FIG2DEV) -L pdftex $< > $@
	
%.fig.tex: %.fig %.fig.pdf
	$(FIG2DEV) -L pdftex_t -p $*.fig.pdf $*.fig > $@
	

%.tl.svg %.svg.tex: %.svg
	$(SVG2TEX) -i $*.svg.pdf -t $*.tl.svg $< $*.svg.tex

%.svg.pdf: %.tl.svg
	$(INKSCAPE) -z --file=$< --export-pdf=$@


bib: 
	$(PYTHON) $(BIBTEXDOWNLOAD) $(TARGET)
	$(BIBTEX) $(TARGET); true
	touch $(TARGET).tex

print: $(TARGET).pdf
	lpr -P $(PRINTER) $(TARGET).pdf

show: $(TARGET).pdf
	$(VIEWPDF) $(TARGET).pdf &

spell: $(HEFT_SRC) $(wildcard *.bib)
	for i in ${HEFT_SRC};do	$(SPELL) $$i;	done
	for i in ${wildcard *.bib};do $(SPELL) $$i;	done

all: $(TARGET).pdf bib fig $(TARGET).pdf
final: $(TARGET).pdf bib spell fig $(TARGET).pdf show

# destroy all files that are produced automatically
clean: 
	rm -f $(TARGET).pdf *.log *.aux *.bbl *.toc *.lof *.lot \
	*.blg *.bak *.out *~ $(FIG_PDF) $(FIG_TEX) $(SVG_PDF) $(SVG_TEX)

test:
	echo $(TARGET)
	echo $(FIG_SRC)
	echo $(FIG_PDF)
	echo $(FIG_TEX)
	echo svg $(SVG_SRC)
	echo svg $(SVG_PDF)
	echo svg $(SVG_TEX)
	
