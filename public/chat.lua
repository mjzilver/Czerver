local html = [[
% ./public/nav.html %
    <div class="chat-messages">
]]

local chat_messages = get_arr("chat_messages")
if chat_messages then
    for i, v in ipairs(chat_messages) do
        html = html .. '<div class="message">' .. v .. '</div>\n'
    end
end

html = html .. [[
    </div>

    <form action="/add_chat_message.lua" method="POST">
        <input type="text" name="new_message" placeholder="Write your message..." required>
        <input type="submit" value="Submit">
    </form>
]]


return html
