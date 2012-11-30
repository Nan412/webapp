-- Include modules.
local cli = require "cliargs"
local colors = require "ansicolors"
local tlua = require "tlua"

-- Expose the package.
module("cli", package.seeall)

-- Display the version.
cli:add_flag("-v, --version", "Displays webapp's version and exits.")

-- Override the error function.
function cli_error(msg, noprint)
  local name = colors("%{dim white}Warning%{reset}")
  local err = colors("%{red}Command not recognized.%{reset}")
  local msg = name .. ": " .. err .. " Run with --help for usage."

  if not noprint then
    print("\n" .. msg .. "\n")
  end

  return nil, msg
end

-- Parses from _G['arg'].
local args = cli:parse_args()
local EMPTY = ""

if #arg == 0 then
  return cli_error("", false)
elseif not args then
  return
end

-- Checking for flags: is -v or --version set?
if args["v"] then
  local msg = {
    colors("Web Application Builder"),
    colors("%{greenbg black} webapp %{reset} Version - 1.0.0-wip"),

    EMPTY,

    colors("%{yellow}Usage: webapp <task_name>")
  }

  return print("\n" .. table.concat(msg, "\n") .. "\n")
end
