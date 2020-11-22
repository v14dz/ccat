# Ccat (Color cat)

*Ccat colorizes command outputs or plaintext files on your terminal.*

## Description

Ccat can be used to print your log files or command outputs with colors.
For this, it surrounds matched patterns, with terminal escape sequences.
Better than a long speech, here are some screenshots.

The xxd command output (hex dump data):

![](http://vladz.devzero.fr/images/ccat1.png)

The objdump command output (binary disassembly):

![](http://vladz.devzero.fr/images/ccat2.png)

## Installation

Download, compile and install with:

    git clone --depth https://github.com/v14dz/ccat
    cd ccat
    make
    sudo make install

After this, the ``ccat`` binary will be placed under the
``/usr/local/bin/`` directory.

## Usage

Usage is obtained with the ``-h`` option:

    usage: ./ccat [options] [color1[:decoration1] pattern1] [[color2[:decoration2] pattern2] ...] [filename]

      -c|--config <cfg>    Use configuration file
      -g|--group           Only colorize sub matches (use parentheses)
      -h|--help            Display this help
      -t|--html            Set output as HTML
      -v|--version         Display current version

    Possible colors are: black red green brown blue magenta cyan white
    Possible decorations are: 'b' (bold) 'i' (italic) or 'u' (underline)

A pattern in Ccat is a [POSIX extended regular expression](https://www.gnu.org/software/findutils/manual/html_node/find_html/posix_002dextended-regular-expression-syntax.html).

## Some examples

Colorizes the ``/etc/passwd`` file to have the *root* string in red and
default shells in *green* and underlined:

    ccat red 'root' green:u ':[^:]+$' /etc/passwd

Colorizes an input string (received from *stdin*):

    echo {a..h} | ccat black 'a' red 'b' green 'c' brown 'd' \
                       blue 'e' magenta 'f' cyan 'g' white 'h'

Only colorizes sub matches fields (with ``-g``):

    echo "xxxxxx" | ccat -g green '(.).(.).(.).' cyan '.(.).(.).(.)'

Colorizes simple patterns (one per line only) with sub matches option and
show them on bold:

    ccat -g red:b '(root|error)' /var/log/kern.log

Colorizes a system log file (``/var/log/messages``) and highlight in bold
critical keywords:

    ccat -g green  '^(.*\s\w{2}:\w{2}:\w{2})\s\S+\s+\S+: .*$' \
            blue   '^.*\s\w{2}:\w{2}:\w{2}\s(\S+)\s+\S+: .*$' \
            cyan   '^.*\s\w{2}:\w{2}:\w{2}\s\S+\s+(\S+): .*$' \
            red:b  '(error|failed|corrupt)' \
            /var/log/messages

Colorizes the output of an hex dump and highlight a specific byte value:

    xxd /tmp/data.dump | ccat cyan '^[0-f]+:' blue '  .*$' red '92'

## Configuration files

Command lines are sometime long to type or remember, fortunately you can
store Ccat's parameters to a configuration file.  Syntax is really simple,
for example, the following file (``messages.cfg``) can be used to colorize
and display the file ``/var/log/messages``:

    # Configuration file to display message with ccat.
    filename   /var/log/messages
    flags      g
    # Colors
    green      ^(.*\s\w{2}:\w{2}:\w{2})\s\S+\s+\S+: .*$
    blue       ^.*\s\w{2}:\w{2}:\w{2}\s(\S+)\s+\S+: .*$
    cyan       ^.*\s\w{2}:\w{2}:\w{2}\s\S+\s+(\S+): .*$
    red:b      (error|failed|corrupt)

Note that *flags* key may contains any letter that matches a flag option
(i.e. **g** for group, **t** for html and so on).  This configuration file
will produce the same output of the command show previously if invoked
with:

    ccat -c ./messages.cfg

You may want to apply the same settings to colorize another file
(``/var/log/messages.1`` for instance), in this case use:

    ccat -c ./messages.cfg /var/log/messages.1

You may also want to add more color/pattern pairs to the configuration
file:

    ccat -c ./messages.cfg red '(failure)'

To save a colored output to an HTML file:

    ccat --html -c ./messages.cfg > /tmp/messages.html

Configuration files shipped with the sources (i.e. those in the
[configs](./configs/) directory) are copied under the
``/usr/local/etc/ccat/`` directory.  To store your own configuration file,
place them in ``~/.config/ccat/`` (and keep the ``*.cfg`` extension).  Once
a configuration file are place either in the system directory
(``/usr/local/etc/ccat/``) or in a user (``~/.config/ccat/``), it can be
used only by specifying it's name, for instance:

    ccat -c messages /var/log/messages.1
