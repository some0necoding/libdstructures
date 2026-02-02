SUBDIRS = src test

default: all

.DEFAULT:
	for dir in $(SUBDIRS); do $(MAKE) -C $$dir $@; done
