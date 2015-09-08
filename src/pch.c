   Copyright (C) 1990, 1991, 1992, 1993, 1997, 1998, 1999, 2000, 2001,
   2002, 2003, 2006, 2009 Free Software Foundation, Inc.
   This program is free software; you can redistribute it and/or modify
   the Free Software Foundation; either version 2, or (at your option)
   any later version.
   along with this program; see the file COPYING.
   If not, write to the Free Software Foundation,
   51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.  */
#include <backupfile.h>
static time_t p_timestamp[2];		/* timestamps in patch headers */
static LINENUM p_first;			/* 1st line number */
static LINENUM p_newfirst;		/* 1st line number of replacement */
static LINENUM p_ptrn_lines;		/* # lines in pattern */
static LINENUM p_repl_lines;		/* # lines in replacement text */
static LINENUM p_end = -1;		/* last line in hunk */
static LINENUM p_max;			/* max allowed value of p_end */
static LINENUM p_prefix_context;	/* # of prefix context lines */
static LINENUM p_suffix_context;	/* # of suffix context lines */
static LINENUM p_input_line;		/* current line # from patch file */
static LINENUM hunkmax = INITHUNKMAX;	/* size of above arrays */
static LINENUM p_bline;			/* line # of p_base */
static LINENUM p_sline;			/* and the line number for it */
static LINENUM p_hunk_beg;		/* line number of current hunk */
static LINENUM p_efake = -1;		/* end of faked up lines--don't free */
static LINENUM p_bfake = -1;		/* beg of faked up lines */
static char *scan_linenum (char *, LINENUM *);
static enum diff intuit_diff_type (bool);
static void next_intuit_at (file_offset, LINENUM);
static void skip_to (file_offset, LINENUM);
	file_offset stdin_pos;
	if (binary_transput)
	  {
	    if (isatty (STDIN_FILENO))
	      fatal ("cannot read binary data from tty on this platform");
	    setmode (STDIN_FILENO, O_BINARY);
	  }
	if (fstat (STDIN_FILENO, &st) != 0)
	  pfatal ("fstat");
	if (S_ISREG (st.st_mode) && (stdin_pos = file_tell (stdin)) != -1)
	  {
	    pfp = stdin;
	    file_pos = stdin_pos;
	  }
	else
	  {
	    size_t charsread;
	    int exclusive = TMPPATNAME_needs_removal ? 0 : O_EXCL;
	    TMPPATNAME_needs_removal = 1;
	    pfp = fdopen (create_file (TMPPATNAME,
				       O_RDWR | O_BINARY | exclusive,
				       (mode_t) 0, true),
			  "w+b");
	    if (!pfp)
	      pfatal ("Can't open stream for file %s", quotearg (TMPPATNAME));
	    for (st.st_size = 0;
		 (charsread = fread (buf, 1, bufsize, stdin)) != 0;
		 st.st_size += charsread)
	      if (fwrite (buf, 1, charsread, pfp) != charsread)
		write_fatal ();
	    if (ferror (stdin) || fclose (stdin) != 0)
	      read_fatal ();
	    if (fflush (pfp) != 0
		|| file_seek (pfp, (file_offset) 0, SEEK_SET) != 0)
	      write_fatal ();
	  }
      }
	pfp = fopen (filename, binary_transput ? "rb" : "r");
	if (!pfp)
	  pfatal ("Can't open patch file %s", quotearg (filename));
	if (fstat (fileno (pfp), &st) != 0)
	  pfatal ("fstat");
    next_intuit_at (file_pos, (LINENUM) 1);
maybe_reverse (char const *name, bool nonexistent, bool empty)
  bool is_empty = nonexistent || empty;
  bool r;

  r = (! is_empty) < p_says_nonexistent[reverse ^ is_empty]
      && ok_to_reverse ("The next patch%s would %s the file %s,\nwhich %s!",
			reverse ? ", when reversed," : "",
			(nonexistent ? "delete"
			 : empty ? "empty out"
			 : "create"),
			quotearg (name),
			(nonexistent ? "does not exist"
			 : empty ? "is already empty"
			 : "already exists"));
  reverse ^= r;
  return r;
there_is_another_patch (bool need_header)
    diff_type = intuit_diff_type (need_header);
	if (no_strip_trailing_cr)
	  p_strip_trailing_cr = false;
	else if (p_strip_trailing_cr)
	    if (stat (inname, &instat) == 0)
