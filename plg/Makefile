SUBDIRS = $(shell find -mindepth 1 -maxdepth 1 -type d)

all clean: $(SUBDIRS)

$(SUBDIRS):
	$(MAKE) -C $@ $(MAKECMDGOALS)

.PHONY: all clean $(SUBDIRS)

