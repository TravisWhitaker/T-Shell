# T-Shell

## What is T-Shell?

  T-Shell is my first large project written entirely in [C][C] (despite GitHub's language meter). It is a [Linux][Linux] [Command Line Shell][Shell].

***

## Goal
  **Short term:** I've been using this project as a kind of road-map to follow while I learn [C][C]. In addition to that, I've been learning more about [Command Line Shells][Shell] and [Linux][Linux] in general.
  
  **Long term:** I intend make this a comprehensive shell, with capabilities similar to that of [Bash][Bash], [Zsh][Zsh], and [Fish][Fish].

***

# Features
  - Execution of external programs (Commands).
  - [Redirection][Redirection]:
    - [Piping][Pipeline].
    - Output.
      - Overwrite file.
      - Append to file.
    - Input.
  - Command Aliasing.

***

## Things I've Learned
  - How to program in [C][C]:
    - The Basics:
      - Primative Data Types.
      - Declaring and Calling Functions.
        - Making [System Calls][Syscall].
      - Using the [Preprocessor][Preprocessor].
    	  - Including Header files.
    	  - Using Macros.
    	  - Conditional Code Inclusion.
    - More Advanced features:
      - Managing Memory.
        - Introduced to the fundamental concepts of the Stack and Heap.
        - The types of allocation:
          - [Automatic][Automatic].
          - [Static][Static].
          - [Dynamic][Dynamic].
      - File I/O (Text & Binary).
      - Using Structs and Unions to build more complex data types.
      - Optimizing with keywords like `inline` and `register`.

  - The characteristics / common features of a [Command Line Shell][Shell].
    - Command Aliasing.
    - [Redirection][Redirection].
  
  - What [System Calls][Syscall] are.
  - Basic [Signal][Signal] Handling.
  - Creating and running new processes ([Fork][Fork] & [Exec][Exec]).
  - What [ANSI Escape Codes][ANSI Escape Codes] are and how to use some.
  - How to write a [Man page][Man page].
  - How to use [Makefiles][Makefile].
  - How to use [Valgrind][Valgrind].
  - Introduced me to [Clang][Clang]/[LLVM][LLVM].

***

## Dependencies:
  - glibc (2.16 or later) ([GNU C Library][GLIBC])
  - gcc ([GNU Compiler Collection][GCC]) OR clang ([Clang][Clang]/[LLVM][LLVM])
  - readline ([GNU Readline][Readline])
  - Data-Structs/[hash][Hash]
  - Data-Structs/[vector][Vector]
  - [StrUtil][StrUtil]

***

## How to Install:
  To install, do the following...<br>
    1) Dowload the zip file from the link on the right.<br>
    2) `unzip master.zip`<br>
    3) `cd T-Shell-master`<br>
    4) `make`<br>
    5) `[sudo] make install`<br><br>
  Notes:<br>
  1) The Makefile specifies [Clang][Clang]/[LLVM][LLVM] as the compiler, feel free to change that.<br>
  2) When un-installing, you should remove T-Shell from `/etc/shells`.
  
[C]: http://en.wikipedia.org/wiki/C_(programming_language)
[GLIBC]: http://en.wikipedia.org/wiki/GNU_C_Library
[GCC]: http://en.wikipedia.org/wiki/GNU_Compiler_Collection
[Clang]: http://en.wikipedia.org/wiki/Clang
[LLVM]: http://en.wikipedia.org/wiki/LLVM
[Preprocessor]: http://en.wikipedia.org/wiki/C_preprocessor
[Syscall]: http://en.wikipedia.org/wiki/System_call
[Automatic]: http://en.wikipedia.org/wiki/Automatic_memory_allocation
[Static]: http://en.wikipedia.org/wiki/Static_memory_allocation
[Dynamic]: http://en.wikipedia.org/wiki/Dynamic_memory_allocation#Dynamic_memory_allocation
[Readline]: http://en.wikipedia.org/wiki/GNU_Readline
[Hash]: https://github.com/tyler-cromwell/Data-Structs/tree/master/hash
[Vector]: https://github.com/tyler-cromwell/Data-Structs/tree/master/vector
[StrUtil]: https://github.com/tyler-cromwell/StrUtil
[Man page]: http://en.wikipedia.org/wiki/Man_page
[Makefile]: http://en.wikipedia.org/wiki/Make_(software)
[Valgrind]: http://en.wikipedia.org/wiki/Valgrind
[Exec]: http://en.wikipedia.org/wiki/Exec_(computing)
[Fork]: http://en.wikipedia.org/wiki/Fork_(system_call)
[Pipeline]: http://en.wikipedia.org/wiki/Pipeline_(Unix)
[ANSI Escape Codes]: http://en.wikipedia.org/wiki/ANSI_escape_code
[Generics]: http://en.wikipedia.org/wiki/Generic_programming
[OOP]: http://en.wikipedia.org/wiki/Object-oriented_programming
[Redirection]: http://en.wikipedia.org/wiki/Redirection_(computing)
[Linux]: http://en.wikipedia.org/wiki/Linux
[Signal]: http://en.wikipedia.org/wiki/Unix_signal
[Shell]: http://en.wikipedia.org/wiki/Shell_(computing)
[Bash]: http://en.wikipedia.org/wiki/Bash_(Unix_shell)
[Zsh]: http://en.wikipedia.org/wiki/Z_shell
[Fish]: http://en.wikipedia.org/wiki/Friendly_interactive_shell
