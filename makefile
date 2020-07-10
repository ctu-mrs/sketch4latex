#
# sketch: 3d line drawings from a simple scene description language
#
# (c) Gene Ressler
#
# makefile v0.03
#
CC = gcc -O1 -g -Wall
#CC = gcc -g -Wall
YACC = bison --defines --debug --report=all --yacc
LEX = flex

verdata=$(shell cat version.dat)
VER_MINOR=$(word 3,$(verdata))
VER_MAJOR=$(word 6,$(verdata))
VER_BUILD=$(word 9,$(verdata))
VER=$(VER_MAJOR).$(VER_MINOR).$(VER_BUILD)

ifeq ($(OS),Windows_NT)
EXE=sketch.exe
MAKEDOCS=cmd /c "make examples"
else
EXE=sketch
MAKEDOCS=perl make.pl examples
endif

DISTDIR=sketch-$(VER)
TARFILE=$(DISTDIR).tar
TARZIP=$(DISTDIR).tgz
ZIP=$(DISTDIR).zip
EXEZIP=sketchexe-$(VER).zip
UPLOAD=Upload

OBJECTS=bsp.o cmdline.o emit.o error.o expr.o geometry.o geomio.o global.o\
	langver.o lex.yy.o main.o memutil.o opts.o scene.o symbol.o y.tab.o

SOURCES=$(OBJECTS:%.o=%.c) sketch.l sketch.y 

HEADERS=bsp.h cmdline.h dynarray.h emit.h error.h expr.h geometry.h geomio.h\
	global.h langver.h main.h memutil.h opts.h parse.h resource.h scene.h\
	symbol.h version.h y.tab.h

$(EXE) : $(OBJECTS)
	$(CC) -o $(EXE) $(OBJECTS) -lm

y.tab.c y.tab.h : sketch.y
	$(YACC) $<

lex.yy.c : sketch.l
	$(LEX) $<

version.h : version.dat FORCE
	perl makever.pl
FORCE:

.PHONY : dist
dist : clean
	-rm $(TARZIP)
	-rm -rf $(DISTDIR)
	mkdir $(DISTDIR)
	perl distadmin.pl
	cp readme COPYING.txt makefile updates.htm makever.pl $(DISTDIR)
	echo dist=1 | cat version.dat - > $(DISTDIR)/version.dat
	cp $(SOURCES) $(DISTDIR)
	touch $(DISTDIR)/lex.yy.c $(DISTDIR)/y.tab.c
	cp $(HEADERS) $(DISTDIR)
	mkdir $(DISTDIR)/Data
	-cp Data/bunny.sk Data/buggy.sk Data/test.sk \
		Data/2tori.sk Data/hand.sk Data/cone.sk $(DISTDIR)/Data
	cp -r Doc $(DISTDIR)
	tar cfv $(TARFILE) $(DISTDIR)
	gzip $(TARFILE)
	mv -f $(TARFILE).gz $(TARZIP)
	-rm -f $(ZIP)
	zip -r $(ZIP) $(DISTDIR)
	-rm -f $(EXEZIP)
	cl -DYY_NEVER_INTERACTIVE=1 -Za -Ox -Fe$(EXE) *.c
	zip $(EXEZIP) $(EXE)
	-rm -rf $(UPLOAD)
	mkdir $(UPLOAD)
	mv $(EXEZIP) $(UPLOAD)
	mv $(ZIP) $(UPLOAD)
	mv $(TARZIP) $(UPLOAD)
	cp Doc/sketch.pdf $(UPLOAD)
	cp Doc/sketch.html $(UPLOAD)
	cp index.htm updates.htm $(UPLOAD)

.PHONY : upload
upload :
	ftp -s:upload.ftp -d ftp.frontiernet.net

.PHONY : clean
clean :
	-rm -f $(EXE) $(OBJECTS) *.obj core *~ *.stackdump y.output
	-rm -rf Debug Release
	-cd Doc; rm -f \
                sketch.ps sketch.dvi \
		sketch.aux sketch.cp sketch.cps sketch.fn sketch.ky \
		sketch.log sketch.op sketch.ops sketch.pg \
		sketch.sx sketch.sxs sketch.tmp sketch.toc sketch.tp \
		sketch.vr texput.log ex*\.pdf ex*\.eps ex*\.tex ex*\.txt \
		\#*\# *~
	-cd Data; rm -f *~ \#*\#

MANUAL=sketch
MANUALTEXI=$(MANUAL).texi

.PHONY : realclean
realclean: clean
	cd Doc;\
	rm -f *~ $(MANUAL).dvi $(MANUAL).pdf $(MANUAL).ps $(MANUAL).html;\
	cd $(MANUAL); rm -f *; cd ..;\
	cd Data; rm -f *~; cd ..

.PHONY : docs
docs :  clean $(EXE)
	cd Doc;\
	$(MAKEDOCS)

bsp.o: bsp.c bsp.h geometry.h dynarray.h error.h geomio.h
cmdline.o: cmdline.c cmdline.h langver.h error.h memutil.h symbol.h \
  scene.h opts.h dynarray.h geometry.h expr.h bsp.h global.h emit.h \
  parse.h version.h
emit.o: emit.c emit.h scene.h opts.h dynarray.h error.h geometry.h expr.h \
  bsp.h global.h langver.h memutil.h version.h
error.o: error.c error.h
expr.o: expr.c expr.h geometry.h dynarray.h error.h
geometry.o: geometry.c geometry.h dynarray.h error.h memutil.h
geomio.o: geomio.c geomio.h geometry.h dynarray.h error.h
global.o: global.c global.h opts.h dynarray.h error.h geometry.h \
  langver.h memutil.h
langver.o: langver.c langver.h error.h
lex.yy.o: lex.yy.c geometry.h dynarray.h error.h symbol.h scene.h opts.h \
  expr.h bsp.h global.h langver.h memutil.h parse.h y.tab.h
main.o: main.c main.h cmdline.h langver.h error.h memutil.h symbol.h \
  scene.h opts.h dynarray.h geometry.h expr.h bsp.h global.h parse.h \
  emit.h
memutil.o: memutil.c memutil.h
opts.o: opts.c opts.h dynarray.h error.h geometry.h
scene.o: scene.c scene.h opts.h dynarray.h error.h geometry.h expr.h \
  bsp.h global.h langver.h memutil.h emit.h
symbol.o: symbol.c error.h geometry.h dynarray.h symbol.h scene.h opts.h \
  expr.h bsp.h global.h langver.h memutil.h
y.tab.o: y.tab.c parse.h scene.h opts.h dynarray.h error.h geometry.h \
  expr.h bsp.h global.h langver.h memutil.h symbol.h
