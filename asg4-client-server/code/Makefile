# $Id: Makefile,v 1.16 2019-11-06 14:10:27-08 - - $
# Jeffrey Yao jeyao
# Herman Wu hwwu

MKFILE      = Makefile
DEPFILE     = ${MKFILE}.dep
NOINCL      = ci clean spotless
NEEDINCL    = ${filter ${NOINCL}, ${MAKECMDGOALS}}
GMAKE       = ${MAKE} --no-print-directory

GPPWARN     = -Wall -Wextra -Wpedantic -Wshadow -Wold-style-cast
GPPOPTS     = ${GPPWARN} -fdiagnostics-color=always
COMPILECPP  = g++ -std=gnu++17 -g -O0 ${GPPOPTS}
MAKEDEPCPP  = g++ -std=gnu++17 -MM ${GPPOPTS}
UTILBIN     = /afs/cats.ucsc.edu/courses/cse111-wm/bin

MODULES     = logstream protocol sockets
EXECBINS    = cix cixd
ALLMODS     = ${MODULES} ${EXECBINS}
SOURCELIST  = ${foreach MOD, ${ALLMODS}, ${MOD}.h ${MOD}.tcc ${MOD}.cpp}
CPPSOURCE   = ${wildcard ${MODULES:=.cpp} ${EXECBINS:=.cpp}}
ALLSOURCE   = ${wildcard ${SOURCELIST}} ${MKFILE} README PARTNER
CPPLIBS     = ${wildcard ${MODULES:=.cpp}}
OBJLIBS     = ${CPPLIBS:.cpp=.o}
CIXOBJS     = cix.o ${OBJLIBS}
CIXDOBJS    = cixd.o ${OBJLIBS}
CLEANOBJS   = ${OBJLIBS} ${CIXOBJS} ${CIXDOBJS}
LISTING     = Listing.ps

all: ${DEPFILE} ${EXECBINS}

cix: ${CIXOBJS}
	${COMPILECPP} -o $@ ${CIXOBJS}

cixd: ${CIXDOBJS}
	${COMPILECPP} -o $@ ${CIXDOBJS}

%.o: %.cpp
	- ${UTILBIN}/checksource $<
	- ${UTILBIN}/cpplint.py.perl $<
	${COMPILECPP} -c $<

ci: ${ALLSOURCE}
	${UTILBIN}/cid + ${ALLSOURCE}
	- ${UTILBIN}/checksource ${ALLSOURCE}

lis: all ${ALLSOURCE} ${DEPFILE}
	- pkill -g 0 gv || true
	${UTILBIN}/mkpspdf ${LISTING} ${ALLSOURCE} ${DEPFILE}

clean:
	- rm ${LISTING} ${LISTING:.ps=.pdf} ${CLEANOBJS} core

spotless: clean
	- rm ${EXECBINS} ${DEPFILE}


dep: ${ALLCPPSRC}
	@ echo "# ${DEPFILE} created `LC_TIME=C date`" >${DEPFILE}
	${MAKEDEPCPP} ${CPPSOURCE} >>${DEPFILE}

${DEPFILE}:
	@ touch ${DEPFILE}
	${GMAKE} dep

again: ${ALLSOURCE}
	${GMAKE} spotless dep ci all lis

submit : ${ALLSOURCE}
	submit cse111-wm.f19 asg4 $^

ifeq (${NEEDINCL}, )
include ${DEPFILE}
endif

