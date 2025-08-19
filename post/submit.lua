local item_one = item_one or "NULL"
local item_two = item_two or "NULL"
local item_three = item_three or "NULL"

dict_replace("item_one", item_one)
dict_replace("item_two", item_two)
dict_replace("item_three", item_three)

return redirect("/list.html")
