SUBDIRS := \
	utility/conv2gas \
	utility/structer \
	modules/ft2 \
	engine \
	drivers \
	$(empty)

all clean install::
	for i in $(SUBDIRS); do $(MAKE) -C $$i $@ || exit 1; done

GCC_9=/usr/bin/m68k-atari-mint-gcc-9
GCC_8=/usr/bin/m68k-atari-mint-gcc-8
GCC_7=/usr/bin/m68k-atari-mint-gcc-7
GCC_4=/usr/bin/m68k-atari-mint-gcc

sizecheck:
	@for ft2 in "" freetype-2.2.1 freetype-2.5.2 freetype-2.8 freetype-2.10.1; do \
		echo $$ft2; \
		rm -f modules/ft2/freetype; \
		test "$$ft2" != "" && ln -s $(HOME)/gtk/$$ft2 modules/ft2/freetype; \
		for cc in $(GCC_4) $(GCC_7) $(GCC_8) $(GCC_9); do \
			$(MAKE) clean >/dev/null;  \
			$(MAKE) -j8 CC="$$cc" >/dev/null; \
			$(MAKE) -j8 CC="$$cc" CPU=v4e >/dev/null; \
			echo $$cc; \
			ls -l engine/*.prg; \
			if test "$$ft2" == ""; then ls -l drivers/16_bit/16_bit.sys drivers/aranym/aranym.sys drivers/bitplane/bitplane.sys drivers/firebee/firebee.sys drivers/saga/saga.sys drivers/uaegfx/uaegfx.sys; fi; \
		done; \
	done
	@rm -f modules/ft2/freetype
	@ln -s $(HOME)/gtk/freetype-2.2.1 modules/ft2/freetype
