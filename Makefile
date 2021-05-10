latex/scanner.pdf:
	./scanner && cd latex && pdflatex scanner.tex && mv ./scanner.pdf ../result.pdf && cd .. && evince result.pdf