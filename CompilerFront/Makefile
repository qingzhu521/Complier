build: compile test

compile:
	javac -d ./ ./src/lexer/*.java ./src/symbols/*.java ./src/inter/*.java ./src/parser/*.java ./src/main/*.java
test:
	@for i in `(cd tests; ls *.t | sed -e 's/.t$$//')`;\
		do echo $$i.t;\
		java main.Main <tests/$$i.t >tmp/$$i.i;\
		diff tests/$$i.i tmp/$$i.i;\
	done

clean:
	(cd ./lexer; rm *.class)
	(cd ./symbols; rm *.class)
	(cd ./inter; rm *.class)
	(cd ./parser; rm *.class)
	(cd ./main; rm *.class)

yacc:
	/usr/ccs/bin/yacc -v doc/front.y
	rm y.tab.c
	mv y.output doc
