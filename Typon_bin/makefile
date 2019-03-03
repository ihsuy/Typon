
PREFIX=/usr/local


.PHONY: install uninstall
  
install: 
	install -d $(DESTDIR)$(PREFIX)/bin
	exec ./typon -new
	install typon_run.sh $(DESTDIR)$(PREFIX)/bin/typon
	rm -f typon_run.sh
uninstall:
	rm -f $(DESTDIR)$(PREFIX)/bin/typon
