local i
local rand=0
for i=1,10000000000 do
  rand = rand+math.random()
  --print(i .. " hello world")
end
print(rand)
