-- Include modules. -----------------------------------------------------------

local cli = require "cliargs"
local colors = require "ansicolors"
local tlua = require "tlua"

-- Set the name to show up. ---------------------------------------------------

cli:set_name("webapp")

-- Display the version. -------------------------------------------------------

cli:add_flag("-v, --version", "Displays webapp's version and exits.")

-- Override the error function. -----------------------------------------------

function cli_error(msg, noprint, cli)
  local name = colors("%{dim white}" .. (cli.name or "") .. "%{reset}")
  local err = colors("%{red}Command not recognized.%{reset}")

  local msg = name .. ": " .. err .. " Run with --help for usage."

  if not noprint then
    print("\n" .. msg .. "\n")
  end

  return nil, msg
end

-- Parses from _G['arg']. -----------------------------------------------------

local args = cli:parse_args()

if not args or #arg == 0 then
  return cli_error("", false, cli)
end

-- Checking for flags: is -v or --version set? --------------------------------

if args["v"] then
  local msg = colors("%{dim white}webapp%{reset} - WebApp Builder - 1.0.0")
  return print("\n" .. msg .. "\n")
end