intuit_diff_type (bool need_header)
    register file_offset this_line = 0;
    register file_offset first_command_line = -1;
    LINENUM fcl_line = 0; /* Pacify `gcc -W'.  */
    register bool this_is_a_command = false;
    register bool stars_this_line = false;
    register enum diff retval;
    p_timestamp[OLD] = p_timestamp[NEW] = (time_t) -1;
	register char *s;
	register char *t;
	register file_offset previous_line = this_line;
	register bool last_line_was_command = this_is_a_command;
	register bool stars_last_line = stars_this_line;
	register size_t indent = 0;
	      continue;
		  continue;
		  continue;
	    p_name[OLD] = fetchname (s+4, strippath, &p_timestr[OLD],
				     &p_timestamp[OLD]);
	    p_name[OLD] = fetchname (s+4, strippath, &p_timestr[OLD],
				     &p_timestamp[OLD]);
	    p_name[INDEX] = fetchname (s+6, strippath, (char **) 0,
				       (time_t *) 0);
	else if (strnEQ(s, "Prereq:", 7)) {
	      continue;
		    continue;
	} else
	      continue;
		time_t timestamp = (time_t) -1;
		p_name[NEW] = fetchname (t+4, strippath, &p_timestr[NEW],
					 &timestamp);
		if (timestamp != (time_t) -1)
	    /* `p_name', `p_timestr', and `p_timestamp' are backwards;
	    time_t ti = p_timestamp[OLD];
	      p_says_nonexistent[OLD] = 1 + ! p_timestamp[OLD];
	      p_says_nonexistent[NEW] = 1 + ! p_timestamp[NEW];
	    if (! ((p_name[OLD] || ! p_timestamp[OLD])
		   && (p_name[NEW] || ! p_timestamp[NEW]))
	      p_says_nonexistent[OLD] = 1 + ! p_timestamp[OLD];
	      LINENUM saved_p_bline = p_bline;
		p_says_nonexistent[NEW] = 1 + ! p_timestamp[NEW];
	    if (! ((p_name[OLD] || ! p_timestamp[OLD])
		   && (p_name[NEW] || ! p_timestamp[NEW]))
	    p_strip_trailing_cr = strip_trailing_cr;
    /* To intuit `inname', the name of the file to patch,
	 and take the index name from the `Index:' line if present and
       - Otherwise, report failure by setting `inname' to 0;
	      else if (stat (p_name[i], &st[i]) != 0)
		  if (posixly_correct)
			      if (diffbuf)
				free (diffbuf);
	    if (i != NONE && st[i].st_size > 0)
	      i0 = i;
		&& ! maybe_reverse (p_name[i0], i == NONE,
				    i == NONE || st[i].st_size == 0))
	    inerrno = stat (inname, &instat) == 0 ? 0 : errno;
	    maybe_reverse (inname, inerrno, inerrno || instat.st_size == 0);
	      stat_result = stat (filename, &stat_buf);
next_intuit_at (file_offset file_pos, LINENUM file_line)
skip_to (file_offset file_pos, LINENUM file_line)
    register FILE *i = pfp;
    register FILE *o = stdout;
    register int c;
scan_linenum (char *s0, LINENUM *linenum)
  LINENUM n = 0;
      LINENUM new_n = 10 * n + (*s - '0');
    register char *s;
    register LINENUM context = 0;
    register size_t chars_read;
	LINENUM repl_beginning = 0;	/* index of --- line */
	register LINENUM fillcnt = 0;	/* #lines of missing ptrn or repl */
	register LINENUM fillsrc;	/* index of first line to copy */
	register LINENUM filldst;	/* index of first missing line */
	register bool repl_could_be_missing = true;
	LINENUM repl_patch_line;	/* input line number for same */
	LINENUM repl_context;		/* context for same */
	LINENUM ptrn_prefix_context = -1; /* lines in pattern prefix context */
	LINENUM ptrn_suffix_context = -1; /* lines in pattern suffix context */
	LINENUM repl_prefix_context = -1; /* lines in replac. prefix context */
	LINENUM ptrn_copiable = 0;	/* # of copiable lines in ptrn */
	LINENUM repl_copiable = 0;	/* Likewise for replacement.  */

	/* Pacify `gcc -Wall'.  */
		    fatal ("unexpected `***' at line %s: %s",
		  continue;
			/* `Old' lines were omitted.  Set up to fill
			   them in from `new' context lines.  */
		      fatal ("%s `---' at line %s; check line numbers at line %s",
		      fatal ("duplicate `---' at line %s; check line numbers at line %s",
		  continue;
	  fatal ("no `---' found in patch at line %s",
	assert (p_prefix_context != -1 && p_suffix_context != -1);
	file_offset line_beginning = file_tell (pfp);
					/* file pos of the current line */
	register LINENUM fillsrc;	/* index of old lines */
	register LINENUM filldst;	/* index of new lines */
	register int i;
	LINENUM min, max;
	      fatal ("`<' expected at line %s of patch",
	      fatal ("`---' expected at line %s of patch",
	      fatal ("`>' expected at line %s of patch",
	LINENUM i;
  register FILE *fp = pfp;
  register int c;
  register size_t i;
  register char *b;
  register size_t s;
  register FILE *fp = pfp;
  register int c;
  register file_offset line_beginning = file_tell (fp);
	continue;
    register LINENUM i;
    register LINENUM n;
    register char *s;
	if (p_line)
	  free (p_line);
	if (p_len)
	  free (p_len);
	if (p_Char)
	  free (p_Char);
    if (tp_line)
      free (tp_line);
    if (tp_len)
      free (tp_len);
    if (tp_char)
      free (tp_char);
/* Return timestamp of patch header for file WHICH (false = old, true = new),
   or -1 if there was no timestamp or an error in the timestamp.  */
time_t
pch_timestamp (bool which)
  return p_timestamp[which];
LINENUM
LINENUM
LINENUM
LINENUM
LINENUM
LINENUM
LINENUM
pch_line_len (LINENUM line)
pch_char (LINENUM line)
pfetch (LINENUM line)
pch_write_line (LINENUM line, FILE *file)
LINENUM
/* Is the newline-terminated line a valid `ed' command for patch
static char
	continue;
	    continue;
do_ed_script (FILE *ofp)
    static char const ed_program[] = ed_PROGRAM;
    register file_offset beginning_of_this_line;
    register FILE *pipefp = 0;
    register size_t chars_read;
	int exclusive = TMPOUTNAME_needs_removal ? 0 : O_EXCL;
	TMPOUTNAME_needs_removal = 1;
	copy_file (inname, TMPOUTNAME, 0, exclusive, instat.st_mode, true);
	sprintf (buf, "%s %s%s", ed_program, verbosity == VERBOSE ? "" : "- ",
		 TMPOUTNAME);
      fatal ("%s FAILED", ed_program);
	FILE *ifp = fopen (TMPOUTNAME, binary_transput ? "rb" : "r");
	  pfatal ("can't open `%s'", TMPOUTNAME);
  LINENUM old = 1;
  LINENUM new = p_ptrn_lines + 1;