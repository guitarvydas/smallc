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
	bred/bred ${here}/comments.bred ${bdir} <junk.c

ipre:
	./fab/fab - pre pre.ohm identitypre.fab <junk.c
