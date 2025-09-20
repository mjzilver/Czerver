if item_one ~= "" then
    set_var("item_one", item_one)
end

if item_two ~= "" then
    set_var("item_two", item_two)
end

if item_three ~= "" then
    set_var("item_three", item_three)
end

return redirect("/list.html")
