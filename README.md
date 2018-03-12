TZCONV(1) - General Commands Manual

# NAME

**tzconv** - time zone conversion

# SYNOPSIS

**tzconv**
\[**-F**&zwnj;*inputfmt*]
\[**-f**&zwnj;*inputtz*]
\[**-T**&zwnj;*outputfmt*]
\[**-t**&zwnj;*outputtz*]
\[*file&nbsp;...*]

# DESCRIPTION

The
**tzconv**
utility reads the specified files, or the standard input if no files are
specified, converting timestamps in the input matched according to the
**-F**
and
**-f**
options.
The converted input is then written to standard output according to the
**-T**
and
**-t**
options.

The options are as follows:

**-F** *inputfmt*

> Parse input timestamp according to
> *inputfmt*
> which must be an
> strptime(3)
> accepted string.
> If missing the default format
> **%Y-%m-%d %H:%M:%S %Z**
> is used.

**-f** *inputtz*

> Parse input timestamp localized in the
> *inputtz*
> time zone.
> If missing the current time zone is used.

**-T** *outputfmt*

> Parse output timestamp according to
> *outputfmt*
> which must be an
> strftime(3)
> accepted string.
> If missing the default format
> **%Y-%m-%d %H:%M:%S %Z**
> is used.

**-t** *outputtz*

> Parse output timestamp localized in the
> *outputtz*
> time zone.
> If missing the current time zone is used.

If the input line has data after the
*inputfmt*
is parsed, its printed verbatim to output.

# ENVIRONMENT

`TZ`

> See
> tzset(3).

# FILES

*/usr/share/zoneinfo*

> time zone information directory

*/etc/localtime*

> local time zone file

# EXIT STATUS

The **tzconv** utility exits&#160;0 on success, and&#160;&gt;0 if an error occurs.

# EXAMPLES

To verify timestamp are correct for the
**Z**
(UTC) time zone:

		$ printf "%s\n" "2018-01-30 00:00:00 UTC: stuff" | \
			tzconv -fZ -tZ && echo line is ok
	2018-01-30 00:00:00 GMT: stuff
	line is ok

Now with bogus timestamp (there is no day 32):

		$ printf "%s\n" "2018-01-32 00:00:00 UTC: stuff" | \
			tzconv -fZ -tZ || echo line is not ok
	2018-01-32 00:00:00 UTC: stuff
	line is not ok

Convert from UTC to New\_York timezone:

		$ printf "%s\n" "2018-01-32 00:00:00 UTC: stuff" | \
			tzconv -fZ -tAmerica/New_York && echo line is ok
	2018-01-29 19:00:00 EST: stuff
	line is ok

# SEE ALSO

mktime(3),
strptime(3),
strftime(3),
tzset(3)

# AUTHORS

Abel Abraham Camarillo Ojeda &lt;[acamari@verlet.org](mailto:acamari@verlet.org)&gt;

# CAVEATS

Currently prints input lines not matching
*inputfmt*
verbatim to output, returning with &gt;0 exit status.
There might be a user selectable option to choose behaviour for
this in a future version.

OpenBSD 6.2 - March 12, 2018
