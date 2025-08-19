if todo ~= "" then
    remove_from_arr("todos", todo)
end

return redirect("/todos.html")
