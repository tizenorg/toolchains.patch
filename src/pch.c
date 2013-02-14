   Copyright (C) 1990-1993, 1997-2003, 2006, 2009-2012 Free Software
   Foundation, Inc.
   This program is free software: you can redistribute it and/or modify
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */
#if HAVE_SETMODE_DOS
# include <io.h>
#endif
static char const *invalid_names[2];
bool p_copy[2];				/* Does this patch create a copy? */
bool p_rename[2];			/* Does this patch rename a file? */
static mode_t p_mode[2];		/* file modes */
static lin p_first;			/* 1st line number */
static lin p_newfirst;			/* 1st line number of replacement */
static lin p_ptrn_lines;		/* # lines in pattern */
static lin p_repl_lines;		/* # lines in replacement text */
static lin p_end = -1;			/* last line in hunk */
static lin p_max;			/* max allowed value of p_end */
static lin p_prefix_context;		/* # of prefix context lines */
static lin p_suffix_context;		/* # of suffix context lines */
static lin p_input_line;		/* current line # from patch file */
static lin hunkmax = INITHUNKMAX;	/* size of above arrays */
static lin p_bline;			/* line # of p_base */
static lin p_sline;			/* and the line number for it */
static lin p_hunk_beg;			/* line number of current hunk */
static lin p_efake = -1;		/* end of faked up lines--don't free */
static lin p_bfake = -1;		/* beg of faked up lines */
static bool p_git_diff;			/* true if this is a git style diff */
static char *scan_linenum (char *, lin *);
static enum diff intuit_diff_type (bool, mode_t *);
static void next_intuit_at (file_offset, lin);
static void skip_to (file_offset, lin);
    file_offset pos;

      pfp = stdin;
      }
#if HAVE_SETMODE_DOS
    if (binary_transput)
      {
	if (isatty (fileno (pfp)))
	  fatal ("cannot read binary data from tty on this platform");
	setmode (fileno (pfp), O_BINARY);
      }
