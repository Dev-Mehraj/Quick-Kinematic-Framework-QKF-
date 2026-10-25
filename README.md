# Quick Kinetic Framework (QKF)

[![Language](https://img.shields.io/badge/Language-QKF%20%28.qkf%29-blue.svg)](file:///e:/Learnings_Projs/Quick%20Kinetic%20Framework)
[![Toolchain](https://img.shields.io/badge/Engine-C%2B%2B20%20%2F%20MinGW-green.svg)](file:///e:/Learnings_Projs/Quick%20Kinetic%20Framework)
[![Output](https://img.shields.io/badge/Target-Native%20Desktop%20%28.exe%29-orange.svg)](file:///e:/Learnings_Projs/Quick%20Kinetic%20Framework)
[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](file:///e:/Learnings_Projs/Quick%20Kinetic%20Framework/LICENSE)

**Quick Kinetic Framework (QKF)** is a fast, expressive programming language and native desktop application maker. It compiles high-level declarative UI code and business logic directly into standalone, high-performance native desktop executables (`.exe`), without HTML/browser bloat.

---

## Key Highlights

- ⚡ **Native Desktop Application Maker**: Produces real Win32 desktop binaries (`.exe`), not browser/webview mockups.
- 📐 **Strict Semicolon Discipline**: Enforces `;` on every single statement, declaration, and property assignment with pinpoint line/column error diagnostics.
- 🪟 **Native Window Controls**:
  - **Windows (`style: "windows"`)**: Standard top-right Minimize, Maximize, and Close buttons directly integrated with the Windows desktop window manager.
  - **macOS (`style: "macos"`)**: Top-left traffic lights (🔴 Close, 🟡 Minimize, 🟢 Maximize) with custom borderless drag physics.
  - **Linux (`style: "linux"`)**: Native window decorations.
- 🧭 **Recursive Desktop Toolbars & Menus**: Native system Menu Bars with arbitrarily deep sub-items (`Toolbar > File > Save > Quick Save`) wired directly to `onClick` event triggers.
- 🏷️ **Component Identifiers**: All UI components and menu items track `id` and `name` attributes for full control and future styling.
- 📦 **Manifest Configuration (`config.gpq`)**: Clean declarative manifest file for project configuration, build targets, and templates.
- 🛠️ **Project Scaffolder (`qkf-cloner`)**: Dedicated CLI tool to scaffold, validate, and manage QKF projects.

---

## Architecture Overview

```
Quick Kinetic Framework
├── bin/
│   ├── qkf.exe               # Main compiler, validator, and runner CLI
│   ├── qkf-cloner.exe        # Dedicated project scaffolding tool
│   ├── MyDesktopApp.exe      # Compiled native Windows desktop app
│   └── MacApp.exe            # Compiled native macOS-style desktop app
├── include/qkf/
│   ├── token.hpp             # Lexer token definitions & keyword registry
│   ├── lexer.hpp             # Tokenizer engine
│   ├── ast.hpp               # AST nodes (Statements, Expressions, Window, UI)
│   ├── parser.hpp            # Recursive descent parser with strict ';' checking
│   ├── native_gen.hpp        # Win32 C++ native desktop code generator
│   ├── codegen.hpp           # Target code generator
│   ├── config_gpq.hpp        # .gpq manifest parser
│   ├── cloner.hpp            # Project template cloner
│   └── driver.hpp            # Multi-file compilation driver & GCC linker
├── src/                      # Source implementations
├── examples/                 # Sample QKF desktop applications and scripts
├── config.gpq                # Project configuration manifest
└── build.bat                 # One-click Windows build script
```

---

## Language Syntax & Features

### 1. Function Declarations

#### A. Returning Functions
Functions with return values declare `return = <type>`. Every line inside requires a semicolon `;`, except the **final tail expression**, which serves as the return value:
```qkf
f add(a: int, b: int) return = int {
    let result = a + b;
    result
};

f getGreeting(user: string) return = string {
    "Hello, " + user + "!"
};
```

#### B. Void Functions
Void functions declare `void`. **Every single statement** must end with a semicolon `;`:
```qkf
f logMessage(msg: string) void {
    print("LOG: " + msg);
};
```

### 2. Standard I/O & Logging
Built-in native logging and console interaction:
- `print(message);` - Standard application logging.
- `warnlog(message);` - Warning level log.
- `errorlog(message);` - Error level log (triggers native popup dialog on desktop).
- `input(prompt);` - Console input read.

### 3. Native Desktop Window (`window`)
Declare native desktop windows with properties and lifecycle methods:
```qkf
window MainWindow {
    id: "main_window";
    name: "AppMainWindow";
    title: "My Application";
    width: 960;
    height: 640;

    // Window controls style: "windows" (top-right) or "macos" (top-left)
    WindowControls {
        id: "sys_controls";
        name: "WindowSystemControls";
        style: "windows";
    };
};

f main() void {
    MainWindow.create();
    MainWindow.center();
    MainWindow.start();
};
```

#### Available Window Methods:
| Method | Description |
| :--- | :--- |
| `name.create();` | Instantiates and registers the native Win32 window |
| `name.start();` | Displays window and starts the native message loop |
| `name.center();` | Centers window on primary monitor |
| `name.setSize(w, h);` | Dynamically updates window width and height |
| `name.setPosition(x, y);` | Moves window to specified screen coordinates |
| `name.setTitle(text);` | Sets the window caption text |
| `name.minimize();` | Minimizes window to taskbar |
| `name.maximize();` | Maximizes window to fullscreen |
| `name.restore();` | Restores window to normal size |
| `name.close();` | Destroys window and posts quit message |

---

### 4. Recursive Toolbars & Menus
Toolbars compile into real Win32 desktop menu bars with nested cascading submenus and `onClick` handlers:
```qkf
Toolbar {
    id: "main_toolbar";
    name: "ApplicationToolbar";

    File {
        id: "menu_file";
        name: "FileMenu";
        label: "File";

        Save {
            id: "menu_save";
            name: "SaveSubMenu";
            label: "Save";

            QuickSave {
                id: "btn_quicksave";
                name: "QuickSaveButton";
                label: "Quick Save";
                onClick: {
                    print("Quick Save action executed natively!");
                };
            };

            ExportAll {
                id: "btn_export";
                name: "ExportAllButton";
                label: "Export All...";
                onClick: {
                    print("Export All triggered!");
                };
            };
        };

        Exit {
            id: "btn_exit";
            name: "ExitButton";
            label: "Exit Application";
            onClick: {
                print("Exiting application.");
            };
        };
    };

    Edit {
        id: "menu_edit";
        name: "EditMenu";
        label: "Edit";

        Undo {
            id: "btn_undo";
            name: "UndoButton";
            label: "Undo";
            onClick: {
                print("Undo executed!");
            };
        };
    };
};
```

---

### 5. Control Flow & Error Handling
```qkf
// Conditionals
if (count > 10) {
    print("Greater");
} else {
    print("Lesser");
};

// Loops
while (running) {
    doTask();
};

do {
    attemptAction();
} while (retrying);

for (let i = 0; i < 10; i = i + 1) {
    print("Index: " + i);
};

// Error Handling
try {
    throw "Fatal connection failure";
} catch (err) {
    errorlog("Caught: " + err);
} finally {
    cleanup();
};
```

---

### 6. Module Imports & C++ Clones
```qkf
// Whole module import
import "components.header";

// Selective import with alias
take "formatDate" from "utils.dates" use fmt;

// Namespaces & Enums
namespace Graphics {
    enum Color {
        Red = 1,
        Green = 2,
        Blue = 3
    };
} // namespace Graphics

// Classes with access specifiers
class ButtonBase {
public:
    let isEnabled: bool = true;
    f click() void {
        print("Clicked");
    };
};
```

---

## Configuration Manifest (`config.gpq`)

QKF projects are configured using `.gpq` files:
```toml
# Quick Kinetic Framework Configuration
[project]
    name = "MyDesktopApp";
    version = "1.0.0";
    entry = "src/main.qkf";

[build]
    target = "native";
    output = "bin/MyDesktopApp.exe";
    strict_semicolon = "true";

[cloner]
    template = "desktop-app";
    scaffold_version = "1.0.0";
```

---

## Toolchain Usage

### Compiling and Running
```bash
# Compile project defined in config.gpq
qkf

# Build native desktop executable (.exe)
qkf build examples/desktop_app.qkf -o bin/MyApp.exe

# Build and immediately launch desktop app
qkf run examples/desktop_app.qkf

# Validate syntax & strict semicolons across all modules without building
qkf check examples/desktop_app.qkf

# List all registered language keywords
qkf keywords
```

### Project Scaffolding (`qkf-cloner`)
```bash
# Scaffold a new QKF project from template
qkf-cloner new MyNewApp --template desktop-app

# Validate an existing config.gpq manifest
qkf-cloner validate config.gpq
```

---

## Building the Toolchain from Source

Requires **MinGW-w64** with C++20 support (`g++`).

Run the included build script:
```cmd
build.bat
```
Or compile directly:
```cmd
g++ -std=c++20 -O2 -Iinclude src/compiler/*.cpp src/config/*.cpp src/cloner/*.cpp src/main_qkf.cpp -o bin/qkf.exe
g++ -std=c++20 -O2 -Iinclude src/compiler/*.cpp src/config/*.cpp src/cloner/*.cpp src/main_cloner.cpp -o bin/qkf-cloner.exe
```

---

## Roadmap & Status

- [x] Lexer with full operator set & keyword registry
- [x] Parser with strict semicolon enforcement across all statements & declarations
- [x] Multi-file import resolution & module caching
- [x] `config.gpq` manifest parsing and validation
- [x] `qkf-cloner` project scaffolding tool
- [x] Window keyword & lifecycle methods (`create`, `start`, `center`, `setSize`, `close`)
- [x] Real Win32 Native Desktop Application Generator (`.exe`)
- [x] Windows top-right controls & macOS top-left traffic lights
- [x] Native recursive toolbars & nested cascading menus with `onClick` triggers
- [ ] Component Styling Engine *(Scheduled as the final phase per project plan)*

---

## License

This project is licensed under the **MIT License**. See the [LICENSE](file:///e:/Learnings_Projs/Quick%20Kinetic%20Framework/LICENSE) file for details.
