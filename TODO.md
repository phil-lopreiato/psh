# Features to Add
Eventually...

  + Pipes
  + I/O Redirection
  + Add last command status to prompt
  + Build string parser so we can ditch `strtok` (can't handle spaces in strings). Can implement as state machine (in string, not in string, etc)
  + Improve variable expansion, don't need to be their own argument (e.g. within paths)
  + Support `~` expansion for home directory path
  + Implement `source` builtin function
  + Implement `alias` builtin function
  + Add default rc file that gets auto-sourced on startup
  + Catch up arrow and auto-fill previous command
  + Tab completion