#endif
    if (fstat (fileno (pfp), &st) != 0)
      pfatal ("fstat");
    if (S_ISREG (st.st_mode) && (pos = file_tell (pfp)) != -1)
      file_pos = pos;
    else
      {
	size_t charsread;
	int fd = make_tempfile (&TMPPATNAME, 'p', NULL, O_RDWR | O_BINARY, 0);
	FILE *read_pfp = pfp;
	TMPPATNAME_needs_removal = 1;
	pfp = fdopen (fd, "w+b");
	if (! pfp)
	  pfatal ("Can't open stream for file %s", quotearg (TMPPATNAME));
	for (st.st_size = 0;
	     (charsread = fread (buf, 1, bufsize, read_pfp)) != 0;
	     st.st_size += charsread)
	  if (fwrite (buf, 1, charsread, pfp) != charsread)
	    write_fatal ();
	if (ferror (read_pfp) || fclose (read_pfp) != 0)
	  read_fatal ();
	if (fflush (pfp) != 0
	    || file_seek (pfp, (file_offset) 0, SEEK_SET) != 0)
	  write_fatal ();
    next_intuit_at (file_pos, 1);
maybe_reverse (char const *name, bool nonexistent, bool is_empty)
  bool looks_reversed = (! is_empty) < p_says_nonexistent[reverse ^ is_empty];

  /* Allow to create and delete empty files when we know that they are empty:
     in the "diff --git" format, we know that from the index header.  */
  if (is_empty
      && p_says_nonexistent[reverse ^ nonexistent] == 1
      && p_says_nonexistent[! reverse ^ nonexistent] == 2)
    return false;

  if (looks_reversed)
    reverse ^=
      ok_to_reverse ("The next patch%s would %s the file %s,\nwhich %s!",
		     reverse ? ", when reversed," : "",
		     (nonexistent ? "delete"
		      : is_empty ? "empty out"
		      : "create"),
		     quotearg (name),
		     (nonexistent ? "does not exist"
		      : is_empty ? "is already empty"
		      : "already exists"));
  return looks_reversed;
there_is_another_patch (bool need_header, mode_t *file_type)
    diff_type = intuit_diff_type (need_header, file_type);
	    diff_type == GIT_BINARY_DIFF ? "a git binary diff" :
    if (no_strip_trailing_cr)
      p_strip_trailing_cr = false;

	if (p_strip_trailing_cr)
	    if (lstat (inname, &instat) == 0)
static mode_t _GL_ATTRIBUTE_PURE
fetchmode (char const *str)
{
   const char *s;
   mode_t mode;

   while (ISSPACE ((unsigned char) *str))
     str++;

   for (s = str, mode = 0; s < str + 6; s++)
     {
       if (*s >= '0' && *s <= '7')
	mode = (mode << 3) + (*s - '0');
       else
	{
	 mode = 0;
	 break;
	}
     }
   if (*s == '\r')
     s++;
   if (*s != '\n')
     mode = 0;

    /* NOTE: The "diff --git" format always sets the file mode permission
       bits of symlinks to 0.  (On Linux, symlinks actually always have
       0777 permissions, so this is not even consistent.)  */

   return mode;
}

static int
sha1_says_nonexistent(char const *sha1, char const *end)
{
  char const *empty_sha1 = "e69de29bb2d1d6434b8b29ae775ad8c2e48c5391";
  char const *s;

  /* Nonexisting files have an all-zero checksum.  */
  for (s = sha1; s != end; s++)
    if (*s != '0')
      break;
  if (s == end)
    return 2;

  /* Empty files have empty_sha1 as their checksum.  */
  for (s = sha1; s != end; s++, empty_sha1++)
    if (*s != *empty_sha1)
      break;
  return s == end;
}

static char const * _GL_ATTRIBUTE_PURE
skip_hex_digits (char const *str)
{
  char const *s;

  for (s = str; (*s >= '0' && *s <= '9') || (*s >= 'a' && *s <= 'f'); s++)
    /* do nothing */ ;
  return s == str ? NULL : s;
}

static bool
name_is_valid (char const *name)
{
  char const *n;
  int i;
  bool is_valid = true;

  for (i = 0; i < ARRAY_SIZE (invalid_names); i++)
    {
      if (! invalid_names[i])
	break;
      if (! strcmp (invalid_names[i], name))
	return false;
    }

  if (IS_ABSOLUTE_FILE_NAME (name))
    is_valid = false;
  else
    for (n = name; *n; )
      {
	if (*n == '.' && *++n == '.' && ( ! *++n || ISSLASH (*n)))
	  {
	    is_valid = false;
	    break;
	  }
	while (*n && ! ISSLASH (*n))
	  n++;
	while (ISSLASH (*n))
	  n++;
      }

  if (! is_valid)
    {
      say ("Ignoring potentially dangerous file name %s\n", quotearg (name));
      if (i < ARRAY_SIZE (invalid_names))
	invalid_names[i] = name;
    }
  return is_valid;
}

intuit_diff_type (bool need_header, mode_t *p_file_type)
    file_offset this_line = 0;
    file_offset first_command_line = -1;
    lin fcl_line = 0; /* Pacify 'gcc -W'.  */
    bool this_is_a_command = false;
    bool stars_this_line = false;
    bool extended_headers = false;
    enum diff retval;
    mode_t file_type;
    for (i = 0; i < ARRAY_SIZE (invalid_names); i++)
	invalid_names[i] = NULL;
    p_git_diff = false;
    for (i = OLD; i <= NEW; i++)
      {
	p_mode[i] = 0;
	p_copy[i] = false;
	p_rename[i] = false;
      }
    p_timestamp[OLD].tv_sec = p_timestamp[NEW].tv_sec = -1;
	char *s;
	char *t;
	file_offset previous_line = this_line;
	bool last_line_was_command = this_is_a_command;
	bool stars_last_line = stars_this_line;
	size_t indent = 0;
		if (extended_headers)
		  {
		    /* Patch contains no hunks; any diff type will do. */
		    retval = UNI_DIFF;
		    goto scan_exit;
		  }
	      /* do nothing */ ;
		  /* do nothing */ ;
		  /* do nothing */ ;
	    fetchname (s+4, strippath, p_git_diff, &p_name[OLD], &p_timestr[OLD],
		       &p_timestamp[OLD]);
	    fetchname (s+4, strippath, p_git_diff, &p_name[OLD], &p_timestr[OLD],
		       &p_timestamp[OLD]);
	    fetchname (s+6, strippath, p_git_diff, &p_name[INDEX], (char **) 0, NULL);
	else if (strnEQ(s, "Prereq:", 7))
	  {
	      /* do nothing */ ;
		    /* do nothing */ ;
	  }
	else if (strnEQ (s, "diff --git ", 11))
	  {
	    char const *u;

	    if (extended_headers)
	      {
		p_start = this_line;
		p_sline = p_input_line;
		/* Patch contains no hunks; any diff type will do. */
		retval = UNI_DIFF;
		goto scan_exit;
	      }

	    for (i = OLD; i <= NEW; i++)
	      {
		free (p_name[i]);
		p_name[i] = 0;
	      }
	    if (! ((p_name[OLD] = parse_name (s + 11, strippath, &u))
		   && ISSPACE ((unsigned char) *u)
		   && (p_name[NEW] = parse_name (u, strippath, &u))
		   && (u = skip_spaces (u), ! *u)))
	      for (i = OLD; i <= NEW; i++)
		{
		  free (p_name[i]);
		  p_name[i] = 0;
		}
	    p_git_diff = true;
	  }
	else if (p_git_diff && strnEQ (s, "index ", 6))
	  {
	    char const *u, *v;
	    if ((u = skip_hex_digits (s + 6))
		&& u[0] == '.' && u[1] == '.'
		&& (v = skip_hex_digits (u + 2))
		&& (! *v || ISSPACE ((unsigned char) *v)))
	      {
		p_says_nonexistent[OLD] = sha1_says_nonexistent (s + 6, u);
		p_says_nonexistent[NEW] = sha1_says_nonexistent (u + 2, v);
		if (*(v = skip_spaces (v)))
		  p_mode[OLD] = p_mode[NEW] = fetchmode (v);
		extended_headers = true;
	      }
	  }
	else if (p_git_diff && strnEQ (s, "old mode ", 9))
	  {
	    p_mode[OLD] = fetchmode (s + 9);
	    extended_headers = true;
	  }
	else if (p_git_diff && strnEQ (s, "new mode ", 9))
	  {
	    p_mode[NEW] = fetchmode (s + 9);
	    extended_headers = true;
	  }
	else if (p_git_diff && strnEQ (s, "deleted file mode ", 18))
	  {
	    p_mode[OLD] = fetchmode (s + 18);
	    p_says_nonexistent[NEW] = 2;
	    extended_headers = true;
	  }
	else if (p_git_diff && strnEQ (s, "new file mode ", 14))
	  {
	    p_mode[NEW] = fetchmode (s + 14);
	    p_says_nonexistent[OLD] = 2;
	    extended_headers = true;
	  }
	else if (p_git_diff && strnEQ (s, "rename from ", 12))
	  {
	    /* Git leaves out the prefix in the file name in this header,
	       so we can only ignore the file name.  */
	    p_rename[OLD] = true;
	    extended_headers = true;
	  }
	else if (p_git_diff && strnEQ (s, "rename to ", 10))
	  {
	    /* Git leaves out the prefix in the file name in this header,
	       so we can only ignore the file name.  */
	    p_rename[NEW] = true;
	    extended_headers = true;
	  }
	else if (p_git_diff && strnEQ (s, "copy from ", 10))
	  {
	    /* Git leaves out the prefix in the file name in this header,
	       so we can only ignore the file name.  */
	    p_copy[OLD] = true;
	    extended_headers = true;
	  }
	else if (p_git_diff && strnEQ (s, "copy to ", 8))
	  {
	    /* Git leaves out the prefix in the file name in this header,
	       so we can only ignore the file name.  */
	    p_copy[NEW] = true;
	    extended_headers = true;
	  }
	else if (p_git_diff && strnEQ (s, "GIT binary patch", 16))
	  {
	    p_start = this_line;
	    p_sline = p_input_line;
	    retval = GIT_BINARY_DIFF;
	    goto scan_exit;
	  }
	else
	      /* do nothing */ ;
		struct timespec timestamp;
		timestamp.tv_sec = -1;
		fetchname (t+4, strippath, p_git_diff, &p_name[NEW], &p_timestr[NEW],
			   &timestamp);
		if (timestamp.tv_sec != -1)
	    /* 'p_name', 'p_timestr', and 'p_timestamp' are backwards;
	    struct timespec ti = p_timestamp[OLD];
	      p_says_nonexistent[OLD] = 1 + ! p_timestamp[OLD].tv_sec;
	      p_says_nonexistent[NEW] = 1 + ! p_timestamp[NEW].tv_sec;
	    if (! ((p_name[OLD] || ! p_timestamp[OLD].tv_sec)
		   && (p_name[NEW] || ! p_timestamp[NEW].tv_sec))
	      p_says_nonexistent[OLD] = 1 + ! p_timestamp[OLD].tv_sec;
	      lin saved_p_bline = p_bline;
		p_says_nonexistent[NEW] = 1 + ! p_timestamp[NEW].tv_sec;
	    if (! ((p_name[OLD] || ! p_timestamp[OLD].tv_sec)
		   && (p_name[NEW] || ! p_timestamp[NEW].tv_sec))
    /* The old, new, or old and new file types may be defined.  When both
       file types are defined, make sure they are the same, or else assume
       we do not know the file type.  */
    file_type = p_mode[OLD] & S_IFMT;
    if (file_type)
      {
	mode_t new_file_type = p_mode[NEW] & S_IFMT;
	if (new_file_type && file_type != new_file_type)
	  file_type = 0;
      }
    else
      {
	file_type = p_mode[NEW] & S_IFMT;
	if (! file_type)
	  file_type = S_IFREG;
      }
    *p_file_type = file_type;

    /* To intuit 'inname', the name of the file to patch,
	 and take the index name from the 'Index:' line if present and
       - Otherwise, report failure by setting 'inname' to 0;
	      else if (lstat (p_name[i], &st[i]) != 0)
	      else if (lookup_file_id (&st[i]) == DELETE_LATER)
		stat_errno[i] = ENOENT;
		  if (posixly_correct && name_is_valid (p_name[i]))
	    /* The best of all existing files. */
			      free (diffbuf);
		&& (i == NONE || (st[i].st_mode & S_IFMT) == file_type)
		&& maybe_reverse (p_name[i == NONE ? i0 : i], i == NONE,
				  i == NONE || st[i].st_size == 0)
		&& i == NONE)
		/* The best of the filenames which create the fewest directories. */
	    inerrno = lstat (inname, &instat) == 0 ? 0 : errno;
	    if (inerrno || (instat.st_mode & S_IFMT) == file_type)
	      maybe_reverse (inname, inerrno, inerrno || instat.st_size == 0);
	      stat_result = lstat (filename, &stat_buf);
	&& name_is_valid (name[i])
