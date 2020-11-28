# Ccat


## Description

Ccat is a simple CLI tool for Linux that brings colors and decorations to
your files and command outputs.  To achieve this, it surrounds patterns
with [ANSI escape sequences](https://en.wikipedia.org/wiki/ANSI_escape_code).
Better than a long speech, watch the video below!

[![asciicast](https://asciinema.org/a/DI0e612jZcNKz0nK1lNFf7S6S.svg)](https://asciinema.org/a/DI0e612jZcNKz0nK1lNFf7S6S)


## Features

Current features (version 1.1):

  - 3 decorations: bold, italic, underline
  - 8 colors: black, red, green, brown, blue, magenta, cyan, white
  - colorize patterns based on [POSIX extended regular expressions](https://www.gnu.org/software/findutils/manual/html_node/find_html/posix_002dextended-regular-expression-syntax.html)
  - colorize regexp sub matches
  - export coloration results to HTML files


## Installation

Download, compile and install with:

    git clone --depth https://github.com/v14dz/ccat
    cd ccat
    make
    sudo make install

After the `make install` command, the `ccat` binary will be placed under the
`/usr/local/bin/` directory.


## Usage

Usage is obtained with the `-h` option:

    usage: ./ccat [options] [color1[:decoration1] pattern1] [[color2[:decoration2] pattern2] ...] [filename]

      -c|--config <cfg>    Use configuration file
      -g|--group           Only colorize sub matches (use parentheses)
      -h|--help            Display this help
      -t|--html            Set output as HTML
      -v|--version         Display current version

    Possible colors are: black red green brown blue magenta cyan white
    Possible decorations are: 'b' (bold) 'i' (italic) or 'u' (underline)

Colorizes the `/etc/passwd` file to have the "root" string in *red* and
default shells in *green* and *underlined* (note that patterns can be
[POSIX extended regular expressions](https://www.gnu.org/software/findutils/manual/html_node/find_html/posix_002dextended-regular-expression-syntax.html)):

    ccat red 'root' green:underline ':[^:]+$' /etc/passwd

Colorizes an input string (received from *stdin*):

    echo {a..h} | ccat black 'a' red 'b' green 'c' brown 'd' \
                       blue 'e' magenta 'f' cyan 'g' white 'h'

Colorizes sub matches fields (with `-g`):

    echo "xxxxxx" | ccat -g green '(.).(.).(.).' cyan '.(.).(.).(.)'

Colorizes a log file (`/var/log/messages`) and highlight critical keywords
with bold red:

    ccat -g green  '^(.*\s\w{2}:\w{2}:\w{2})\s\S+\s+\S+: .*$' \
            blue   '^.*\s\w{2}:\w{2}:\w{2}\s(\S+)\s+\S+: .*$' \
            cyan   '^.*\s\w{2}:\w{2}:\w{2}\s\S+\s+(\S+): .*$' \
            red:b  '(error|failed|corrupt)' \
            /var/log/messages

Colorizes the output of an hex dump and highlight a specific byte:

    xxd /tmp/data.dump | ccat cyan:b '^[0-f]+:' blue:i '  .*$' red:u '92'


## Configuration files

Ccat parameters are sometime long to type or remember, fortunately you can
store those parameters to a configuration file.  Syntax is really simple,
for example, the following configuration file, colorizes and display the
file `/var/log/messages`:

    # Configuration file to display message with ccat.
    filename   /var/log/messages
    flags      g
    # Colors
    green      ^(.*\s\w{2}:\w{2}:\w{2})\s\S+\s+\S+: .*$
    blue       ^.*\s\w{2}:\w{2}:\w{2}\s(\S+)\s+\S+: .*$
    cyan       ^.*\s\w{2}:\w{2}:\w{2}\s\S+\s+(\S+): .*$
    red:b      (error|failed|corrupt)

Note that *flags* value may contains any letter that matches a flag option
(i.e. *g* for group, *t* for html and so on).  This configuration file will
produce the same output of the command show previously if invoked with:

    ccat -c ./messages.cfg

You may want to apply the same settings to colorize another file
(`/var/log/messages.1` for instance), in this case use:

    ccat -c ./messages.cfg /var/log/messages.1

You may also want to add more color/pattern pairs to the command line:

    ccat -c ./messages.cfg red '(failure)'

To save a colored output exported to an HTML file:

    ccat --html -c ./messages.cfg > /tmp/messages.html

Configuration files shipped with the sources (i.e. those in the
[configs](./configs/) directory) are copied under the
`/usr/local/etc/ccat/` directory.  To store your own configuration files,
place them in `~/.config/ccat/` (and keep the `*.cfg` extension).  Once
a configuration file is placed either in the system directory
(`/usr/local/etc/ccat/`) or in a user directory (`~/.config/ccat/`), it can
be used by specifying it's name, for instance:

    ccat -c messages /var/log/messages.1
