PROJS  = fft_test pcm_conv pcm_echo pcm_mse pcm_play pcm_info video_player video_echo video_win ini_test
LIBD   = ../projects
WKSD   = workspaces


all: mk_lib mk_test

mk_lib:
	( cd $(LIBD); $(MAKE) )

mk_test:
	for proj in $(PROJS);		\
	do				\
	    ( cd $$proj; $(MAKE) );	\
	done


delete:
	for proj in $(PROJS);			\
	do					\
	    ( cd $$proj; $(MAKE) clean );	\
	done


clean: delete
	( cd $(LIBD); $(MAKE) clean )
	rm -f $(WKSD)/Makefile
	rm -f $(WKSD)/*.tags
	rm -f $(WKSD)/*_wsp.mk
	rm -f $(WKSD)/*.session
	rm -rf $(WKSD)/.build-*
	rm -rf $(WKSD)/.clang


purge: clean
	( cd $(LIBD); $(MAKE) purge )
