# Czerver

**Czerver** is a simple HTTP server written in C.

Tested on **macOS** and **Linux** (UNIX only).

---

## Features

- Serves static files (HTML, CSS, JS) from `/public`
- Automatic **route registration** for everything under `/public`
- **Template inheritance** via `_content_` placeholders
- **Variable injection** using `{{ variable_name }}`
- **For loops** using `#for item in items`
- **Post requests** handled via Lua
- **API handlers** both POST and GET requests using C functions
- **Custom JSON parser** Used for API endpoints

## Custom datastructures
- Dictionary
- Arraylist
- Arena Allocator

---

## Example Templates

### `./public/list.html`

```html
% ./public/nav.html %

<h1>This is a list of items to show variable templating</h1>
<ul>
    <li> {{ item_one }} </li>
    <li> {{ item_two }} </li>
    <li> {{ item_three }} </li>
</ul>
```

### `./public/nav.html`

```html
% ./public/header.html %

<nav>
    <a class="nav_item" href="/">Home</a>
    <a class="nav_item" href="/about.html">About</a>
    <a class="nav_item" href="/list.html">List</a>
    <a class="nav_item" href="/recipes/">Recipes</a>
</nav>

<div class="main_content">
    _content_
</div>
```

### `./public/base.html`

```html
<!DOCTYPE html>
<html>

<head>
    <title> {{ site_title }} </title>
    <link rel="stylesheet" href="/css/style.css">
    <script src="/js/main.js"></script>
</head>

<body>
    _content_
</body>

</html>
```

This allows **nested templates**:

* `list.html` shows a simple list with injected variables and uses nav as parent
* `nav.html` fills `_content_` with navigation and uses base as parent
* `base.html` provides the global structure

## Handling POST Requests with Lua

You can handle POST requests and update variables globally using Lua.

### `./public/form.html`

```html
% ./public/nav.html %

<form action="/submit.lua" method="POST">
    <label for="item_one">First list item:</label>
    <input type="text" id="item_one" name="item_one"><br><br>

    <label for="item_two">Second list item:</label>
    <input type="text" id="item_two" name="item_two"><br><br>

    <label for="item_three">Third list item:</label>
    <input type="text" id="item_three" name="item_three"><br><br>

    <input type="submit" value="Send">
</form>
```

### `./post/submit.lua`


```lua
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
```

### Explanation:

- Lua receives form data automatically as global variables.
- `set_var(key, value)` updates the global variable dictionary.
- `redirect(url)` sends the user to a new page after the POST.