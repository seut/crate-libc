## Process this file with automake to produce Makefile.in

TESTS = check_streaming
check_PROGRAMS = check_streaming
check_streaming_SOURCES = check_streaming.c $(top_builddir)/src/streaming.h $(top_builddir)/src/debug.h
check_streaming_CFLAGS = @CHECK_CFLAGS@
check_streaming_LDADD = $(top_builddir)/src/libstreaming.la @CHECK_LIBS@

