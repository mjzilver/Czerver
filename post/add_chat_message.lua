if new_message ~= "" then
    append_to_arr("chat_messages", new_message);
end

return redirect("/chat.lua")
