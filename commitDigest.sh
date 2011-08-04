#!/bin/bash

# Set pager options (assuming using 'less'. Disables default "-S" option which chops off lines wider than the terminal.
export LESS=FRX

# Run git log. Arguments passed directly into git log: you can use all forms of git log. In the simplest case, use oldRef..newRef.  You can also omit newRef if you're going all the way to the HEAD of your working folder.
git log $@
