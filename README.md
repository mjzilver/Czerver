# Czerver

**Czerver** is a simple HTTP server written in C.
It supports **static files**, **HTML templates** and **variable injection**.

Tested on **macOS** and **Linux** (UNIX only).

---

## Features

* Serves static files (HTML, CSS, JS) from `/html`
* Automatic **route registration** for everything under `/html`
* **Template inheritance** via `_content_` placeholders
* **Variable injection** using `{{ variable_name }}`

---

## Example Templates

### `./html/list.html`

```html
% ./html/nav.html %

<h1>This is a list of items to show variable templating</h1>
<ul>
    <li> {{ item_one }} </li>
    <li> {{ item_two }} </li>
    <li> {{ item_three }} </li>
</ul>
```

### `./html/nav.html`

```html
% ./html/header.html %

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

### `./html/base.html`

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

