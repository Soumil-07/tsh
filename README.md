# tsh: The sTraightforward SHell

tsh is designed to be an *easy* and straightforward shell to use with minimal configuration. Originally written as an educational exercise, I plan to use tsh as a testing ground for fixing some of my common gripes with shells.

## Features:

* Pipes, input redirection, output redirection.
* History expansion, with a GNU readline interface.
* A minimal set of built-in commands.
* A decently pretty start-up prompt.

## Planned Features:

* Wildcard expansion.
* Tab completion.
* Syntax highlighting.
* A simple (and more intuitive) scripting interface.
* Configuration file support.

## Acknowledgements:

tsh's existence would be impossible (or the code at least 10 times longer) without these excellent open-source tools:
1. [flex](http://gnu.ist.utl.pt/software/flex/flex.html)
2. [bison](https://www.gnu.org/software/bison/)

And these brilliant resources for anybody looking to write their own shell:
1. https://brennan.io/2015/01/16/write-a-shell-in-c/
2. Introduction to Systems Programming: a Hands-on Approach, by Gustavo Rodriguez-Rivera and Justin Ennen,
