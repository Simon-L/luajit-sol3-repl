-- fix for xml: https://gist.githubusercontent.com/ossie-git/ffddb4fd619c93db6baa45b62a65b89b/raw/09cc71a13ed3c7e772e51bba01c0602b1e7504c8/xml-1.1.3-1.rockspec
-- luarocks build <above rockspec>
-- luarocks install lua-requests
-- luarocks install lbase64

-- usage: echo "print'hello world from base64!'" | lua send-code.lua

requests = require('requests')
base64 = require('base64')

local size = 2^13      -- good buffer size (8K)
while true do
  local block = io.read(size)
  if not block then break end
  print(block)
  data = {code = base64.encode(block)}
  response = requests.post{'http://127.0.0.1:8888/api/eval', data = data}
  print(response.status_code)
end
