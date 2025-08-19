local name = name or "NULL"
local age = age or "NULL"

dict_replace("item_one", name)

return string.format([[
<!DOCTYPE html>
<html>
<body>
    <h1>Hello %s!</h1>
    <p>You are %s years old.</p>
</body>
</html>
]], name, age)
