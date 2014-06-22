-- Include modules.
local cli = require "cliargs"
local colors = require "ansicolors"
local tlua = require "tlua"

-- Expose the package.
module("cli", package.seeall)

-- Define the name.
cli:set_name("webapp")

cli:add_argument("COMMAND", "command to execute", colors("list of commands:"))

-- Display the version.
cli:add_flag("-v, --version", "print version.")
cli:add_flag("-h, --help", "print how-to.")

-- Join one table into the other.
function extend(first, second)
  for i=1, #second do
    first[#first+1] = second[i]
  end
end

-- Reusable method to display the header.
function display_header()
  return {
    colors("WebApp Command Line Interface"),
    colors("%{greenbg black} webapp %{reset} Version - 1.0.0-wip"),

    "",
  }
end

-- Parses from _G['arg'].
local args = cli:parse_args()

-- Don't bother continuing if there are no useful arguments, flags, or options.
if #arg == 0 or not args then
  return
end

-- Checking for flags: is -v or --version set?
if args["v"] then
  local out = display_header()

  return print(table.concat(out, "\n"))
end