next_intuit_at (file_offset file_pos, lin file_line)
skip_to (file_offset file_pos, lin file_line)
    FILE *i = pfp;
    FILE *o = stdout;
    int c;
scan_linenum (char *s0, lin *linenum)
  lin n = 0;
      lin new_n = 10 * n + (*s - '0');
    char *s;
    lin context = 0;
    size_t chars_read;
	lin repl_beginning = 0;		/* index of --- line */
	lin fillcnt = 0;	/* #lines of missing ptrn or repl */
	lin fillsrc;		/* index of first line to copy */
	lin filldst;		/* index of first missing line */
	bool repl_could_be_missing = true;
	lin repl_patch_line;		/* input line number for same */
	lin repl_context;		/* context for same */
	lin ptrn_prefix_context = -1;	/* lines in pattern prefix context */
	lin ptrn_suffix_context = -1;	/* lines in pattern suffix context */
	lin repl_prefix_context = -1;	/* lines in replac. prefix context */
	lin ptrn_copiable = 0;		/* # of copiable lines in ptrn */
	lin repl_copiable = 0;		/* Likewise for replacement.  */

	/* Pacify 'gcc -Wall'.  */
		    fatal ("unexpected '***' at line %s: %s",
		  /* do nothing */ ;
			/* 'Old' lines were omitted.  Set up to fill
			   them in from 'new' context lines.  */
		      fatal ("%s '---' at line %s; check line numbers at line %s",
		      fatal ("duplicate '---' at line %s; check line numbers at line %s",
		  /* do nothing */ ;
	  fatal ("no '---' found in patch at line %s",
	if (p_prefix_context == -1 || p_suffix_context == -1)
	    fatal ("replacement text or line numbers mangled in hunk at line %s",
		   format_linenum (numbuf0, p_hunk_beg));
	file_offset line_beginning = file_tell (pfp);  /* file pos of the current line */
	lin fillsrc;  /* index of old lines */
	lin filldst;  /* index of new lines */
	int i;
	lin min, max;
	      fatal ("'<' expected at line %s of patch",
	      fatal ("'---' expected at line %s of patch",
	      fatal ("'>' expected at line %s of patch",
	lin i;
  FILE *fp = pfp;
  int c;
  size_t i;
  char *b;
  size_t s;
  FILE *fp = pfp;
  int c;
  file_offset line_beginning = file_tell (fp);
	/* do nothing */ ;
    lin i;
    lin n;
    char *s;
	free (p_line);
	free (p_len);
	free (p_Char);
    free (tp_line);
    free (tp_len);
    free (tp_char);
bool pch_copy (void)
{
  return p_copy[OLD] && p_copy[NEW]
	 && p_name[OLD] && p_name[NEW];
}
bool pch_rename (void)
  return p_rename[OLD] && p_rename[NEW]
	 && p_name[OLD] && p_name[NEW];
lin
lin
lin
lin
lin
lin
lin
pch_line_len (lin line)
pch_char (lin line)
pfetch (lin line)
pch_write_line (lin line, FILE *file)
lin
/* Return true if in a git-style patch. */

bool
pch_git_diff (void)
{
  return p_git_diff;
}

mode_t
pch_mode (bool which)
{
  return p_mode[which];
}

/* Is the newline-terminated line a valid 'ed' command for patch
static char _GL_ATTRIBUTE_PURE
	/* do nothing */ ;
	    /* do nothing */ ;
do_ed_script (char const *inname, char const *outname,
	      int *outname_needs_removal, FILE *ofp)
    static char const editor_program[] = EDITOR_PROGRAM;
    file_offset beginning_of_this_line;
    FILE *pipefp = 0;
    size_t chars_read;
	int exclusive = *outname_needs_removal ? 0 : O_EXCL;
	*outname_needs_removal = 1;
	copy_file (inname, outname, 0, exclusive, instat.st_mode, true);
	sprintf (buf, "%s %s%s", editor_program,
		 verbosity == VERBOSE ? "" : "- ",
		 outname);
      fatal ("%s FAILED", editor_program);
	FILE *ifp = fopen (outname, binary_transput ? "rb" : "r");
	  pfatal ("can't open '%s'", outname);
  lin old = 1;
  lin new = p_ptrn_lines + 1;