# The lexical tokenizer.

This is a description of the lexer used by the nuvm assembler to tokenize its
input. It's meant as a companion to the finite state machine diagram available
inside `diagrams/tokenizer-state-machine.svg`.

## Tokens Recognized

The tokenizer as it is supports 4 classes of tokens:
  - Status tokens, used to describe an error, end of input or otherwise
    unrecognized input;
  - Data tokens, used to describe numbers, strings and locations (labels);
  - Keyword tokens, used to describe the structural elements of an assembly
    module file, as well as constant types;
  - Opcode tokens, used to describe the operations in the machine code;

The status tokens are `EOF`, `TOO_BIG`, `UNKNOWN`, `UNRECOGNIZED_KW` and
`UNRECOGNIZED_OPCODE`. Of those, `EOF` signals the end of input has been while
`TOO_BIG` signals the current input token is bigger than the tokenizer buffer.

`UNRECOGNIZED_KW` and `UNRECOGNIZED_OPCODE` are output when input matches the
format of a Keyword or Opcode token, respectively, but it isn't any of the
known tokens for these classes.

`UNKNOWN` is the default token type for whatever input doesn't match any other
known token.


The data tokens are `DEC_INTEGER`, `HEX_INTEGER`, `REAL`, `LABEL`, `LABEL_DEF`,
`REGISTER` and `STRING`. Four of those (`DEC_INTEGER`, `HEX_INTEGER`, `REAL`
and `STRING`) are used to describe constants contents, while the other three
(`LABEL`, `LABEL_DEF` and `REGISTER`) are used to describe locations of code
and data.

`DEC_INTEGER` is also used here and there to parameterize a few structure
elements, like expected virtual machine version or number of global registers.


The keyword tokens are all tokens that begin with `KW_`. A group of them
(`KW_VERSION`, `KW_GLOBALS_COUNT`, `KW_CONSTANTS` and `KW_CODE`) are used to
describe the structure of the assembly file while the rest (`KW_CHARACTER`,
`KW_DOUBLE`, `KW_INT32`, `KW_PROCEDURE` and `KW_STRING`) are used to specify
the type of a constant in the constants list.


The Opcode tokens are all tokens that begin with `OP_`, and each one of them
represents a opcode name in the machine code language. There's no point to
provide a comprehensive description of each one here, since this will be the same
as the description of the opcodes themselves.


## Recognized Language Basic Description

Each of the token types has a pattern associated with them which is used to
determine if input fits that token type. Below we have a prose description of
the language recognized, so an intuition of the intent can be formed. A formal
description is left for the next session.

So that a token pattern description doesn't have to take into account the 
description of the exceptions handled by other rules, assume that the patterns
described are tested in sequence, yielding the first result that matches.
While that's not how the lexer works, it gives a good intuition.

Also note that only whitespace characters (such as tab, space or newline)
separate tokens so there's no ambiguity between a single token type and two
other tokens laid one after the other.


### Register descriptor

A register is described by a register type identifier followed by a colon and a
decimal positive integer number, where register type identifiers are either 'C'
(for constants), 'G' (for globals) or 'L' (for locals). 

Examples of register descriptors are "C:0", "L:10" and "G:10588".


### Label Definition

A Label definition consists of a name followed by a colon, where a name starts
with any upper or lower case letter which can be followed by any number of 
upper or lower case letters, numbers or the minus sign (-).

Examples of label definitions are "main:", "hello-world:" and "the-2nd-label:".


### Label Reference

A Label reference consists of the at character (@) followed by a name, where
a name follows the same rules as for Label Definition above.

Examples of label references are "@main", "@hello-world" and "@the-2nd-label".

### Decimal Integers

A Decimal Integer consists of an optional minus sign (-) followed by one or
more decimal digits.

Examples of Decimal Integers are "0", "1234567890" and "-256".


### Hexadecimal Integers

A Hexadecimal Integer starts with the prefix 0x and is followed by one or
more hexadecimal digits (0 to 9 or a to f).

Examples of Hexadecimal Integers are "0x0", "0xA123" and "0xDEADBEEF".


### Real Numbers

A Real Number starts with a Decimal Integer but is followed by a dot (.) and
one or more digits.

Examples of Real Numbers are "0.0", "123.45" and "-256.1".


### Keywords

A Keyword starts with a dot (.) and is followed by any number of lower case
letters, digits or minus signs (-). Those are possible keywords: in order to
actually be recognized as a valid keyword they must be equal to one of the
following list:
  - ".character"
  - ".code"
  - ".constants"
  - ".double"
  - ".entry-point"
  - ".globals-count"
  - ".int32"
  - ".procedure"
  - ".string"
  - ".version"


### Opcodes

An Opcode is but a name as described in Label Definition but, like Keywords,
matching the pattern for Opcode is only the first step towards recognition:
in order to actually be recognized as a valid opcode the input must be equal
to one of the opcode names.

These are liable for change and might end up being out of date so it's always
a good idea to check the OPCODE_TABLE structure inside `src/asm/tokens.c`.

As of now the list of opcodes recognized by the lexer is as follows:
  - "move"
  - "global-ref"
  - "global-set"
  - "jump"
  - "jump-if"
  - "eq"
  - "lt"
  - "le"
  - "gt"
  - "ge"
  - "not"
  - "or"
  - "and"
  - "add"
  - "sub"
  - "mul"
  - "div"
  - "load-bool"


### Strings

Strings are sequences of characters enclosed in double quotes (") and can not
contain the double quote character or a newline, except when these are preceded
by a backslash (\) character.


## State Machine Description

To identify these tokens in an input a Finite State Machine is used, which is
defined by the following list of regular expressions (in POSIX Extended Regular
Expression Syntax):

    [CLG]:[0-9]+                             REGISTER
    [a-zA-Z][a-zA-Z0-9-]*                    UNRECOGNIZED_OPCODE
    [a-zA-Z][a-zA-Z0-9-]*:                   LABEL_DEF
	@[a-zA-Z][a-zA-Z0-9-]*                   LABEL
    "(\\.|[^\"])"                            STRING
    0|(-?[1-9][0-9]*)                        DEC_INTEGER
    0x[0-9A-F]+                              HEX_INTEGER
    (0|(-?[1-9][0-9]*))\.[0-9]+              REAL
    \..*                                     UNRECOGNIZED_KW

As before, these are laid out in a way that should be interpreted that the 
longest match wins, with the one appearing higher on the list taking precedence
if two rules apply.
