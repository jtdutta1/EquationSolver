# EquationSolver
This is a simple project to build a __proof of concept__ of an equation solver. This is still in the making.

## Current Feature
- Parse an equation and represent it in a binary tree with operation precedence.
- Display the tree as intended (still work in progress for incomplete trees).

## How to use
To see how the tool can be used using a predefined equation, you can use the following command

    make run

If you wish to manually compile and run it using your own parameters then run the following command

    make

and then run

    LD_LIBRARY_PATH=. ./solve <expression>

Example

    LD_LIBRARY_PATH=. ./solve 2+3+5+2/4

## Features currently not supported
❌ Paranthesis operators `()`

❌ Floating point numbers. E.g. `5.3` or `276.7`.