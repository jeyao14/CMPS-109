# $Id: Makefile,v 1.24 2019-10-22 12:41:48-07 - - $
# Jeffrey Yao jeyao
# Herman Wu hwwu

MKFILE      = Makefile
DEPFILE     = ${MKFILE}.dep
NOINCL      = ci clean spotless
NEEDINCL    = ${filter ${NOINCL}, ${MAKECMDGOALS}}
GMAKE       = ${MAKE} --no-print-directory

GPPWARN    = -Wall -Wextra -Wpedantic -Wshadow -Wold-style-cast
GPPOPTS    = ${GPPWARN} -fdiagnostics-color=always
COMPILECPP = g++ -std=gnu++17 -g -O0 ${GPPOPTS}
MAKEDEPCPP = g++ -std=gnu++17 -MM ${GPPOPTS}
UTILBIN    = /afs/cats.ucsc.edu/courses/cse111-wm/bin

MODULES    = listmap xless xpair debug util main
CPPSOURCE  = ${wildcard ${MODULES:=.cpp}}
OBJECTS    = ${CPPSOURCE:.cpp=.o}
SOURCELIST = ${foreach MOD, ${MODULES}, ${MOD}.h ${MOD}.tcc ${MOD}.cpp}
ALLSOURCE  = ${wildcard ${SOURCELIST}}
EXECBIN    = keyvalue
OTHERS     = ${MKFILE} ${DEPFILE}
ALLSOURCES = ${ALLSOURCE} ${MKFILE} README PARTNER
LISTING    = Listing.ps

all : ${EXECBIN}

${EXECBIN} : ${OBJECTS}
	${COMPILECPP} -o $@ ${OBJECTS}

%.o : %.cpp
	- ${UTILBIN}/checksource $<
	- ${UTILBIN}/cpplint.py.perl $<
	${COMPILECPP} -c $<

ci : ${ALLSOURCES}
	${UTILBIN}/cid + ${ALLSOURCES}
	- ${UTILBIN}/checksource ${ALLSOURCES}

lis : ${ALLSOURCES}
	mkpspdf ${LISTING} ${ALLSOURCES}

clean :
	- rm ${OBJECTS} ${DEPFILE} core

spotless : clean
	- rm ${EXECBIN} ${LISTING} ${LISTING:.ps=.pdf}

dep : ${ALLCPPSRC}
	@ echo "# ${DEPFILE} created `LC_TIME=C date`" >${DEPFILE}
	${MAKEDEPCPP} ${CPPSOURCE} >>${DEPFILE}

${DEPFILE} :
	@ touch ${DEPFILE}
	${GMAKE} dep

again :
	${GMAKE} spotless dep ci all lis

submit : ${ALLSOURCES}
	submit cse111-wm.f19 asg3 $^

ifeq (${NEEDINCL}, )
include ${DEPFILE}
endif

