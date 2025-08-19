if new_todo ~= "" then
    append_to_arr("todos", new_todo)
end

return redirect("/todos.html")
