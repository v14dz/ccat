# Ccat (Color cat)

*Ccat is a simple utility that colorizes and prints plaintext files on your
terminal.*

![](./logo.png)

## Description

It can be used to print your log files or command outputs with colors.  For
this, it surrounds matched patterns, with escape sequences.  Better than a
long speech, here are some screenshots:

  - [xxd's output (hex dump data)](http://vladz.devzero.fr/images/ccat1.png)
  - [objdump's output (binary disassembly)](http://vladz.devzero.fr/images/ccat2.png)

## Installation

Compile and install with:

    make
    sudo make install

After this, the ``ccat`` binary will be placed under the
``/usr/include/bin/`` directory.

## Usage

Usage is obtained with the ``-h`` option:

    usage: ./ccat [options] [color1 pattern1] [[color2 pattern2] ...] [filename]

      -b|--bold            Use bold for matching patterns
      -c|--config <cfg>    Use configuration file
      -g|--group           Only colorize sub matches (use parentheses)
      -h|--help            Display this help
      -t|--html            Set output as HTML
      -v|--version         Display current version

    Possible colors are: black red green brown blue magenta cyan white

Pattern is a [POSIX extended regular expression](https://www.gnu.org/software/findutils/manual/html_node/find_html/posix_002dextended-regular-expression-syntax.html).

## Some examples

Colorizes the *passwd* file to have the *root* string in red and users
default shell in *green*:

    ccat -b red 'root' green ':[^:]+$' /etc/passwd

Colorizes an input string (received from *stdin*):

    echo {a..h} | ccat black 'a' red 'b' green 'c' brown 'd' \
                       blue 'e' magenta 'f' cyan 'g' white 'h'

Colorizes the fields using sub matches (with ``-g``):

    echo "xxxxxx" | ccat -gb green '(.).(.).(.).' cyan '.(.).(.).(.)'

Colorizes simple patterns (one per line only) with sub matches option:

    ccat -g red '(root|error)' /var/log/kern.log

Colorizes a system log file (``/var/log/messages``) and highlight critical
keywords:

    ccat -g green '^(.*\s\w{2}:\w{2}:\w{2})\s\S+\s+\S+: .*$' \
            blue  '^.*\s\w{2}:\w{2}:\w{2}\s(\S+)\s+\S+: .*$' \
            cyan  '^.*\s\w{2}:\w{2}:\w{2}\s\S+\s+(\S+): .*$' \
            red   '(error|failed|corrupt)' \
            /var/log/messages

Colorizes the output of an hex dump and highlight a specific byte value:

    xxd /tmp/data.dump | ccat cyan '^[0-f]+:' blue '  .*$' red '92'

## Configuration files

Command line are sometime long to type or remember, fortunately you can
store Ccat's parameters to a configuration file.  Syntax is really simple,
for example, the following file (``messages.cfg``) can be used to colorize
and display the file ``/var/log/messages``:

    # Configuration file to display message with ccat.
    filename   /var/log/messages
    flags      gb
    # Colors
    green      ^(.*\s\w{2}:\w{2}:\w{2})\s\S+\s+\S+: .*$
    blue       ^.*\s\w{2}:\w{2}:\w{2}\s(\S+)\s+\S+: .*$
    cyan       ^.*\s\w{2}:\w{2}:\w{2}\s\S+\s+(\S+): .*$
    red        (error|failed|corrupt)

Note that *flags* key may contains any letter that matches an flag option
(i.e. **b** for bold, **g** for group, **t** for html and so on).

This configuration file is invoked with (it will shows the same output of
the command in the previous chapter):

    ccat -c ./messages.cfg

You may want to apply the same settings to colorize and display another
file, in this case:

    ccat -c ./messages.cfg /var/log/messages.1

You may also want to add more color/pattern pairs on the fly:

    ccat -c ./messages.cfg red '(failure)'

To save a colored output to an HTML file:

    ccat --html -c ./messages.cfg > /tmp/messages.html

Some configuration files can be found as example in the
[configs](./configs/) directory.
