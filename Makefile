here=`pwd`
bdir=${here}/bred

all:

install: repos npmstuff

repos:
	multigit -r

npmstuff:
	npm install ohm-js yargs atob pako
	npm install cli
	npm install js-beautify

smallc:
	./fab/fab - pre pre.ohm pre.fab <sample.c >sample.c0

ipre:
	./fab/fab - pre pre.ohm identitypre.fab <sample.c >sample.c0
