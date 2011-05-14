lude
====

Messing around with Lua and testing to see if its worth pursuing as an evented networking scripting platform thing.

Build lude:
-----------

All the dependancies should exist, `git submodule init --update` if you're missing anything.

  make jit

Should be all you need to get going with the luajit code.  Still need to figure out how to toggle different builds easier.

Benchmarking:
-------------

I was using unix `time` with the following commands:

  time build/lude example/hello.lua

  time node example/hello.js

If you have lua-llvm installed you can do:

  lua-compiler example/hello.lua

  time example/hello
