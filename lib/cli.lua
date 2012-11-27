-- Include modules.
local cli = require "cliargs"
local colors = require "ansicolors"
local tlua = require "tlua"

-- Set the name to show up.
cli:set_name("webapp")

-- Display the version.
cli:add_flag("-v, --version", "Displays webapp's version and exits.")

-- Parses from _G['arg']
local args = cli:parse_args()

if not args then
  -- something wrong happened and an error was printed
  return
end

-- Checking for flags: is -v or --version set?
if args["v"] then
  return print(colors("%{dim white}webapp%{reset} - WebApp Builder - 1.0.0\n"))
end
