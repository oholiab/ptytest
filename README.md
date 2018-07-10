# Pseudoterminal Shenanigans
I wanted to understand how programs like `tmux` control pseudoterminals so I
started to follow along with some parts of [this excellent
site](https://www.uninformativ.de/blog/postings/2018-02-24/0/POSTING-en.html)
which details how they're implemented.

My code is largely a modified version of the example code. I'm hoping to play
around with manipulating and using the PTY from something other than X11 or
Wayland.
