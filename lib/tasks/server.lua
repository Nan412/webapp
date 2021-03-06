-- String helpers
function string.ends(String,End)
   return End=='' or string.sub(String,-string.len(End))==End
end

function string.trim(String)
    return string.gsub(String, "^%s*(.-)%s*$", "%1")
end

function string.split(str, pat)
    local t = {}

    if str then
      string.gsub(str, pat, function(val)
          val = string.trim(val)
          table.insert(t, val)
      end)
    end

    return t
end

HttpServer = {}
HttpServer.prototype = { host = "*", port = 0, default = { "index.html", },
    mimetypes = {
        [".pdf"] = "application/pdf",
        [".ps"] = "application/postscript",
        [".gz"] = "application/x-gzip",
        [".swf"] = "application/x-shockwave-flash",
        [".tar.gz"] = "application/x-tgz",
        [".tgz"] = "application/x-tgz",
        [".tar"] = "application/x-tar",
        [".zip"] = "application/zip",
        [".mp3"] = "audio/mpeg",
        [".m3u"] = "audio/x-mpegurl",
        [".wma"] = "audio/x-ms-wma",
        [".wax"] = "audio/x-ms-wax",
        [".ogg"] = "application/ogg",
        [".wav"] = "audio/x-wav",
        [".gif"] = "image/gif",
        [".jpg"] = "image/jpeg",
        [".jpeg"] = "image/jpeg",
        [".png"] = "image/png",
        [".css"] = "text/css",
        [".html"] = "text/html",
        [".htm"] = "text/html",
        [".js"] = "text/javascript",
        --[".appcache"] = "text/cache-manifest",
        [".log"] = "text/plain",
        [".conf"] = "text/plain",
        [".text"] = "text/plain",
        [".txt"] = "text/plain",
        [".dtd"] = "text/xml",
        [".xml"] = "text/xml",
        [".mpeg"] = "video/mpeg",
        [".mpg"] = "video/mpeg",
        [".mov"] = "video/quicktime",
        [".qt"] = "video/quicktime",
        [".avi"] = "video/x-msvideo",
        [".asf"] = "video/x-ms-asf",
        [".asx"] = "video/x-ms-asf",
        [".wmv"] = "video/x-ms-wmv",
        [".bz2"] = "application/x-bzip",
        [".tbz"] = "application/x-bzip-compressed-tar",
        [".tar.bz2"] = "application/x-bzip-compressed-tar",
        
        -- Default mimetype
        [""] = "application/octet-stream",
    },
}
HttpServer.__meta = {}

-- Constructor
function HttpServer:new (args)
    this = {}
    -- Bring in all args
    for k,v in pairs(args or {}) do this[k] = v end
    -- Set this to be a metatable
    setmetatable(this, HttpServer.__meta)

    -- Set socket
    local socket = require("socket"), server

    -- Handle request
    function handle(client, req)
        --if not err then client:send("<html><head></head><body><marquee>404</marquee></body></html>".. "\r\n") end
        local parts = {}, file, data
 
        -- Set parts
        parts = string.split(req, ".[^ ]+")

        -- Set defaults
        if string.ends(parts[2], "/") then
            parts[2] = parts[2] .. "index.html"
        end

        -- Set to local directory
        parts[2] = "." .. parts[2]

        -- TODO: Break this out
        --
        local path = string.split(parts[2], "[^\.]+")
        mimetype = this.mimetypes["." .. path[#path]] or this.mimetypes[""]
        local headers = "HTTP/1.1 200 OK\nServer: Energize\nContent-Type: " .. mimetype .. "\r\n\n"

        -- Open file
        if not pcall(function()
            file = assert(io.open(parts[2], "rb"))
            data = file:read("*all")
            file:close()
        end) then
            headers = "HTTP/1.1 400 OK\nServer: Energize\nContent-Type: text/html\r\n\n"
            client:send("<html><head></head><body><marquee>404</marquee></body></html>".. "\r\n")
            return nil
        end

        ---- Show 404
        if not err then client:send(headers .. data .. "\r\n") end
    end

    -- Start method to kick off the server
    this.start = function()
        local client, line, err
        server = assert(socket.bind(this.host, this.port))
        -- Normalization, reset the port to whatever we are actualy bound to
        this.host, this.port = server:getsockname()

        -- Friendly messages
        print(string.format("Started server running at: http://%s:%d", this.host, this.port))
        print("Waiting for connections, press CTRL+C twice to stop.")

        -- Loop
        while 1 do
            client = server:accept()
            client:settimeout(10)

            line, err = client:receive()

            print(line)

            handle(client, line)

            client:close()
        end
    end

    return this
end

-- Emulate prototypical inheritance
HttpServer.__meta.__index = function (table, key)
  return HttpServer.prototype[key]
end

http = HttpServer:new { host = "0.0.0.0", port = 8000 }
http.start()
