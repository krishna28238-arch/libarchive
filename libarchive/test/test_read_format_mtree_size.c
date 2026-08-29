/*-
 * Copyright (c) 2026 krishna28238-arch
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR(S) ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR(S) BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#include "test.h"

/*
 * Verify how the size keyword interacts with the checkfs option.
 *
 * Like the other metadata keywords (uid, gid, mode, mtime, ...), an
 * explicit size in the specification must win over the value found on
 * disk; the on-disk size is only a fallback for entries without a
 * size keyword.  The nochange keyword inverts this and forces the
 * on-disk values to be used.
 */
DEFINE_TEST(test_read_format_mtree_size)
{
	struct archive_entry *ae;
	struct archive *a;
	char buf[16];
	int64_t filesize;

	assertMakeDir("mtree_size", 0777);
	assertChdir("mtree_size");
	/* The file referenced by the specification is 10 bytes long. */
	assertMakeFile("file", 0644, "0123456789");

	/* An explicit size must override the on-disk file size. */
	{
		static char spec[] =
		    "#mtree\n"
		    "x type=file size=3 contents=file\n";

		assert((a = archive_read_new()) != NULL);
		assertEqualIntA(a, ARCHIVE_OK,
		    archive_read_support_filter_all(a));
		assertEqualIntA(a, ARCHIVE_OK,
		    archive_read_support_format_all(a));
		assertEqualIntA(a, ARCHIVE_OK,
		    archive_read_set_options(a, "mtree:checkfs"));
		assertEqualIntA(a, ARCHIVE_OK,
		    archive_read_open_memory(a, spec, sizeof(spec)));
		assertEqualIntA(a, ARCHIVE_OK, archive_read_next_header(a, &ae));
		assertEqualString(archive_entry_pathname(ae), "x");
		assertEqualInt(archive_entry_size(ae), 3);
		filesize = archive_read_data(a, buf, sizeof(buf));
		assertEqualInt(filesize, 3);
		assertEqualIntA(a, ARCHIVE_EOF, archive_read_next_header(a, &ae));
		assertEqualIntA(a, ARCHIVE_OK, archive_read_close(a));
		assertEqualInt(ARCHIVE_OK, archive_read_free(a));
	}

	/* Without a size keyword, the on-disk file size is used. */
	{
		static char spec[] =
		    "#mtree\n"
		    "x type=file contents=file\n";

		assert((a = archive_read_new()) != NULL);
		assertEqualIntA(a, ARCHIVE_OK,
		    archive_read_support_filter_all(a));
		assertEqualIntA(a, ARCHIVE_OK,
		    archive_read_support_format_all(a));
		assertEqualIntA(a, ARCHIVE_OK,
		    archive_read_set_options(a, "mtree:checkfs"));
		assertEqualIntA(a, ARCHIVE_OK,
		    archive_read_open_memory(a, spec, sizeof(spec)));
		assertEqualIntA(a, ARCHIVE_OK, archive_read_next_header(a, &ae));
		assertEqualString(archive_entry_pathname(ae), "x");
		assertEqualInt(archive_entry_size(ae), 10);
		assertEqualIntA(a, ARCHIVE_EOF, archive_read_next_header(a, &ae));
		assertEqualIntA(a, ARCHIVE_OK, archive_read_close(a));
		assertEqualInt(ARCHIVE_OK, archive_read_free(a));
	}

	/* The nochange keyword forces the on-disk file size. */
	{
		static char spec[] =
		    "#mtree\n"
		    "x type=file size=3 nochange contents=file\n";

		assert((a = archive_read_new()) != NULL);
		assertEqualIntA(a, ARCHIVE_OK,
		    archive_read_support_filter_all(a));
		assertEqualIntA(a, ARCHIVE_OK,
		    archive_read_support_format_all(a));
		assertEqualIntA(a, ARCHIVE_OK,
		    archive_read_set_options(a, "mtree:checkfs"));
		assertEqualIntA(a, ARCHIVE_OK,
		    archive_read_open_memory(a, spec, sizeof(spec)));
		assertEqualIntA(a, ARCHIVE_OK, archive_read_next_header(a, &ae));
		assertEqualString(archive_entry_pathname(ae), "x");
		assertEqualInt(archive_entry_size(ae), 10);
		assertEqualIntA(a, ARCHIVE_EOF, archive_read_next_header(a, &ae));
		assertEqualIntA(a, ARCHIVE_OK, archive_read_close(a));
		assertEqualInt(ARCHIVE_OK, archive_read_free(a));
	}

	assertChdir("..");
}
